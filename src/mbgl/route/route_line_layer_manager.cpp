// clang-format off

// This file is generated. Edit scripts/generate-style-code.js, then run `make style-code`.

#include <mapbox/eternal.hpp>
#include <mbgl/style/layers/location_indicator_layer_impl.hpp>
#include <mbgl/style/layer_observer.hpp>
#include <mbgl/style/conversion/color_ramp_property_value.hpp>
#include <mbgl/style/conversion/constant.hpp>
#include <mbgl/style/conversion/property_value.hpp>
#include <mbgl/style/conversion/transition_options.hpp>
#include <mbgl/style/conversion/json.hpp>
#include <mbgl/style/conversion_impl.hpp>
#include <mbgl/style/expression/dsl.hpp>
#include <mbgl/util/traits.hpp>
#include <mbgl/style/style.hpp>
#include <mbgl/style/style_impl.hpp>
#include <mbgl/route/route_line_layer_manager.hpp>
#include <mbgl/route/route_source.hpp>
#include <mbgl/route/render_route_source.hpp>
#include <mbgl/style/layers/line_layer.hpp>
#include <mbgl/tile/tile_id.hpp>
#include <mbgl/math/wrap.hpp>
#include <mbgl/math/clamp.hpp>
#include <mbgl/util/string.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/geometry.hpp>

namespace mbgl {

using namespace style;

//namespace route {

//static RouteLineLayerManager instance;
const nav::stringid RouteLineLayerManager::RouteSourceID( "com.mapbox.routelines" );
//const nav::stringid RouteLineLayerManager::PointLayerID( "com.mapbox.routelines.points" );
const nav::stringid RouteLineLayerManager::RouteShapeLayerID( "com.mapbox.routelines.shape" );

//RouteLineLayerManager& RouteLineLayerManager::getInstance() {
//    return instance;
//}

RouteLineLayerManager::RouteLineLayerManager(Style& style_)
        : style(style_) {
};

RouteLineLayerManager::~RouteLineLayerManager() = default;

void RouteLineLayerManager::setRouteInfo(const route& route) {
    
    // #*# 将原始导航数据根据当前zoom级别进行切分
    
}

void RouteLineLayerManager::setPuckLocation(const mbgl::LatLng& location) {
    puckLocation = location;
    // #*# 根据puckLocation计算已行驶路线的型点数据
    
}

void RouteLineLayerManager::setRouteGeometry(const mapbox::geometry::line_string<double>& line_strings) {
    
    // 保存导航路线的型点数据
    line_string_unpast = std::move(line_strings);
    
    // 根据layer manager中的金字塔的tile列表切分型点数据
    
    
    // 将切分后（分属不同tile的）数据进行parse解析装载进route line layer
    
    
    // 将route line layer保存到layer manager等待渲染
    
}

void RouteLineLayerManager::showRouteLine() {
    inShowing = true;
    
}

void RouteLineLayerManager::hideRouteLine() {
    inShowing = false;
    
}

bool RouteLineLayerManager::routeLineShowState() {
    return inShowing;
}

void RouteLineLayerManager::updateData() {
//    CHECK_ROUTE_ENABLED_AND_RETURN();
    std::lock_guard<std::mutex> lock(mutex);
    if (dirty) {
        for (auto& tile : tiles) {
            tile->setData(getTileData(tile->id.canonical));
        }
        dirty = false;
    }
}

void RouteLineLayerManager::addTile(RouteTile& tile) {
//    CHECK_ROUTE_ENABLED_AND_RETURN();
    std::lock_guard<std::mutex> lock(mutex);
    tiles.insert(&tile);
    tile.setData(getTileData(tile.id.canonical));
}


void RouteLineLayerManager::removeTile(RouteTile& tile) {
//    CHECK_ROUTE_ENABLED_AND_RETURN();
    std::lock_guard<std::mutex> lock(mutex);
    tiles.erase(&tile);
}

void RouteLineLayerManager::setStyle(style::Style& style_) {
//    CHECK_ROUTE_ENABLED_AND_RETURN();
    style = style_;
}

void RouteLineLayerManager::onStyleLoaded() {
//    CHECK_ROUTE_ENABLED_AND_RETURN();
    updateStyle();
}

void RouteLineLayerManager::updateStyle() {
    // Create annotation source, point layer, and point bucket. We do everything via Style::Impl
    // because we don't want annotation mutations to trigger Style::Impl::styleMutated to be set.
    if (!style.get().impl->getSource(RouteSourceID.get())) {
        style.get().impl->addSource(std::make_unique<RouteSource>());

        // #*#
        std::unique_ptr<LineLayer> layer = std::make_unique<LineLayer>(RouteShapeLayerID, RouteSourceID);

        using namespace expression::dsl;
        layer->setSourceLayer(RouteSourceID.get());

        style.get().impl->addLayer(std::move(layer));
    }

//    std::lock_guard<std::mutex> lock(mutex);

    // #*#
//    for (const auto& shape : shapeAnnotations) {
//        shape.second->updateStyle(*style.get().impl);
//    }

    // #*# 未来可能添加导航使用图片 - 如：鱼骨线
//    for (const auto& image : images) {
//        // Call addImage even for images we may have previously added, because we must support
//        // addAnnotationImage being used to update an existing image. Creating a new image is
//        // relatively cheap, as it copies only the Immutable reference. (We can't keep track
//        // of which images need to be added because we don't know if the style is the same
//        // instance as in the last updateStyle call. If it's a new style, we need to add all
//        // images.)
//        style.get().impl->addImage(std::make_unique<style::Image>(image.second));
//    }
}

std::unique_ptr<RouteTileData> RouteLineLayerManager::getTileData(const CanonicalTileID& tileID) {
    // #*#
    if (line_string_unpast.empty() && line_string_past.empty())
        return nullptr;

    auto tileData = std::make_unique<RouteTileData>();

    auto pointLayer = tileData->addLayer(RouteSourceID);

    LatLngBounds tileBounds(tileID);
    // Hack for https://github.com/mapbox/mapbox-gl-native/issues/12472
    // To handle precision issues, query a slightly larger area than the tile bounds
    // Symbols at a border can be included in vector data for both tiles
    // The rendering/querying logic will make sure the symbols show up in only one of the tiles
    tileBounds.extend(LatLng(tileBounds.south() - 0.000000001, tileBounds.west() - 0.000000001));
    tileBounds.extend(LatLng(tileBounds.north() + 0.000000001, tileBounds.east() + 0.000000001));
    
    // #*# 未来可能添加路况插标图
//    symbolTree.query(boost::geometry::index::intersects(tileBounds),
//        boost::make_function_output_iterator([&](const auto& val){
//            val->updateLayer(tileID, *pointLayer);
//        }));
    
    // #*# 将根据tile切分后的型点数据分tile id传递到tileData中
//    for (const auto& shape : shapeAnnotations) {
//        shape.second->updateTileData(tileID, *tileData);
//    }
    updateTileData(tileID, *tileData);

    return tileData;
}

#if 1
void RouteLineLayerManager::updateTileData(const CanonicalTileID& tileID, RouteTileData& data) {
    static const double baseTolerance = 4;

    if (!shapeTiler) {
        mapbox::feature::feature_collection<double> features;
//        features.emplace_back(ShapeAnnotationGeometry::visit(
//            geometry(), [](auto&& geom) { return Feature{std::forward<decltype(geom)>(geom)}; }));
        features.emplace_back(Feature{std::forward<decltype(geometry())>(geometry())});
        mapbox::geojsonvt::Options options;
        // The annotation source is currently hard coded to maxzoom 16, so we're topping out at z16
        // here as well.
        options.maxZoom = 16;
        options.buffer = 255u;
        options.extent = util::EXTENT;
        options.tolerance = baseTolerance;
        shapeTiler = std::make_unique<mapbox::geojsonvt::GeoJSONVT>(features, options);
    }

    const auto& shapeTile = shapeTiler->getTile(tileID.z, tileID.x, tileID.y);
    if (shapeTile.features.empty())
        return;

    auto layer = data.addLayer(RouteShapeLayerID);

    ToGeometryCollection toGeometryCollection;
    ToFeatureType toFeatureType;
    for (const auto& shapeFeature : shapeTile.features) {
        FeatureType featureType = apply_visitor(toFeatureType, shapeFeature.geometry);
        GeometryCollection renderGeometry = apply_visitor(toGeometryCollection, shapeFeature.geometry);

        assert(featureType != FeatureType::Unknown);

        // https://github.com/mapbox/geojson-vt-cpp/issues/44
        if (featureType == FeatureType::Polygon) {
            renderGeometry = fixupPolygons(renderGeometry);
        }

        // #*# id
        layer->addFeature(1, featureType, renderGeometry);
    }
}

const LineString<double>& RouteLineLayerManager::geometry() const {
    return line_string_unpast;
}

#endif

RoutePlanID RouteLineLayerManager::addRoutePlan(const RoutePlan& routePlan) {
//    CHECK_ROUTE_ENABLED_AND_RETURN(nextID++);
    std::lock_guard<std::mutex> lock(mutex);
    RoutePlanID id = nextID++;
    RoutePlan::visit(routePlan, [&] (const auto& routePlan_) {
        this->add(id, routePlan_);
    });
    dirty = true;
    return id;
}

// #*# 未来大概率需要用于路况插标
void RouteLineLayerManager::add(const RoutePlanID& id, const SymbolRoutePlan& routePlan) {
//    auto impl = std::make_shared<SymbolAnnotationImpl>(id, annotation);
//    symbolTree.insert(impl);
//    symbolAnnotations.emplace(id, impl);
}

void RouteLineLayerManager::add(const RoutePlanID& id, const LineRoutePlan& routePlan) {
//    ShapeAnnotationImpl& impl = *shapeAnnotations.emplace(id,
//        std::make_unique<LineAnnotationImpl>(id, annotation)).first->second;
//    impl.updateStyle(*style.get().impl);
}

// #*# 未来大概率无效需废弃
void RouteLineLayerManager::add(const RoutePlanID& id, const FillRoutePlan& routePlan) {
//    ShapeAnnotationImpl& impl = *shapeAnnotations.emplace(id,
//        std::make_unique<FillAnnotationImpl>(id, annotation)).first->second;
//    impl.updateStyle(*style.get().impl);
}

//}

} // namespace mbgl

// clang-format on
