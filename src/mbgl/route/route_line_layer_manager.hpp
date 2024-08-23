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
#include <mbgl/route/route_plan.hpp>
#include <mbgl/route/route_plan_tile.hpp>

#include <unordered_set>
#include <unordered_map>
#include <mbgl/clipper2/clipper.h>

#include "mbgl/nav/nav.stringid.hpp"

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
    
    RoutePlanID addRoutePlan(const RoutePlan& routePlan);
    void add(const RoutePlanID& id, const LineRoutePlan& routePlan);
    void add(const RoutePlanID& id, const SymbolRoutePlan& routePlan);
    void add(const RoutePlanID& id, const FillRoutePlan& routePlan);
    void updateRoutePlan(const RoutePlanID& id, const RoutePlan& routePlan);
    void update(const RoutePlanID& id, const LineRoutePlan& routePlan);
    void update(const RoutePlanID& id, const SymbolRoutePlan& routePlan);
    void update(const RoutePlanID& id, const FillRoutePlan& routePlan);
public:
    
    static const int8_t MaxRenderaleZoom;
    static const nav::stringid RouteSourceID;
    static const nav::stringid RouteShapeLayerID;
    static const nav::stringid RouteDimmedLayerID;
    
    mapbox::base::WeakPtr<RouteLineLayerManager> makeWeakPtr() { return weakFactory.makeWeakPtr(); }
    
private:
    //
    // Dynamic line geometry vector for road conditions
    mapbox::geometry::line_string<double> line_string_past;         // past
//    mapbox::geometry::line_string<double> line_string_unpast;     // unpast or all, mapbox::geometry::line_string<T>
    
    mapbox::geometry::line_string<double> route_points;             // all points of current route plan, mapbox::geometry::line_string<T>
    std::vector<TrafficInfo> trafficInfo;
    mapbox::geometry::line_string<double> route_points_inserted;    // route_points inserted traffic node point
    std::vector<int16_t> route_conds_inserted;
    double totol_distance = 0.0;
    
    bool updateRoute = false;
    bool updateTrafficInfo = false;
    
    bool hasRouteLayer = false;
    bool hasRouteDimmedLayer = false;
    uint8_t zoom;
    mbgl::Point<int64_t> last_point;
    CanonicalTileID* last_tileID;
    nav::stringid last_tile_id = {"not_set"};
    std::unordered_map<nav::stringid, LineRoutePlanTile> planTiles16;
    std::unordered_map<nav::stringid, LineRoutePlanTile> planTiles11;
    std::unordered_map<nav::stringid, LineRoutePlanTile> planTiles6;
    
//    style::PropertyValue<Color> color_f3 = Color::black();
//    style::PropertyValue<Color> color_f2 = Color::black();
//    style::PropertyValue<Color> color_f1 = Color::black();
//    style::PropertyValue<Color> color_normal = Color::black();
//    style::PropertyValue<Color> color_s1 = Color::black();
//    style::PropertyValue<Color> color_s2 = Color::black();
//    style::PropertyValue<Color> color_s3 = Color::black();
    
    bool inShowing = false;
    bool crossfade = true;
    
    std::unordered_set<RouteTile*> tiles;
    std::mutex mutex;
    bool dirty = false;
    std::reference_wrapper<style::Style> style;
    RoutePlanID nextID = 0;
    
    mbgl::LatLng puckLocation;
    std::unique_ptr<mapbox::geojsonvt::GeoJSONVT> shapeTiler;
    
private:
    mapbox::base::WeakPtrFactory<RouteLineLayerManager> weakFactory{this};
    std::unique_ptr<RouteTileData> getTileData(const CanonicalTileID& tileID);
    void updateStyle();
    void updateTileData(const CanonicalTileID& tileID, RouteTileData& data);
    const LineString<double>& geometry() const;
    CanonicalTileID latLonToTileID(const mbgl::Point<double>& point, mbgl::Point<int64_t>& point_local, const int8_t z);
    mbgl::Point<int64_t> latLonToTileCoodinates(const mbgl::Point<int64_t>& point, mbgl::CanonicalTileID& canonical);
    Point<int64_t> intersectPoint(const LineString<int64_t>& line_, const CanonicalTileID& tileID);
    
    void insertNodesForTrafficCondition(const LineRoutePlan&);
    void convertTileData(const LineRoutePlan& routePlan, std::unordered_map<nav::stringid, LineRoutePlanTile>& planTiles_, int8_t zoom = 16);
    double countTotalDistance(LineString<double>& line_string_);
};

//}

} // namespace mbgl

// clang-format on
