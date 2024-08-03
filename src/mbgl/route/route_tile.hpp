#pragma once

#include <mbgl/annotation/annotation.hpp>
#include <mbgl/tile/geometry_tile.hpp>
#include <mbgl/tile/geometry_tile_data.hpp>
#include <mbgl/route/route_plan.hpp>

namespace mbgl {

//namespace route {

class AnnotationManager;
class RouteLineLayerManager;
class TileParameters;

class RouteTile : public GeometryTile {
public:
    RouteTile(const OverscaledTileID&, const TileParameters&);
    ~RouteTile() override;

private:
    mapbox::base::WeakPtr<RouteLineLayerManager> routeManager;
};

class RouteTileFeatureData;

class RouteTileFeature : public GeometryTileFeature {
public:
    RouteTileFeature(std::shared_ptr<const RouteTileFeatureData>);
    ~RouteTileFeature() override;

    FeatureType getType() const override;
    optional<Value> getValue(const std::string&) const override;
    FeatureIdentifier getID() const override;
    const GeometryCollection& getGeometries() const override;

private:
    std::shared_ptr<const RouteTileFeatureData> data;
};

class RouteTileLayerData;

class RouteTileLayer : public GeometryTileLayer {
public:
    RouteTileLayer(std::shared_ptr<RouteTileLayerData>);

    std::size_t featureCount() const override;
    std::unique_ptr<GeometryTileFeature> getFeature(std::size_t i) const override;
    std::string getName() const override;

    void addFeature(RoutePlanID,
                    FeatureType,
                    GeometryCollection,
                    std::unordered_map<std::string, std::string> properties = {{}});

private:
    std::shared_ptr<RouteTileLayerData> layer;
};

class RouteTileData : public GeometryTileData {
public:
    std::unique_ptr<GeometryTileData> clone() const override;
    std::unique_ptr<GeometryTileLayer> getLayer(const nav::stringid&) const override;
    
    std::unique_ptr<RouteTileLayer> getAndNewLayer(const nav::stringid&);
    std::unique_ptr<RouteTileLayer> addLayer(const nav::stringid&);

private:
    std::unordered_map<nav::stringid, std::shared_ptr<RouteTileLayerData>> layers;
};

//}

} // namespace mbgl
