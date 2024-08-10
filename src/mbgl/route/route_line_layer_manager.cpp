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
#include <mbgl/clipper2/clipper.h>

namespace mbgl {

using namespace style;

//namespace route {

const int8_t RouteLineLayerManager::MaxRenderaleZoom = 6;
const nav::stringid RouteLineLayerManager::RouteSourceID( "route" );
const nav::stringid RouteLineLayerManager::RouteShapeLayerID( "route" );
const nav::stringid RouteLineLayerManager::RouteDimmedLayerID( "route-dimmed");

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
    route_points = std::move(line_strings);
    
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
        // #*# 根据导航状态（车载定位变化及路况变化等）修改route line状态
//        for (auto& tile : tiles) {
//            if(tile->id.canonical.z != 6 && tile->id.canonical.z != 11 && tile->id.canonical.z != 16) {
//                tile->setRenderable(false);
//            }
//            else tile->setData(getTileData(tile->id.canonical));
//        }
        dirty = false;
    }
}

void RouteLineLayerManager::addTile(RouteTile& tile) {
//    CHECK_ROUTE_ENABLED_AND_RETURN();
    std::lock_guard<std::mutex> lock(mutex);
    // zooxm小于6级的tile直接忽略
    if(tile.id.canonical.z < MaxRenderaleZoom) {
        tile.setRenderable(false);
        return;
    }
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
        
        // #*# 导航主线路图层 - 未行驶
        Layer* layer = style.get().impl->getLayer(RouteShapeLayerID);
        if(layer) {
            layer->setSourceLayer(RouteSourceID.get());
            layer->setSourceID(RouteSourceID.get());
            hasRouteLayer = true;
        }
        
        // #*# 导航主线路图层 - 已行驶
        Layer* layer_dimmed = style.get().impl->getLayer(RouteDimmedLayerID);
        if(layer_dimmed) {
            layer_dimmed->setSourceLayer(RouteSourceID.get());
            layer_dimmed->setSourceID(RouteSourceID.get());
            hasRouteDimmedLayer = true;
        }
        
        // #*# 导航路况指示牌插标
//        std::lock_guard<std::mutex> lock(mutex);
//        for (const auto& shape : shapeAnnotations) {
//            shape.second->updateStyle(*style.get().impl);
//        }
        
    }

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
    // #*# 如果没有原始导航数据，直接返回nullptr
    if (route_points.empty() && line_string_past.empty())
        return nullptr;
    // #*# 如果没有导航显示所需的相关layer，直接返回nullptr
    if(!hasRouteLayer || !hasRouteDimmedLayer) {
        return nullptr;
    }

    auto tileData = std::make_unique<RouteTileData>();

    auto pointLayer = tileData->addLayer(RouteSourceID);

    // #*# 基于经纬坐标的地理围栏，未来可能有用
//    LatLngBounds tileBounds(tileID);
//    // Hack for https://github.com/mapbox/mapbox-gl-native/issues/12472
//    // To handle precision issues, query a slightly larger area than the tile bounds
//    // Symbols at a border can be included in vector data for both tiles
//    // The rendering/querying logic will make sure the symbols show up in only one of the tiles
//    tileBounds.extend(LatLng(tileBounds.south() - 0.000000001, tileBounds.west() - 0.000000001));
//    tileBounds.extend(LatLng(tileBounds.north() + 0.000000001, tileBounds.east() + 0.000000001));
    
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


void RouteLineLayerManager::updateTileData(const CanonicalTileID& tileID, RouteTileData& data) {
    
    std::unordered_map<nav::stringid, LineRoutePlanTile>* planTiles_ = nullptr;
    if(tileID.z == 6) {
        planTiles_ = &planTiles6;
    }
    else if(tileID.z == 11) {
        planTiles_ = &planTiles11;
    }
    else {
        planTiles_ = &planTiles16;
    }
    
    nav::stringid tile_id = nav::stringid(util::toString(tileID));
    auto plan_tile = planTiles_->find(tile_id);
    if(plan_tile == planTiles_->end())
        return;
    if(plan_tile->second.segments.size() == 0)
        return;
    
    auto layer = data.addLayer(RouteShapeLayerID);
    
    ToGeometryCollection toGeometryCollection;
    ToFeatureType toFeatureType;
    FeatureType featureType = FeatureType::LineString;
    GeometryCollection renderGeometry;
    for(auto& segment : plan_tile->second.segments) {
        GeometryCoordinates points;
        for (auto& point : segment.points) {
            points.push_back(Point<int16_t>{static_cast<int16_t>(point.x), static_cast<int16_t>(point.y)});
        }
        renderGeometry.push_back(points);
    }
    
    // #*# id
    layer->addFeature(1, featureType, renderGeometry);
    
    // #*# 原逻辑，将废弃
    return;
#if 0
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
#endif
}

const LineString<double>& RouteLineLayerManager::geometry() const {
    return route_points;
}


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

void RouteLineLayerManager::updateRoutePlan(const RoutePlanID& id, const RoutePlan& routePlan) {
    //    CHECK_ROUTE_ENABLED_AND_RETURN(nextID++);
    std::lock_guard<std::mutex> lock(mutex);
    RoutePlan::visit(routePlan, [&] (const auto& routePlan_) {
        this->update(id, routePlan_);
    });
    dirty = true;
}

CanonicalTileID RouteLineLayerManager::latLonToTileID(const mbgl::Point<double>& point, mbgl::Point<int64_t>& point_local, const int8_t z) {
    const double size_local = mbgl::util::EXTENT * std::pow(2, z);
    const double size = std::pow(2, z);
    
    auto x_local = (point.x + mbgl::util::LONGITUDE_MAX) * size_local / mbgl::util::DEGREES_MAX;
    auto y_local =
        (mbgl::util::LONGITUDE_MAX - (std::log(std::tan(point.y * M_PI / mbgl::util::DEGREES_MAX + M_PI / 4.0)) * mbgl::util::RAD2DEG)) *
        size_local / mbgl::util::DEGREES_MAX;
    
    point_local.x = x_local;
    point_local.y = y_local;
    
    auto x = (point.x + mbgl::util::LONGITUDE_MAX) * size / mbgl::util::DEGREES_MAX;
    auto y =
        (mbgl::util::LONGITUDE_MAX - (std::log(std::tan(point.y * M_PI / mbgl::util::DEGREES_MAX + M_PI / 4.0)) * mbgl::util::RAD2DEG)) *
        size / mbgl::util::DEGREES_MAX;

    CanonicalTileID p(z,
                      (mbgl::util::clamp<int64_t>(x, std::numeric_limits<int64_t>::min(), std::numeric_limits<int64_t>::max())),
                      (mbgl::util::clamp<int64_t>(y, std::numeric_limits<int64_t>::min(), std::numeric_limits<int64_t>::max())));
    return p;
};

Point<int64_t> RouteLineLayerManager::intersectPoint(const LineString<int64_t>& line_, const CanonicalTileID& tileID ) {
    
    // #*# 跨tile补点的碰撞偏移，未来或需调整
    const int16_t clipper_offset = 256;
    Clipper2Lib::Rect64 rect = {tileID.x * mbgl::util::EXTENT - clipper_offset,
                                tileID.y * mbgl::util::EXTENT - clipper_offset,
                                (tileID.x+1) * mbgl::util::EXTENT + clipper_offset,
                                (tileID.y+1) * mbgl::util::EXTENT + clipper_offset};
    Clipper2Lib::Path64 line;
    for (auto point_ : line_) {
        Clipper2Lib::Point<int64_t> point = {point_.x, point_.y};
        line.emplace_back(point);
    }
    Clipper2Lib::Paths64 solution;
    solution = Clipper2Lib::RectClipLines(rect, line);
    
    if(solution.size() > 0 && solution[0].size() > 0) {
        return Point<int64_t>(solution[0][0].x, solution[0][0].y);
    }
    return Point<int64_t>(0, 0);
}

// #*# 未来大概率需要用于路况插标
void RouteLineLayerManager::add(const RoutePlanID& id, const SymbolRoutePlan& routePlan) {
//    auto impl = std::make_shared<SymbolAnnotationImpl>(id, annotation);
//    symbolTree.insert(impl);
//    symbolAnnotations.emplace(id, impl);
}

void RouteLineLayerManager::add(const RoutePlanID& id, const LineRoutePlan& routePlan) {

    // #*# 导航数据或路况数据不合法，直接返回
    if(routePlan.geometry.size() <= 1)
        return;
    if(routePlan.trafficInfo.size() < 1)
        return;
    
    // #*# 根据不同id保存导航方案，mainPlan和非mainPlan使用不同配色方案（layer paint property）显示
    // #*# 暂时只处理mainPlan数据解析和显示逻辑
    // 清理之前的tile数据
    planTiles16.clear();
    planTiles11.clear();
    planTiles6.clear();
    
    route_points = routePlan.geometry;
    trafficInfo = routePlan.trafficInfo;
    totol_distance = countTotalDistance(route_points);
    
    insertNodesForTrafficCondition();
    
    convertTileData(routePlan, planTiles16, 16);
    convertTileData(routePlan, planTiles11, 11);
    convertTileData(routePlan, planTiles6, 6);
}

#define pi 3.1415926535
double DistanceHaversine_bak(double lat1, double lon1, double lat2, double lon2)
{
    static const double radio = 6378137.0;
    double dlat, dlon;
    //computing procedure
    double a, c, distance;
    dlon = fabs((lon2 - lon1)) * pi / 180;
    dlat = fabs((lat2 - lat1)) * pi / 180;
    a = (sin(dlat / 2) * sin(dlat / 2)) + cos(lat1 * pi / 180) * cos(lat2 * pi / 180) * (sin(dlon / 2) * sin(dlon / 2));
    if(a == 1.0)
        c = pi;
    else
        c = 2 * atan(sqrt(a) / sqrt(1 - a));
    distance = radio * c;

    return distance;
}

double DistanceHaversine(double lat1, double lon1, double lat2, double lon2, int16_t prec = 100, int16_t factor = 1000)
{
    static const double EARTH_RADIUS = 6371.0;//km 地球半径 平均值，千米
    //用haversine公式计算球面两点间的距离。
    //经纬度转换成弧度
    lat1 = lat1 * pi / 180;
    lon1 = lon1 * pi / 180;
    lat2 = lat2 * pi / 180;
    lon2 = lon2 * pi / 180;
    //差值
    double vLon = fabs(lon1 - lon2);
    double vLat = fabs(lat1 - lat2);
    //h is the great circle distance in radians, great circle就是一个球体上的切面，它的圆心即是球心的一个周长最大的圆。
    double h = pow(sin(vLat / 2), 2) + cos(lat1) * cos(lat2) * pow(sin(vLon / 2), 2);
    double distance = round(2 * EARTH_RADIUS * asin(sqrt(h)) * factor * prec) / prec;
    return distance;
}

double RouteLineLayerManager::countTotalDistance(LineString<double>& line_string_) {
    double dis = 0;
    if(line_string_.size() <= 1)
        return 0;
    for(int32_t i = 1; i < line_string_.size(); i++) {
        Point<double> point1 = line_string_[i-1];
        Point<double> point2 = line_string_[i];
//        dis += DistanceHaversine_bak(point1.y, point1.x, point2.y, point2.x);
        dis += DistanceHaversine(point1.y, point1.x, point2.y, point2.x);
    }
    return dis;
}

void RouteLineLayerManager::insertNodesForTrafficCondition() {
    #define EPSILON 0.0000001
    route_points_inserted.clear();
    int32_t cond_index = 0;
    double dis_percent_count = 0.0; // 起点到a点的全路百分占比
    Point<double> point_start = route_points[0];
    route_points_inserted.emplace_back(point_start);
    for(int32_t i = 1; i < route_points.size(); i++) {
        Point<double> point1 = route_points[i-1];
        Point<double> point2 = route_points[i];
        double dis = DistanceHaversine(point1.y, point1.x, point2.y, point2.x);
        double dis_percent = dis / totol_distance;  // a->b点的全路百分占比
        for(; cond_index < trafficInfo.size(); cond_index++) {
            double dis_percent_count_tmp = dis_percent_count + dis_percent; // 起点到b(a + a->b)点的全路百分占比
            double cond_percent_tmp = trafficInfo[cond_index].percent;  // 路况节点的全路百分占比
            if(fabs(dis_percent_count_tmp - cond_percent_tmp) < EPSILON) {
                double node_percent = (cond_percent_tmp - dis_percent_count) / dis_percent;
                Point<double> point_insert;
                point_insert.x = (point2.x - point1.x) * node_percent + point1.x;
                point_insert.y = (point2.y - point1.y) * node_percent + point1.y;
                route_points_inserted.emplace_back(point_insert);
            }
            else
                break;
        }
        route_points_inserted.emplace_back(point2);
        dis_percent_count += dis_percent;
    }
    cond_index = 0;
}

void RouteLineLayerManager::convertTileData(const LineRoutePlan& routePlan,
                                            std::unordered_map<nav::stringid, LineRoutePlanTile>& planTiles_,
                                            int8_t zoom) {
    
    int8_t default_z = zoom;
    if(default_z != 16 && default_z != 11 && default_z != 6)
        default_z = 16;
    
    bool first_point = true;
    for (Point<double>& point_location : route_points) {
        mbgl::Point<int64_t> point_local;
        CanonicalTileID tileID = latLonToTileID(point_location, point_local, default_z);
        nav::stringid cur_tile_id = nav::stringid(util::toString(tileID));
        // 建立新的tile
        if(first_point) {
            LineRoutePlanTile tile(tileID);
            tile.addPoint(point_local, true);
            first_point = false;
            planTiles_.emplace(cur_tile_id, tile);
        }
        else {
            if(cur_tile_id == last_tile_id) {
                auto last_tile = planTiles_.find(last_tile_id);
                last_tile->second.addPoint(point_local);
            }
            else {
                // 需要计算两点的连接线与tile边界的交点，以交点对两个tile进行补点处理
                LineString<int64_t> tmp_line;
                tmp_line.push_back(last_point);
                tmp_line.push_back(point_local);
                // 计算出连接线与tile边界的交点
                mbgl::Point<int64_t> tmp_point_last = intersectPoint(tmp_line, *last_tileID);
                mbgl::Point<int64_t> tmp_point = intersectPoint(tmp_line, tileID);
                // 在上一个tile末尾补点
                auto last_tile = planTiles_.find(last_tile_id);
                last_tile->second.addPoint(point_local);
                // 在新tile首位补点
                LineRoutePlanTile tile(tileID);
                tile.addPoint(last_point, true);
                tile.addPoint(point_local);
                planTiles_.emplace(cur_tile_id, tile);
            }
        }
        last_tileID = &tileID;
        last_tile_id = cur_tile_id;
        last_point = point_local;
    }
    
    // #*# 当前planTiles中的全部坐标为zoom16下tile{0,0}的坐标，需要将全部坐标转换为对应tiled内的局部坐标
    for(auto& tile : planTiles_) {
        CanonicalTileID& tileID = tile.second.tileID;
        for(auto& segment : tile.second.segments) {
            for (auto& point : segment.points) {
                point.x = point.x - tileID.x * mbgl::util::EXTENT;
                point.y = point.y - tileID.y * mbgl::util::EXTENT;
            }
        }
    }
}

// #*# 未来大概率无效需废弃
void RouteLineLayerManager::add(const RoutePlanID& id, const FillRoutePlan& routePlan) {
//    ShapeAnnotationImpl& impl = *shapeAnnotations.emplace(id,
//        std::make_unique<FillAnnotationImpl>(id, annotation)).first->second;
//    impl.updateStyle(*style.get().impl);
}

void RouteLineLayerManager::update(const RoutePlanID& id, const LineRoutePlan& routePlan) {
    
    
    
}

void RouteLineLayerManager::update(const RoutePlanID& id, const SymbolRoutePlan& routePlan) {}
void RouteLineLayerManager::update(const RoutePlanID& id, const FillRoutePlan& routePlan) {}

//}

} // namespace mbgl

// clang-format on
