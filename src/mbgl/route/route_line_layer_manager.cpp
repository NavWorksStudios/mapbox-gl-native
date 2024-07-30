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






void RouteLineLayerManager::update() {
    
    
}

//std::unique_ptr<AnnotationTileData> RouteLineLayerManager::::getTileData(const CanonicalTileID& tileID) {
//    if (symbolAnnotations.empty() && shapeAnnotations.empty())
//        return nullptr;
//
//    auto tileData = std::make_unique<AnnotationTileData>();
//
//    auto pointLayer = tileData->addLayer(PointLayerID);
//
//    LatLngBounds tileBounds(tileID);
//    // Hack for https://github.com/mapbox/mapbox-gl-native/issues/12472
//    // To handle precision issues, query a slightly larger area than the tile bounds
//    // Symbols at a border can be included in vector data for both tiles
//    // The rendering/querying logic will make sure the symbols show up in only one of the tiles
//    tileBounds.extend(LatLng(tileBounds.south() - 0.000000001, tileBounds.west() - 0.000000001));
//    tileBounds.extend(LatLng(tileBounds.north() + 0.000000001, tileBounds.east() + 0.000000001));
//    
//    symbolTree.query(boost::geometry::index::intersects(tileBounds),
//        boost::make_function_output_iterator([&](const auto& val){
//            val->updateLayer(tileID, *pointLayer);
//        }));
//
//    for (const auto& shape : shapeAnnotations) {
//        shape.second->updateTileData(tileID, *tileData);
//    }
//
//    return tileData;
//}


}

} // namespace mbgl

// clang-format on
