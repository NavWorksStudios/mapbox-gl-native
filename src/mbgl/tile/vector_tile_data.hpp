#include <mbgl/tile/geometry_tile_data.hpp>

#include <mapbox/vector_tile.hpp>
#include <protozero/pbf_reader.hpp>

#include <unordered_map>
#include <functional>
#include <utility>

namespace mbgl {

class VectorTileFeature : public GeometryTileFeature {
public:
    VectorTileFeature(const mapbox::vector_tile::layer&, const protozero::data_view&);

    FeatureType getType() const override;
    optional<Value> getValue(const std::string& key) const override;
    const PropertyMap& getProperties() const override;
    FeatureIdentifier getID() const override;
    const GeometryCollection& getGeometries() const override;

private:
    mapbox::vector_tile::feature feature;
    mutable optional<GeometryCollection> lines;
    mutable optional<PropertyMap> properties;
};

class VectorTileLayer : public GeometryTileLayer {
public:
    VectorTileLayer(std::shared_ptr<const std::string> data, const protozero::data_view&);

    std::size_t featureCount() const override;
    std::unique_ptr<GeometryTileFeature> getFeature(std::size_t i) const override;
    std::string getName() const override;

private:
    std::shared_ptr<const std::string> data;
    mapbox::vector_tile::layer layer;
};

class VectorTileData : public GeometryTileData {
public:
    VectorTileData(std::shared_ptr<const std::string> data);

    std::unique_ptr<GeometryTileData> clone() const override;
    std::unique_ptr<GeometryTileLayer> getLayer(const nav::stringid& name) const override;

    const std::vector<nav::stringid>& layerNames() const;

private:
    std::shared_ptr<const std::string> data;
    std::shared_ptr<mapbox::vector_tile::buffer> buffer;
};

} // namespace mbgl
