// clang-format off

// This file is generated. Do not edit.

#pragma once

#include <mbgl/style/layer.hpp>
#include <mbgl/style/filter.hpp>
#include <mbgl/style/property_value.hpp>
#include <mbgl/style/rotation.hpp>
#include <mbgl/util/color.hpp>

namespace mbgl {

enum class RoadCondition {
    Past = 0,           // 行驶过 - 灰色
    Fast,               // 可高速行驶 - 深绿
    Unblocked,          // 畅通 - 绿色
    Normal,             // 正常路况 - 浅绿色
    Congested,          // 行驶缓慢 - 黄色
    VeryCongested,      // 非常拥堵 - 浅红色
    MostCongested       // 极致拥堵 - 深红色
};

class RouteLineLayerManager {
public:
    RouteLineLayerManager();
    ~RouteLineLayerManager();

    void setRouteLineGeometries(std::vector<mapbox::geometry::line_string<double>>& line_strings, RoadCondition);

protected:
    // Dynamic line geometry vector for every(7) road conditions
    std::vector<mapbox::geometry::line_string<double>> line_string1; // past
    std::vector<mapbox::geometry::line_string<double>> line_string2; // fast
    std::vector<mapbox::geometry::line_string<double>> line_string3; // unblocked
    std::vector<mapbox::geometry::line_string<double>> line_string4; // normal - default road condition
    std::vector<mapbox::geometry::line_string<double>> line_string5; // congested
    std::vector<mapbox::geometry::line_string<double>> line_string6; // very congested
    std::vector<mapbox::geometry::line_string<double>> line_string7; // most congested
    
};

} // namespace mbgl

// clang-format on
