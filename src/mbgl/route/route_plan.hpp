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
    SymbolRoutePlan(Point<double> geometry_, std::string icon_ = {}) : geometry(geometry_), icon(std::move(icon_)) {}

    Point<double> geometry;
    std::string icon;
};

using ShapeRoutePlanGeometry = variant<
    LineString<double>,
    Polygon<double>,
    MultiLineString<double>,
    MultiPolygon<double>>;

class LineRoutePlan {
public:
    LineRoutePlan(ShapeRoutePlanGeometry geometry_,
                  nav::stringid layerId_)
        : geometry(std::move(geometry_)),
          layerId(layerId_) {}
    ShapeRoutePlanGeometry geometry;
    nav::stringid layerId;
};

class FillRoutePlan {
public:
    FillRoutePlan(ShapeRoutePlanGeometry geometry_,
                  nav::stringid layerId_)
    : geometry(std::move(geometry_)),
      layerId(layerId_) {}

    ShapeRoutePlanGeometry geometry;
    nav::stringid layerId;
};

using RoutePlan = variant<
    SymbolRoutePlan,
    LineRoutePlan,
    FillRoutePlan>;

} // namespace mbgl
