#pragma once

#include <mbgl/renderer/bucket.hpp>
#include <mbgl/tile/geometry_tile_data.hpp>
#include <mbgl/gfx/vertex_buffer.hpp>
#include <mbgl/gfx/index_buffer.hpp>
#include <mbgl/programs/segment.hpp>
#include <mbgl/programs/fill_program.hpp>
#include <mbgl/style/layers/fill_layer_properties.hpp>

#include <vector>

namespace mbgl {

class BucketParameters;
class RenderFillLayer;

class FillBucket final : public Bucket {
public:
    ~FillBucket() override;
    using PossiblyEvaluatedLayoutProperties = style::FillLayoutProperties::PossiblyEvaluated;

    FillBucket(const PossiblyEvaluatedLayoutProperties& layout,
               const std::map<nav::stringid, Immutable<style::LayerProperties>>& layerPaintProperties,
               float zoom,
               uint32_t overscaling);

    void addFeature(const GeometryTileFeature&,
                    const GeometryCollection&,
                    const mbgl::ImagePositions&,
                    const PatternLayerMap&,
                    std::size_t,
                    const CanonicalTileID&) override;
    
    void addFeature(const GeometryTileFeature& feature,
                    const GeometryCollection& geometryCollection,
                    const std::vector<std::vector<int16_t>>& conditions,
                    const ImagePositions& patternPositions,
                    const PatternLayerMap& patternDependencies,
                    std::size_t index,
                    const CanonicalTileID& canonical) { addFeature(feature, geometryCollection, patternPositions,
                                                                   patternDependencies, index, canonical); };

    bool hasData() const override;

    void upload(gfx::UploadPass&) override;

    float getQueryRadius(const RenderLayer&) const override;

    void update(const FeatureStates&, const GeometryTileLayer&, const nav::stringid&, const ImagePositions&) override;

    gfx::VertexVector<FillLayoutVertex> vertices;
    gfx::IndexVector<gfx::Lines> lines;
    gfx::IndexVector<gfx::Triangles> triangles;
    SegmentVector<FillAttributes> lineSegments;
    SegmentVector<FillAttributes> triangleSegments;

    optional<gfx::VertexBuffer<FillLayoutVertex>> vertexBuffer;
    optional<gfx::IndexBuffer> lineIndexBuffer;
    optional<gfx::IndexBuffer> triangleIndexBuffer;

    std::map<nav::stringid, FillProgram::Binders> paintPropertyBinders;
};

} // namespace mbgl
