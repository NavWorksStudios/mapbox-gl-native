#pragma once

#include <mbgl/renderer/bucket.hpp>
#include <mbgl/map/mode.hpp>
#include <mbgl/tile/geometry_tile_data.hpp>
#include <mbgl/gfx/vertex_buffer.hpp>
#include <mbgl/gfx/index_buffer.hpp>
#include <mbgl/programs/segment.hpp>
#include <mbgl/programs/heatmap_program.hpp>
#include <mbgl/style/layers/heatmap_layer_properties.hpp>

namespace mbgl {

class BucketParameters;

class HeatmapBucket final : public Bucket {
public:
    HeatmapBucket(const BucketParameters&, const std::vector<Immutable<style::LayerProperties>>&);
    ~HeatmapBucket() override;

    void addFeature(const GeometryTileFeature&,
                    const GeometryCollection&,
                    const ImagePositions&,
                    const PatternLayerMap&,
                    std::size_t,
                    const CanonicalTileID&) override;
    bool hasData() const override;

    void upload(gfx::UploadPass&) override;
    void nav_upload_external(const CanonicalTileID& canonical, const std::string& layerID, const std::string& sourceLayer) override { }

    float getQueryRadius(const RenderLayer&) const override;

    gfx::VertexVector<HeatmapLayoutVertex> vertices;
    gfx::IndexVector<gfx::Triangles> triangles;
    SegmentVector<HeatmapAttributes> segments;

    optional<gfx::VertexBuffer<HeatmapLayoutVertex>> vertexBuffer;
    optional<gfx::IndexBuffer> indexBuffer;

    std::map<std::string, HeatmapProgram::Binders> paintPropertyBinders;

    const MapMode mode;
};

} // namespace mbgl
