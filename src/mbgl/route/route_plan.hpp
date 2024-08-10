#pragma once

#include <mbgl/util/geometry.hpp>
#include <mbgl/util/variant.hpp>
#include <mbgl/util/color.hpp>
#include <mbgl/style/property_value.hpp>

#include <cstdint>
#include <vector>
#include <string>

#include "mbgl/nav/nav_stringid.hpp"

namespace mbgl {

using RoutePlanID = uint64_t;
using RoutePlanIDs = std::vector<RoutePlanID>;

class SymbolRoutePlan {
public:
    SymbolRoutePlan(std::vector<Point<double>> geometries_,
                    std::vector<std::string> icons_ = {},
                    std::vector<std::string> descriptions_ = {})
            : geometries(geometries_),
              icons(std::move(icons)),
              descriptions(std::move(descriptions_)) {}

    std::vector<Point<double>> geometries;
    std::vector<std::string> icons;
    std::vector<std::string> descriptions;
};

using ShapeRoutePlanGeometry = variant<
    LineString<double>,
    Polygon<double>,
    MultiLineString<double>,
    MultiPolygon<double>>;

class TrafficInfo {
public:
    double percent;
    int16_t condition;
};

//class LineRouteTrafficInfo {
//public:
//    std::vector<TrafficInfo> infos;
//};

class LineRoutePlan {
public:
    LineRoutePlan(LineString<double> geometry_,
                  std::vector<TrafficInfo> trafficInfo_,
                  bool mainPlan_ = false)
        : geometry(std::move(geometry_)),
          trafficInfo(std::move(trafficInfo_)),
          mainPlan(mainPlan_) {}
    LineString<double> geometry;
    std::vector<TrafficInfo> trafficInfo;
    bool mainPlan;
};

class FillRoutePlan {
public:
    FillRoutePlan(ShapeRoutePlanGeometry geometry_)
        : geometry(std::move(geometry_)) {}

    ShapeRoutePlanGeometry geometry;
    nav::stringid layerId;  // 关联的style中的layer
};

using RoutePlan = variant<
    SymbolRoutePlan,
    LineRoutePlan,
    FillRoutePlan>;

} // namespace mbgl
