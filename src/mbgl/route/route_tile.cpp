
#include <mbgl/annotation/annotation_manager.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/renderer/tile_parameters.hpp>
#include <mbgl/route/route_line_layer_manager.hpp>
#include <mbgl/route/route_tile.hpp>
#include <utility>

namespace mbgl {

//namespace route {

RouteTile::RouteTile(const OverscaledTileID& overscaledTileID,
                               const TileParameters& parameters)
    : GeometryTile(overscaledTileID, RouteLineLayerManager::SourceID, parameters) {
//    auto guard = routeLineManager.lock();
    if (&mbgl::RouteLineLayerManager::getInstance()) {
        // #*#
        mbgl::RouteLineLayerManager::getInstance().addTile(*this);
    }
}

RouteTile::~RouteTile() {
//    auto guard = routeLineManager.lock();
    if (&mbgl::RouteLineLayerManager::getInstance()) {
        // #*#
        mbgl::RouteLineLayerManager::getInstance().removeTile(*this);
    }
}

class RouteTileFeatureData {
public:
    RouteTileFeatureData(const AnnotationID id_,
                              FeatureType type_,
                              GeometryCollection&& geometries_,
                              std::unordered_map<std::string, std::string>&& properties_)
        : id(id_),
          type(type_),
          geometries(std::move(geometries_)),
          properties(std::move(properties_)) {
    }

    AnnotationID id;
    FeatureType type;
    GeometryCollection geometries;
    std::unordered_map<std::string, std::string> properties;
};

RouteTileFeature::RouteTileFeature(std::shared_ptr<const RouteTileFeatureData> data_)
    : data(std::move(data_)) {
}

RouteTileFeature::~RouteTileFeature() = default;

FeatureType RouteTileFeature::getType() const {
    return data->type;
}

optional<Value> RouteTileFeature::getValue(const std::string& key) const {
    auto it = data->properties.find(key);
    if (it != data->properties.end()) {
        return optional<Value>(it->second);
    }
    return optional<Value>();
}

FeatureIdentifier RouteTileFeature::getID() const {
    return data->id;
}

const GeometryCollection& RouteTileFeature::getGeometries() const {
    return data->geometries;
}

class RouteTileLayerData {
public:
    explicit RouteTileLayerData(std::string name_) : name(std::move(name_)) {}

    const std::string name;
    std::vector<std::shared_ptr<const RouteTileFeatureData>> features;
};

RouteTileLayer::RouteTileLayer(std::shared_ptr<RouteTileLayerData> layer_) : layer(std::move(layer_)) {
}

std::size_t RouteTileLayer::featureCount() const {
    return layer->features.size();
}

std::unique_ptr<GeometryTileFeature> RouteTileLayer::getFeature(std::size_t i) const {
    return std::make_unique<RouteTileFeature>(layer->features.at(i));
}

std::string RouteTileLayer::getName() const {
    return layer->name;
}

void RouteTileLayer::addFeature(const AnnotationID id,
                                     FeatureType type,
                                     GeometryCollection geometries,
                                     std::unordered_map<std::string, std::string> properties) {

    layer->features.emplace_back(std::make_shared<RouteTileFeatureData>(
        id, type, std::move(geometries), std::move(properties)));
}

std::unique_ptr<GeometryTileData> RouteTileData::clone() const {
    return std::make_unique<RouteTileData>(*this);
}

std::unique_ptr<GeometryTileLayer> RouteTileData::getLayer(const nav::stringid& name) const {
    auto it = layers.find(name);
    if (it != layers.end()) {
        return std::make_unique<RouteTileLayer>(it->second);
    }
    return nullptr;
}

std::unique_ptr<RouteTileLayer> RouteTileData::addLayer(const nav::stringid& name) {
    // Only constructs a new layer if it doesn't yet exist, otherwise, we'll use the existing one.
    auto it = layers.find(name);
    if (it == layers.end()) {
        it = layers.emplace(name, std::make_shared<RouteTileLayerData>(name)).first;
    }
    return std::make_unique<RouteTileLayer>(it->second);
}

//}

} // namespace mbgl
