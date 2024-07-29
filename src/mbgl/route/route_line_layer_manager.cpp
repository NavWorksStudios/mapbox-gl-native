// clang-format off

// This file is generated. Edit scripts/generate-style-code.js, then run `make style-code`.

#include <mbgl/route/route_line_layer_manager.hpp>
#include <mbgl/style/layers/location_indicator_layer_impl.hpp>
#include <mbgl/style/layer_observer.hpp>
#include <mbgl/style/conversion/color_ramp_property_value.hpp>
#include <mbgl/style/conversion/constant.hpp>
#include <mbgl/style/conversion/property_value.hpp>
#include <mbgl/style/conversion/transition_options.hpp>
#include <mbgl/style/conversion/json.hpp>
#include <mbgl/style/conversion_impl.hpp>
#include <mbgl/util/traits.hpp>

#include <mapbox/eternal.hpp>

namespace mbgl {

namespace route {

static RouteLineLayerManager instance;

RouteLineLayerManager& RouteLineLayerManager::getInstance() {
    return instance;
}

RouteLineLayerManager::RouteLineLayerManager() {
}

RouteLineLayerManager::~RouteLineLayerManager() = default;

void RouteLineLayerManager::setRouteInfo(const route& route) {
    
    
}

void RouteLineLayerManager::setPuckLocation(const mbgl::LatLng& location) {
    
    
}

}

} // namespace mbgl

// clang-format on
