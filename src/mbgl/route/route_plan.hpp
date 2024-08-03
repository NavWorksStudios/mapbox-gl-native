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

class LineRoutePlan {
public:
    LineRoutePlan(LineString<double> geometry_,
                  nav::stringid layerId_)
        : geometry(std::move(geometry_)),
          layerId(layerId_) {}
    LineString<double> geometry;
    nav::stringid layerId;  // 关联的style中的layer
};

class FillRoutePlan {
public:
    FillRoutePlan(ShapeRoutePlanGeometry geometry_,
                  nav::stringid layerId_)
    : geometry(std::move(geometry_)),
      layerId(layerId_) {}

    ShapeRoutePlanGeometry geometry;
    nav::stringid layerId;  // 关联的style中的layer
};

using RoutePlan = variant<
    SymbolRoutePlan,
    LineRoutePlan,
    FillRoutePlan>;

} // namespace mbgl
