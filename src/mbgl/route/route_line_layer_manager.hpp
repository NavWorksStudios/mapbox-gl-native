// clang-format off

// This file is generated. Do not edit.

#pragma once

#include <mbgl/style/layer.hpp>
#include <mbgl/style/filter.hpp>
#include <mbgl/style/property_value.hpp>
#include <mbgl/style/rotation.hpp>
#include <mbgl/util/color.hpp>
#include <mbgl/util/geo.hpp>
#include <mapbox/geojsonvt.hpp>

#include <mbgl/route/route_data.hpp>
#include <mbgl/route/route_tile.hpp>

#include <unordered_set>
#include <unordered_map>

#include "mbgl/nav/nav_stringid.hpp"

namespace mbgl {

//namespace route {

namespace style {
class Style;
} // namespace style

class RouteLineLayerManager : private util::noncopyable {
public:
    RouteLineLayerManager(style::Style&);
    ~RouteLineLayerManager();
    
//    static RouteLineLayerManager& getInstance();
    
    void setRouteInfo(const route&);
    
    // set puck begin position or update puck position
    void setPuckLocation(const mbgl::LatLng& location);
    
    
    void showRouteLine();
    
    void hideRouteLine();
    
    bool routeLineShowState();
    
    void updateData();
    
    // for debug
    void setRouteGeometry(const mapbox::geometry::line_string<double>&);
    
    void addTile(RouteTile&);
    void removeTile(RouteTile&);
    
    void setStyle(style::Style&);
    void onStyleLoaded();
    
public:
    
    static const nav::stringid RouteSourceID;
//    static const nav::stringid PointLayerID;
    static const nav::stringid RouteShapeLayerID;
    
    mapbox::base::WeakPtr<RouteLineLayerManager> makeWeakPtr() { return weakFactory.makeWeakPtr(); }
    
private:
    // Dynamic line geometry vector for road conditions
    mapbox::geometry::line_string<double> line_string_past;     // past
    mapbox::geometry::line_string<double> line_string_unpast;   // unpast or all, mapbox::geometry::line_string<T>
    
    style::PropertyValue<Color> color_f3 = Color::black();
    style::PropertyValue<Color> color_f2 = Color::black();
    style::PropertyValue<Color> color_f1 = Color::black();
    style::PropertyValue<Color> color_normal = Color::black();
    style::PropertyValue<Color> color_s1 = Color::black();
    style::PropertyValue<Color> color_s2 = Color::black();
    style::PropertyValue<Color> color_s3 = Color::black();
    
    bool inShowing = false;
    bool crossfade = true;
    
    std::unordered_set<RouteTile*> tiles;
    std::mutex mutex;
    bool dirty = false;
    std::reference_wrapper<style::Style> style;
    
    mbgl::LatLng puckLocation;
    std::unique_ptr<mapbox::geojsonvt::GeoJSONVT> shapeTiler;
    
private:
    mapbox::base::WeakPtrFactory<RouteLineLayerManager> weakFactory{this};
    std::unique_ptr<RouteTileData> getTileData(const CanonicalTileID& tileID);
    void updateStyle();
    void updateTileData(const CanonicalTileID& tileID, RouteTileData& data);
    const LineString<double>& geometry() const;
};

//}

} // namespace mbgl

// clang-format on
