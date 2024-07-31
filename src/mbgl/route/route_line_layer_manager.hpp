// clang-format off

// This file is generated. Do not edit.

#pragma once

#include <mbgl/style/layer.hpp>
#include <mbgl/style/filter.hpp>
#include <mbgl/style/property_value.hpp>
#include <mbgl/style/rotation.hpp>
#include <mbgl/util/color.hpp>
#include <mbgl/util/geo.hpp>

#include <mbgl/route/route_data.hpp>
#include <mbgl/route/route_tile.hpp>

#include <unordered_set>
#include <unordered_map>

#include "mbgl/nav/nav_stringid.hpp"

namespace mbgl {

//namespace route {

class RouteLineLayerManager : private util::noncopyable {
public:
    RouteLineLayerManager();
    ~RouteLineLayerManager();
    
    static RouteLineLayerManager& getInstance();
    
    void setRouteInfo(const route&);
    
    // set puck begin position or update puck position
    void setPuckLocation(const mbgl::LatLng& location);
    
    
    void showRouteLine();
    
    void hideRouteLine();
    
    bool routeLineShowState();
    
    // for debug
    void setRouteGeometry(const mapbox::geometry::line_string<double>&);
    
public:
    
    static const nav::stringid SourceID;
    static const nav::stringid PointLayerID;
    static const nav::stringid ShapeLayerID;
    
private:
    // Dynamic line geometry vector for road conditions
    mapbox::geometry::line_string<double> line_string_past;     // past
    mapbox::geometry::line_string<double> line_string_unpast;   // unpast or all
    
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
    
private:
    void update();
    
    
};

//}

} // namespace mbgl

// clang-format on
