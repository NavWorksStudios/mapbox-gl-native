#include <mbgl/renderer/buckets/fill_extrusion_bucket.hpp>
#include <mbgl/programs/fill_extrusion_program.hpp>
#include <mbgl/renderer/bucket_parameters.hpp>
#include <mbgl/style/layers/fill_extrusion_layer_impl.hpp>
#include <mbgl/renderer/layers/render_fill_extrusion_layer.hpp>
#include <mbgl/util/math.hpp>
#include <mbgl/util/constants.hpp>

#include <mapbox/earcut.hpp>

#include <cassert>

namespace mapbox {
namespace util {
template <>
struct nth<0, mbgl::GeometryCoordinate> {
    static int64_t get(const mbgl::GeometryCoordinate& t) {
        return t.x;
    };
};

template <>
struct nth<1, mbgl::GeometryCoordinate> {
    static int64_t get(const mbgl::GeometryCoordinate& t) {
        return t.y;
    };
};
} // namespace util
} // namespace mapbox

namespace mbgl {

using namespace style;

struct GeometryTooLongException : std::exception {};

FillExtrusionBucket::FillExtrusionBucket(const FillExtrusionBucket::PossiblyEvaluatedLayoutProperties&,
                       const std::map<nav::stringid, Immutable<style::LayerProperties>>& layerPaintProperties,
                       const float zoom,
                       const uint32_t) {
    for (const auto& pair : layerPaintProperties) {
        paintPropertyBinders.emplace(
            std::piecewise_construct,
            std::forward_as_tuple(pair.first),
            std::forward_as_tuple(
                getEvaluated<FillExtrusionLayerProperties>(pair.second),
                zoom));
    }
}

FillExtrusionBucket::~FillExtrusionBucket() = default;

void FillExtrusionBucket::addFeature(const GeometryTileFeature& feature,
                                     const GeometryCollection& geometry,
                                     const ImagePositions& patternPositions,
                                     const PatternLayerMap& patternDependencies,
                                     std::size_t index,
                                     const CanonicalTileID& canonical) {
    for (auto& polygon : classifyRings(geometry)) {
        // Optimize polygons with many interior rings for earcut tesselation.
        limitHoles(polygon, 500);

        const bool hasHole = polygon.size() > 1;
        nav_clipTile8192(polygon);

        std::size_t totalVertices = 0;

        for (const auto& ring : polygon) {
            totalVertices += ring.size();
            if (totalVertices > std::numeric_limits<uint16_t>::max())
                throw GeometryTooLongException();
        }

        if (totalVertices == 0) continue;

        std::vector<uint32_t> flatIndices;
        flatIndices.reserve(totalVertices);
        std::vector<uint32_t> flatRingIndices;

        std::size_t startVertices = vertices.elements();

        if (triangleSegments.empty() ||
            triangleSegments.back().vertexLength + (5 * (totalVertices - 1) + 1) > std::numeric_limits<uint16_t>::max()) {
            triangleSegments.emplace_back(startVertices, triangles.elements());
        }

        auto& triangleSegment = triangleSegments.back();
        assert(triangleSegment.vertexLength <= std::numeric_limits<uint16_t>::max());
        uint16_t triangleIndex = triangleSegment.vertexLength;

        assert(triangleIndex + (5 * (totalVertices - 1) + 1) <= std::numeric_limits<uint16_t>::max());

        for (const auto& ring : polygon) {
            flatRingIndices.emplace_back(flatIndices.size());
            
            std::size_t nVertices = ring.size();
            if (nVertices == 0) continue;

            std::size_t edgeDistance = 0;
            
            // 1. all of the wall surface within this ring
            for (std::size_t i = 0; i < nVertices; i++) {
                // [roof surface]
                // pick this point as a [roof vertex] with the normal value (0,0,1)
                const auto& p1 = ring[i];
                vertices.emplace_back(FillExtrusionProgram::layoutVertex(p1, 0, 0, 1, 1, edgeDistance));
                flatIndices.emplace_back(triangleIndex);
                triangleIndex += 1;
                
                // [side surface]
                // pick this and the previous point together to make a [side surface] with the normal value (perp.x, perp.y, 0)
                if (i != 0) {
                    const auto& p2 = ring[(i-1)];
                    const auto d1 = convertPoint<double>(p1);
                    const auto d2 = convertPoint<double>(p2);
                    const Point<double> perp12 = util::unit(util::perp(d1 - d2));
                    
                    const auto edgeLength = util::dist<int16_t>(d1, d2);
                    if (edgeDistance + edgeLength > std::numeric_limits<int16_t>::max()) edgeDistance = 0;
                    
                    vertices.emplace_back(FillExtrusionProgram::layoutVertex(p1, perp12.x, perp12.y, 0, 0, edgeDistance));
                    vertices.emplace_back(FillExtrusionProgram::layoutVertex(p1, perp12.x, perp12.y, 0, 1, edgeDistance));
                    
                    edgeDistance += edgeLength;
                    
                    vertices.emplace_back(FillExtrusionProgram::layoutVertex(p2, perp12.x, perp12.y, 0, 0, edgeDistance));
                    vertices.emplace_back(FillExtrusionProgram::layoutVertex(p2, perp12.x, perp12.y, 0, 1, edgeDistance));
                    
                    if (!nav_isTileClippingSide8192(p1, p2))
                    {
                        // ┌──────┐
                        // │ 0  1 │ Counter-Clockwise winding order.
                        // │      │ Triangle 1: 0 => 2 => 1
                        // │ 2  3 │ Triangle 2: 1 => 2 => 3
                        // └──────┘
                        
                        // Counter-Clockwise winding order.
                        triangles.emplace_back(triangleIndex, triangleIndex + 2, triangleIndex + 1);
                        triangles.emplace_back(triangleIndex + 1, triangleIndex + 2, triangleIndex + 3);
                        
                        // Reflection with clockwise winding order.
                        reflectionTriangles.emplace_back(triangleIndex, triangleIndex + 1, triangleIndex + 2);
                        reflectionTriangles.emplace_back(triangleIndex + 1, triangleIndex + 3, triangleIndex + 2);
                        
                        triangleSegment.indexLength += 6;
                    }
                    
                    triangleIndex += 4;
                    triangleSegment.vertexLength += 4;
                }
            }
        }
        
        {
            // 2. the roof surface within flat indices
            // [roof surface]
            auto addFlatSurface = [&] (const std::vector<uint32_t>& indices, size_t baseIndex) {
                std::size_t nIndices = indices.size();
                assert(nIndices % 3 == 0);
                
                // for all top surface
                for (std::size_t i = 0; i < nIndices; i += 3) {
                    // Counter-Clockwise winding order.
                    triangles.emplace_back(flatIndices[baseIndex + indices[i]],
                                           flatIndices[baseIndex + indices[i + 2]],
                                           flatIndices[baseIndex + indices[i + 1]]);
                    
                    // Reflection with counter-clockwise winding order.
                    // 逆时针方向，因为，如果能看到顶面，一定是从内部看到背面。所以是正常顺序
                    reflectionTriangles.emplace_back(flatIndices[baseIndex + indices[i]],
                                                     flatIndices[baseIndex + indices[i + 2]],
                                                     flatIndices[baseIndex + indices[i + 1]]);
                }

                triangleSegment.indexLength += nIndices;
            };
            
            if (hasHole || polygon.size() == 1) {
                std::vector<uint32_t> indices = mapbox::earcut(polygon);
                addFlatSurface(indices, 0);
            } else {
                struct Collection {
                    Collection(const GeometryCoordinates& points) : points(points) { }
                    std::reference_wrapper<const GeometryCoordinates> points;
                    size_t size() const { return 1; }
                    const GeometryCoordinates& operator [] (size_t) const { return points; }
                    bool empty() const { return false; }
                };
                
                auto it = flatRingIndices.begin();
                for (auto& ring : polygon) {
                    std::vector<uint32_t> indices = mapbox::earcut(Collection(ring));
                    addFlatSurface(indices, *it++);
                }
            }
        }
        
        triangleSegment.vertexLength += totalVertices;
    }

    for (auto& pair : paintPropertyBinders) {
        const auto it = patternDependencies.find(pair.first);
        if (it != patternDependencies.end()){
            pair.second.populateVertexVectors(
                feature, vertices.elements(), index, patternPositions, it->second, canonical);
        } else {
            pair.second.populateVertexVectors(feature, vertices.elements(), index, patternPositions, {}, canonical);
        }
    }
}

void FillExtrusionBucket::upload(gfx::UploadPass& uploadPass) {
    if (!uploaded) {
        vertexBuffer = uploadPass.createVertexBuffer(std::move(vertices));
        indexBuffer = uploadPass.createIndexBuffer(std::move(triangles));
        reflectionIndexBuffer = uploadPass.createIndexBuffer(std::move(reflectionTriangles));
    }

    for (auto& pair : paintPropertyBinders) {
        pair.second.upload(uploadPass);
    }

    uploaded = true;
}

bool FillExtrusionBucket::hasData() const {
    return !triangleSegments.empty();
}

float FillExtrusionBucket::getQueryRadius(const RenderLayer& layer) const {
    const auto& evaluated = getEvaluated<FillExtrusionLayerProperties>(layer.evaluatedProperties);
    const std::array<float, 2>& translate = evaluated.get<FillExtrusionTranslate>();
    return util::length(translate[0], translate[1]);
}

void FillExtrusionBucket::update(const FeatureStates& states, const GeometryTileLayer& layer,
                                 const nav::stringid& layerID, const ImagePositions& imagePositions) {
    auto it = paintPropertyBinders.find(layerID);
    if (it != paintPropertyBinders.end()) {
        it->second.updateVertexVectors(states, layer, imagePositions);
        uploaded = false;
    }
}

} // namespace mbgl
