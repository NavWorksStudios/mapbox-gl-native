#pragma once

#include <mbgl/style/types.hpp>
#include <mbgl/tile/geometry_tile_data.hpp>
#include <mbgl/tile/tile_id.hpp>
#include <mbgl/util/feature.hpp>
#include <mbgl/util/geo.hpp>
#include <mbgl/util/grid_index.hpp>
#include <mbgl/util/mat4.hpp>

#include <vector>
#include <string>
#include <unordered_map>

namespace mbgl {

class RenderedQueryOptions;
class RenderLayer;
class TransformState;
class SourceFeatureState;

class CollisionIndex;

class IndexedSubfeature {
public:
    IndexedSubfeature() = delete;
    IndexedSubfeature(std::size_t index_, nav::stringid sourceLayerName_, nav::stringid bucketName_, size_t sortIndex_)
        : index(index_)
        , sourceLayerName(std::move(sourceLayerName_))
        , bucketLeaderID(std::move(bucketName_))
        , sortIndex(sortIndex_)
        , bucketInstanceId(0)
        , collisionGroupId(0)
    {}
    
    
    IndexedSubfeature(const IndexedSubfeature& other, uint32_t bucketInstanceId_, uint16_t collisionGroupId_)
        : index(other.index)
        , sourceLayerName(other.sourceLayerName)
        , bucketLeaderID(other.bucketLeaderID)
        , sortIndex(other.sortIndex)
        , bucketInstanceId(bucketInstanceId_)
        , collisionGroupId(collisionGroupId_)
    {}
    size_t index;
    nav::stringid sourceLayerName;
    nav::stringid bucketLeaderID;
    size_t sortIndex;

    // Only used for symbol features
    uint32_t bucketInstanceId;
    uint16_t collisionGroupId;
};

using FeatureSortOrder = std::shared_ptr<const std::vector<size_t>>;

class DynamicFeatureIndex {
public:
    ~DynamicFeatureIndex();
    void query(std::unordered_map<nav::stringid, std::vector<Feature>>& result,
               const mbgl::ScreenLineString& queryGeometry,
               const TransformState& state) const;

    void insert(std::shared_ptr<Feature> feature, std::shared_ptr<mapbox::geometry::polygon<int64_t>> envelope);

protected:
    struct FeatureRecord {
        std::shared_ptr<Feature> feature;
        std::shared_ptr<mapbox::geometry::polygon<int64_t>> envelope;
    };

    std::vector<FeatureRecord> features;
};

class FeatureIndex {
public:
    FeatureIndex(std::unique_ptr<const GeometryTileData> tileData_);

    const GeometryTileData* getData() { return tileData.get(); }
    
    void insert(const GeometryCollection&, std::size_t index, const nav::stringid& sourceLayerName, const nav::stringid& bucketLeaderID);

    void query(std::unordered_map<nav::stringid, std::vector<Feature>>& result,
               const GeometryCoordinates& queryGeometry,
               const TransformState&,
               const mat4& posMatrix,
               double tileSize,
               double scale,
               const RenderedQueryOptions& options,
               const UnwrappedTileID&,
               const std::unordered_map<nav::stringid, const RenderLayer*>&,
               float additionalQueryPadding,
               const SourceFeatureState& sourceFeatureState) const;

    static optional<GeometryCoordinates> translateQueryGeometry(const GeometryCoordinates& queryGeometry,
                                                                const std::array<float, 2>& translate,
                                                                style::TranslateAnchorType,
                                                                float bearing,
                                                                float pixelsToTileUnits);

    void setBucketLayerIDs(const nav::stringid& bucketLeaderID, const std::vector<std::string>& layerIDs);

    std::unordered_map<nav::stringid, std::vector<Feature>> lookupSymbolFeatures(
        const std::vector<IndexedSubfeature>& symbolFeatures,
        const RenderedQueryOptions& options,
        const std::unordered_map<nav::stringid, const RenderLayer*>& layers,
        const OverscaledTileID& tileID,
        const FeatureSortOrder& featureSortOrder) const;

private:
    void addFeature(std::unordered_map<nav::stringid, std::vector<Feature>>& result,
                    const IndexedSubfeature&,
                    const RenderedQueryOptions& options,
                    const CanonicalTileID&,
                    const std::unordered_map<nav::stringid, const RenderLayer*>&,
                    const GeometryCoordinates& queryGeometry,
                    const TransformState& transformState,
                    float pixelsToTileUnits,
                    const mat4& posMatrix,
                    const SourceFeatureState* sourceFeatureState) const;

    GridIndex<IndexedSubfeature> grid;
    unsigned int sortIndex = 0;

    std::unordered_map<nav::stringid, std::vector<std::string>> bucketLayerIDs;
    std::unique_ptr<const GeometryTileData> tileData;
};
} // namespace mbgl
