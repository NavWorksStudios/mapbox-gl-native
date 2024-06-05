#include <mbgl/renderer/buckets/fill_extrusion_bucket.hpp>
#include <mbgl/programs/fill_extrusion_program.hpp>
#include <mbgl/renderer/bucket_parameters.hpp>
#include <mbgl/style/layers/fill_extrusion_layer_impl.hpp>
#include <mbgl/renderer/layers/render_fill_extrusion_layer.hpp>
#include <mbgl/util/math.hpp>
#include <mbgl/util/constants.hpp>

#include <mapbox/earcut.hpp>

#include <cassert>

#include "mbgl/nav/nav_unity_bridge.hpp"

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
                       const std::map<std::string, Immutable<style::LayerProperties>>& layerPaintProperties,
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

        std::size_t totalVertices = 0;

        for (const auto& ring : polygon) {
            totalVertices += ring.size();
            if (totalVertices > std::numeric_limits<uint16_t>::max())
                throw GeometryTooLongException();
        }

        if (totalVertices == 0) continue;

        std::vector<uint32_t> flatIndices;
        flatIndices.reserve(totalVertices);

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
            std::size_t nVertices = ring.size();

            if (nVertices == 0)
                continue;

            std::size_t edgeDistance = 0;

            for (std::size_t i = 0; i < nVertices; i++) {
                
                // 1. [top/bottom surface]
                // pick this point as the [top/bottom surface]
                // with the normal value (0,0,1)
                const auto& p1 = ring[i];
                vertices.emplace_back(FillExtrusionProgram::layoutVertex(p1, 0, 0, 1, 1, edgeDistance, 0, 0, 1, 0));
                flatIndices.emplace_back(triangleIndex);
                triangleIndex += 1;

                // 2. [side suface]
                // pick this and the previous point together to make a [side suface]
                // with the normal value (perp.x, perp.y, 0)
                if (i != 0) {
                    //      (p3)              (p4)
                    //        \                /
                    //  perp23 \              / perp14
                    //          \            /
                    //          (p2)-------(p1)
                    //    perp2      perp12     perp1
                    
                    const auto& p2 = ring[(i-1)];
                    const auto& p3 = (i-2) < 0 ? ring[(i-2+nVertices)] : ring[(i-2)];
                    const auto& p4 = (i+1) >= nVertices ? ring[(i+1-nVertices)] : ring[(i+1)];

                    const auto d1 = convertPoint<double>(p1);
                    const auto d2 = convertPoint<double>(p2);
                    const auto d3 = convertPoint<double>(p3);
                    const auto d4 = convertPoint<double>(p4);

                    // 3 surfaces
                    const Point<double> perp12 = util::unit(util::perp(d1 - d2));
                    const Point<double> perp23 = util::unit(util::perp(d2 - d3));
                    const Point<double> perp14 = util::unit(util::perp(d1 - d4));
                    // 2 corner
                    const Point<double> perp1((perp12.x+perp14.x)*.5, (perp12.y+perp14.y)*.5);
                    const Point<double> perp2((perp12.x+perp23.x)*.5, (perp12.y+perp23.y)*.5);
                    
                    const auto edgeLength = util::dist<int16_t>(d1, d2);
                    if (edgeDistance + edgeLength > std::numeric_limits<int16_t>::max()) edgeDistance = 0;

                    vertices.emplace_back(FillExtrusionProgram::layoutVertex(p1, perp12.x, perp12.y, 0, 0, edgeDistance, perp1.x, perp1.y, 0, edgeLength|0x1));
                    vertices.emplace_back(FillExtrusionProgram::layoutVertex(p1, perp12.x, perp12.y, 0, 1, edgeDistance, perp1.x, perp1.y, 0, edgeLength|0x1));

                    edgeDistance += edgeLength;

                    vertices.emplace_back(FillExtrusionProgram::layoutVertex(p2, perp12.x, perp12.y, 0, 0, edgeDistance, perp2.x, perp2.y, 0, edgeLength&0xFFFE));
                    vertices.emplace_back(FillExtrusionProgram::layoutVertex(p2, perp12.x, perp12.y, 0, 1, edgeDistance, perp2.x, perp2.y, 0, edgeLength&0xFFFE));
                    
                    // ┌──────┐
                    // │ 0  1 │ Counter-Clockwise winding order.
                    // │      │ Triangle 1: 0 => 2 => 1
                    // │ 2  3 │ Triangle 2: 1 => 2 => 3
                    // └──────┘
                    triangles.emplace_back(triangleIndex, triangleIndex + 2, triangleIndex + 1);
                    triangles.emplace_back(triangleIndex + 1, triangleIndex + 2, triangleIndex + 3);
                    
                    triangleIndex += 4;
                    triangleSegment.vertexLength += 4;
                    triangleSegment.indexLength += 6;
                }
            }
        }

        // earcut for the top/bottom surface
        std::vector<uint32_t> indices = mapbox::earcut(polygon);

        std::size_t nIndices = indices.size();
        assert(nIndices % 3 == 0);

        for (std::size_t i = 0; i < nIndices; i += 3) {
            // Counter-Clockwise winding order.
            triangles.emplace_back(flatIndices[indices[i]], flatIndices[indices[i + 2]],
                                   flatIndices[indices[i + 1]]);
        }

        triangleSegment.vertexLength += totalVertices;
        triangleSegment.indexLength += nIndices;
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
    }

    for (auto& pair : paintPropertyBinders) {
        pair.second.upload(uploadPass);
    }

    uploaded = true;
}

void FillExtrusionBucket::nav_upload(const CanonicalTileID& canonical, const std::string& layerId, const std::string& sourceLayer) {
    if (hasData()) {
        const nav::layer::ExtrusionBucket param = {
            {&canonical, nav::mb::layerRenderIndex(layerId), layerId.c_str(), sourceLayer.c_str()},
            {(const uint16_t*) vertices.data(), (int) vertices.elements()},
            {triangles.data(), (int) triangles.bytes() / 2}
        };

        nav::layer::onAddExtrusionBucket(&param);
    }
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
                                 const std::string& layerID, const ImagePositions& imagePositions) {
    auto it = paintPropertyBinders.find(layerID);
    if (it != paintPropertyBinders.end()) {
        it->second.updateVertexVectors(states, layer, imagePositions);
        uploaded = false;
    }
}

} // namespace mbgl
