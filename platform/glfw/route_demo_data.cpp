#pragma once

#include <string>
#include <mbgl/style/sources/geojson_source.hpp>
#include "route_demo_data.hpp"

namespace mbgl {
namespace platform {
namespace glfw {

const mapbox::geojson::geojson& RouteValue(int index) {

    static mapbox::geojson::geojson route[] = {
        mapbox::geojson::parse(NewYork::Route),
        mapbox::geojson::parse(Guomao::Route),
        mapbox::geojson::parse(Hongkong::Route),
        mapbox::geojson::parse(Changanjie::Route),
    };
    
    return route[index];
}



} // namespace glfw
} // namespace platform
} // namespace mbgl

