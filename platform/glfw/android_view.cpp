#include "android_view.hpp"
#include "android_gl_backend.hpp"
#include "android_renderer_frontend.hpp"
#include "android_window.hpp"
#include "route_demo_data.hpp"
#include "test_writer.hpp"

#include <mbgl/annotation/annotation.hpp>
#include <mbgl/gfx/backend.hpp>
#include <mbgl/gfx/backend_scope.hpp>
#include <mbgl/map/camera.hpp>
#include <mbgl/renderer/renderer.hpp>
#include <mbgl/style/expression/dsl.hpp>
#include <mbgl/style/image.hpp>
#include <mbgl/style/layers/fill_extrusion_layer.hpp>
#include <mbgl/style/layers/fill_layer.hpp>
#include <mbgl/style/layers/line_layer.hpp>
#include <mbgl/style/sources/custom_geometry_source.hpp>
#include <mbgl/style/sources/geojson_source.hpp>
#include <mbgl/style/style.hpp>
#include <mbgl/style/transition_options.hpp>
#include <mbgl/util/chrono.hpp>
#include <mbgl/util/geo.hpp>
#include <mbgl/util/interpolate.hpp>
#include <mbgl/util/io.hpp>
#include <mbgl/util/logging.hpp>
#include <mbgl/util/platform.hpp>
#include <mbgl/util/string.hpp>

#include <mapbox/cheap_ruler.hpp>
#include <mapbox/geometry.hpp>
#include <mapbox/geojson.hpp>

#include <mbgl/route/route_plan.hpp>

#if MBGL_USE_GLES2
#define GLFW_INCLUDE_ES2
#endif // MBGL_USE_GLES2
#define GL_GLEXT_PROTOTYPES

#include <cassert>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <utility>
#include <chrono>

#if defined(MBGL_RENDER_BACKEND_OPENGL) && !defined(MBGL_LAYER_LOCATION_INDICATOR_DISABLE_ALL)
#include <mbgl/style/layers/location_indicator_layer.hpp>

#include "mbgl/nav/nav.style.hpp"
#include "mbgl/nav/nav.log.hpp"

#include <mbgl/route/route_line_layer_manager.hpp>

namespace {
//const std::string mbglPuckAssetsPath{MAPBOX_PUCK_ASSETS_PATH};
const std::string mbglPuckAssetsPath{""};

mbgl::Color premultiply2(mbgl::Color c) {
    c.r *= c.a;
    c.g *= c.a;
    c.b *= c.a;
    return c;
}

std::array<double, 3> toArray2(const mbgl::LatLng &crd) {
    return {crd.latitude(), crd.longitude(), 0};
}
} // namespace
#endif

AndroidView::AndroidView(bool fullscreen_, bool benchmark_, const mbgl::ResourceOptions &options, bool headless) :
    fullscreen(fullscreen_),
    benchmark(benchmark_),
    mapResourceOptions(options.clone()) {
    
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

//#if MBGL_WITH_EGL
//    glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_EGL_CONTEXT_API);
//#endif

//#ifdef DEBUG
//    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
//#endif

//#if MBGL_USE_GLES2
//    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
//    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
//    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
//#endif

//    if (mbgl::gfx::Backend::GetType() != mbgl::gfx::Backend::Type::OpenGL) {
//        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
//    }

//    glfwWindowHint(GLFW_RED_BITS, 8);
//    glfwWindowHint(GLFW_GREEN_BITS, 8);
//    glfwWindowHint(GLFW_BLUE_BITS, 8);
//    glfwWindowHint(GLFW_ALPHA_BITS, 8);
//    glfwWindowHint(GLFW_STENCIL_BITS, 8);
//    glfwWindowHint(GLFW_DEPTH_BITS, 16);

    // #*# 响应屏幕尺寸变化，需要从android jni获得
//    glfwSetFramebufferSizeCallback(window, onFramebufferResize);
      
    AndroidWindow window_android;
    window_android.width = nav::display::width();
    window_android.height = nav::display::height();
    renderBackend = AndroidBackend::Create(&window_android, benchmark);
    if (renderBackend) {
      pixelRatio = 1.0; // or 2.0
    }
    nav::runtime::texture::load(mbglPuckAssetsPath);
}

AndroidView::~AndroidView() {
//    if (window) glfwDestroyWindow(window);
//    glfwTerminate();
    nav::runtime::texture::release();
}

void AndroidView::setMap(mbgl::Map *map_) {
    map = map_;
    map->addAnnotationImage(makeImage("default_marker", 22, 22, 1));
}

void AndroidView::setRenderFrontend(AndroidRendererFrontend* rendererFrontend_) {
    rendererFrontend = rendererFrontend_;
}

mbgl::gfx::RendererBackend &AndroidView::getRendererBackend() {
    return renderBackend->getRendererBackend();
}

//void AndroidView::onKey(GLFWwindow *window, int key, int /*scancode*/, int action, int mods) {
//    auto *view = reinterpret_cast<AndroidView *>(glfwGetWindowUserPointer(window));
//    view->onKey(key, action, mods);
//}

void AndroidView::onKey(int key, int action, int mods) {
//    if (action == GLFW_RELEASE) {
//        /*
//         * 优先处理空格键对于导航模式的控制
//         * 1. 非导航模式：无任何操作
//         * 2. 导航模式 - 运行状态：导航暂停&相机回归屏幕中心
//         * 3. 导航模式 - 暂停状态：导航恢复&相机回归屏幕中心
//         */
//        if (key == GLFW_KEY_SPACE) {
//            // animateRouteCallback = nullptr;
//            if (puck != nullptr) {
//                if(!routePaused) {
//                    if(puckFollowsCameraCenter)
//                        routePaused = true;
//                    else;
//                    puckFollowsCameraCenter = true;
//                    notFollowCounter = 0;
//                    nav::runtime::setViewMode(nav::runtime::ViewMode::Browse);
//                }
//                else { // routePaused == true
//                    routePaused = false;
//                    puckFollowsCameraCenter = true;
//                    notFollowCounter = 0;
//                    nav::runtime::setViewMode(nav::runtime::ViewMode::Spotlight);
//                }
//            }
//        }
//
//        switch (key) {
//            case GLFW_KEY_ESCAPE:
//                if (this->window) glfwSetWindowShouldClose(window, true);
//                break;
//            case GLFW_KEY_TAB:
//                cycleDebugOptions();
//                break;
//            case GLFW_KEY_X:
//                if (!mods)
//                    map->jumpTo(
//                        mbgl::CameraOptions().withCenter(mbgl::LatLng{}).withZoom(0.0).withBearing(0.0).withPitch(0.0));
//                break;
//            case GLFW_KEY_O:
//                onlineStatusCallback();
//                break;
//            case GLFW_KEY_S:
//                if (changeStyleCallback) changeStyleCallback();
//                break;
//#if not MBGL_USE_GLES2
//        case GLFW_KEY_B: {
//            auto debug = map->getDebug();
//            if (debug & mbgl::MapDebugOptions::StencilClip) {
//                debug &= ~mbgl::MapDebugOptions::StencilClip;
//                debug |= mbgl::MapDebugOptions::DepthBuffer;
//            } else if (debug & mbgl::MapDebugOptions::DepthBuffer) {
//                debug &= ~mbgl::MapDebugOptions::DepthBuffer;
//            } else {
//                debug |= mbgl::MapDebugOptions::StencilClip;
//            }
//            map->setDebug(debug);
//        } break;
//#endif // MBGL_USE_GLES2
//        case GLFW_KEY_N:
//            if (!mods)
//                map->easeTo(mbgl::CameraOptions().withBearing(0.0), mbgl::AnimationOptions {{mbgl::Milliseconds(500)}});
//            break;
//        case GLFW_KEY_Z:
//            nextOrientation();
//            break;
//        case GLFW_KEY_Q: {
//            auto result = rendererFrontend->getRenderer()->queryPointAnnotations({ {}, { double(getSize().width), double(getSize().height) } });
//            printf("visible point annotations: %lu\n", result.size());
//            auto features = rendererFrontend->getRenderer()->queryRenderedFeatures(
//                mbgl::ScreenBox{{double(getSize().width * 0.5), double(getSize().height * 0.5)},
//                                {double(getSize().width * 0.5 + 1), double(getSize().height * 0.5 + 1)}},
//                {});
//            printf("Rendered features at the center of the screen: %lu\n", features.size());
//        } break;
//        case GLFW_KEY_P:
//            pauseResumeCallback();
//            break;
//        case GLFW_KEY_C:
//            clearAnnotations();
//            break;
//        case GLFW_KEY_I:
//            resetDatabaseCallback();
//            break;
//        case GLFW_KEY_K:
//            addTargetPointAnnotations({40.712245, -74.014147});
//            break;
//        case GLFW_KEY_L:
//            addLineAnnotations({40.712245, -74.014147});
//            break;
//        case GLFW_KEY_A: {
//            static const std::vector<std::tuple<mbgl::LatLng,float,float,float>> places = {
////                { mbgl::LatLng { 22.294522, 114.157209 }, 15.722247, 71.050945, 147.869345 },
//                { mbgl::LatLng { 39.903563, 116.391363 }, 17.141364, 70.103668, -33.081634 },
//                { mbgl::LatLng { 31.242662, 121.495084 }, 15.445918, 71.050945, -179.947701 },
//                { mbgl::LatLng { 25.034055, 121.564515 }, 16.077729, 70.723858, 143.526270 },
//                { mbgl::LatLng { 3.157823, 101.711731 }, 16.335255, 57.806744, -38.003147 },
//                { mbgl::LatLng { 25.191752, 55.274737 }, 15.491167, 71.050945, -33.053848 },
//                { mbgl::LatLng { 25.129851, 55.132237 }, 15.150993, 67.077923, 177.527160 },
//                { mbgl::LatLng { 55.751815, 37.621716 }, 17.575426, 70.000000, -157.404684 },
//                { mbgl::LatLng { 50.087501, 14.421360 }, 16.215369, 70, 103.655584 },
//                { mbgl::LatLng { 41.889696, 12.491829 }, 17.483347, 67.692608, 50.031478 },
//                { mbgl::LatLng { 48.859491, 2.293157 }, 16.341660, 71.050945, 117.451230 },
//                { mbgl::LatLng { 51.500735, -0.124490 }, 17.808440, 71.050945, -141.514926 },
//                { mbgl::LatLng { 40.708050, -74.010099 }, 15.317721, 71.050945, 34.123000 },
//                { mbgl::LatLng { 35.884921, 138.527952 }, 7.137733, 0, 0 },
//                { mbgl::LatLng { 31.217935, 120.949375 }, 9.145968, 0, 0 },
//                { mbgl::LatLng { 22.670898, 115.953376 }, 5.778833, 0, 0 },
//                { mbgl::LatLng { 51.501716, -0.080117 }, 10.320973, 0, 0 },
//                { mbgl::LatLng { 40.709430, -74.027215 }, 11.335746, 0, 0 },
//            };
//
//            static size_t nextPlace = 0;
//            mbgl::CameraOptions cameraOptions;
//            const auto& place = places[nextPlace++];
//            cameraOptions.center = std::get<0>(place);
//            cameraOptions.zoom = std::get<1>(place);
//            cameraOptions.pitch = std::get<2>(place);
//            cameraOptions.bearing = std::get<3>(place);
//
//            mbgl::AnimationOptions animationOptions(mbgl::Seconds(10));
//            map->flyTo(cameraOptions, animationOptions);
//            nextPlace = nextPlace % places.size();
//        } break;
//        case GLFW_KEY_R: {
//#if 0
//            nav::runtime::setViewMode(nav::runtime::ViewMode::Spotlight);
//
//            static int index = 0;
//            if (animateRouteCallback) {
//                index = fmod(index + 1, 4);
//            }
//
//            targetLatLng = mbgl::platform::glfw::LonLatValue[index];
//            static mapbox::cheap_ruler::CheapRuler ruler { 0 };
//            ruler = mapbox::cheap_ruler::CheapRuler(mbgl::platform::glfw::LatitudeValue[index]);
//            const mapbox::geojson::geojson& route = mbgl::platform::glfw::RouteValue(index);
//            static double routeProgress = 0;
//
//            static double routeDistance;
//            const auto& geometry = route.get<mapbox::geometry::geometry<double>>();
//            const auto& lineString = geometry.get<mapbox::geometry::line_string<double>>();
//            routeDistance = ruler.lineDistance(lineString);
//
//            addLineAnnotations(targetLatLng);
//            addTargetPointAnnotations(targetLatLng);
//            addRoutePlans();
//
//            animateRouteCallback = [this, route](mbgl::Map* routeMap) {
//                // 导航模式暂停时，不处理任何逻辑直接返回
//                if(puck == nullptr)
//                    return;
//                if(routePaused)
//                    return;
//                if(!puckFollowsCameraCenter) {
//                    if(notFollowCounter++>150) {
//                        notFollowCounter = 0;
//                        puckFollowsCameraCenter = true;
//                        nav::runtime::setViewMode(nav::runtime::ViewMode::Spotlight);
//                    }
//                }
//
//                const auto& geometry = route.get<mapbox::geometry::geometry<double>>();
//                const auto& lineString = geometry.get<mapbox::geometry::line_string<double>>();
//
//                double speed = mbgl::platform::glfw::SpeedValue[index] * 2;
//                double progress = fmod(routeProgress + speed, 1.0);
//                routeProgress = progress;
//
//                // 当前相机参数
//                auto camera = routeMap->getCameraOptions();
//                // 沿导航线路进行routeDistance距离的下一个坐标点
//                auto point = ruler.along(lineString, routeProgress * routeDistance);
//                // 下一个坐标点的经纬度坐标
//                const mbgl::LatLng center { point.y, point.x };
//                // 当前相机中心点的经纬度坐标
//                auto latLng = *camera.center;
//                // 通过当前puck坐标和下一个坐标点计算puck朝向
//                double bearing = ruler.bearing({ lastPoint.x, lastPoint.y }, point);
//                // 通过当前puck(上一次记录的)坐标和下一个坐标点计算puck朝向
//                double initialBearing = ruler.bearing({ lastPoint.x, lastPoint.y }, point);
//                // 计算puck朝向与当前相机方向的差值
//                double easing = bearing - *camera.bearing;
//                easing += easing > 180.0 ? -360.0 : easing < -180 ? 360.0 : 0;
//                // 将相机差值分成12帧进行修正
//                bearing = *camera.bearing + (easing / 20);
//                // 将本次route坐标点记录保存
//                lastPoint = point;
//
//                if(puckFollowsCameraCenter) {
//                    if(firstFrameForRoute) {
//                        routeMap->jumpTo(mbgl::CameraOptions().withCenter(center).withZoom(18).withBearing(bearing).withPitch(70.0));
//                        mbgl::Size size = routeMap->getTranformStateSize();
//                        const auto& loc = routeMap->screenCoordinateToLatLng(mbgl::ScreenCoordinate{static_cast<double>(size.width)/2, static_cast<double>(size.height)*2/5});
//                        routeMap->jumpTo(mbgl::CameraOptions().withCenter(loc).withZoom(18).withBearing(bearing).withPitch(70.0));
//                    }
//                    puck->setLocation(toArray({point.y, point.x}));
//                    puck->setBearing(mbgl::style::Rotation(initialBearing));
//                    updateLineAnnotations({point.y, point.x}, targetLatLng);
//                }
//                else {
//                    puck->setLocation(toArray({point.y, point.x}));
//                    puck->setBearing(mbgl::style::Rotation(initialBearing));
//                    updateLineAnnotations({point.y, point.x}, targetLatLng);
//                }
//            };
//
//            toggleLocationIndicatorLayer(true);
//            firstFrameForRoute = true;
//            animateRouteCallback(map);
//#endif
//        } break;
//        case GLFW_KEY_D: {
//            static const std::vector<mbgl::LatLngBounds> bounds = {
//                mbgl::LatLngBounds::hull(mbgl::LatLng{-45.0, -170.0}, mbgl::LatLng{45.0, 170.0}),  // inside
//                mbgl::LatLngBounds::hull(mbgl::LatLng{-45.0, -200.0}, mbgl::LatLng{45.0, -160.0}), // left IDL
//                mbgl::LatLngBounds::hull(mbgl::LatLng{-45.0, 160.0}, mbgl::LatLng{45.0, 200.0}),   // right IDL
//                mbgl::LatLngBounds()};
//            static size_t nextBound = 0u;
//            static mbgl::AnnotationID boundAnnotationID = std::numeric_limits<mbgl::AnnotationID>::max();
//
//            mbgl::LatLngBounds bound = bounds[nextBound++];
//            nextBound = nextBound % bounds.size();
//
//            map->setBounds(mbgl::BoundOptions().withLatLngBounds(bound));
//
//            if (bound == mbgl::LatLngBounds()) {
//                map->removeAnnotation(boundAnnotationID);
//                boundAnnotationID = std::numeric_limits<mbgl::AnnotationID>::max();
//            } else {
//                mbgl::Polygon<double> rect;
//                rect.push_back({
//                    mbgl::Point<double>{ bound.west(), bound.north() },
//                    mbgl::Point<double>{ bound.east(), bound.north() },
//                    mbgl::Point<double>{ bound.east(), bound.south() },
//                    mbgl::Point<double>{ bound.west(), bound.south() },
//                });
//
//                auto boundAnnotation = mbgl::FillAnnotation { rect, 0.5f, { makeRandomColor() }, { makeRandomColor() } };
//
//                if (boundAnnotationID == std::numeric_limits<mbgl::AnnotationID>::max()) {
//                    boundAnnotationID = map->addAnnotation(boundAnnotation);
//                } else {
//                    map->updateAnnotation(boundAnnotationID, boundAnnotation);
//                }
//            }
//        } break;
//        case GLFW_KEY_F: {
//            using namespace mbgl;
//            using namespace mbgl::style;
//            using namespace mbgl::style::expression::dsl;
//
//            auto &style = map->getStyle();
//            if (!style.getSource("states")) {
//                std::string url = "https://docs.mapbox.com/mapbox-gl-js/assets/us_states.geojson";
//                auto source = std::make_unique<GeoJSONSource>("states");
//                source->setURL(url);
//                style.addSource(std::move(source));
//
//                mbgl::CameraOptions cameraOptions;
//                cameraOptions.center = mbgl::LatLng{42.619626, -103.523181};
//                cameraOptions.zoom = 3;
//                cameraOptions.pitch = 0;
//                cameraOptions.bearing = 0;
//                map->jumpTo(cameraOptions);
//            }
//
//            auto layer = style.getLayer("state-fills");
//            if (!layer) {
//                auto fillLayer = std::make_unique<FillLayer>("state-fills", "states");
//                fillLayer->setFillColor(mbgl::Color{0.0, 0.0, 1.0, 0.5});
//                fillLayer->setFillOpacity(PropertyExpression<float>(
//                    createExpression(R"(["case", ["boolean", ["feature-state", "hover"], false], 1, 0.5])")));
//                style.addLayer(std::move(fillLayer));
//            } else {
//                layer->setVisibility(layer->getVisibility() == mbgl::style::VisibilityType::Visible
//                                         ? mbgl::style::VisibilityType::None
//                                         : mbgl::style::VisibilityType::Visible);
//            }
//
//            layer = style.getLayer("state-borders");
//            if (!layer) {
//                auto borderLayer = std::make_unique<LineLayer>("state-borders", "states");
//                borderLayer->setLineColor(mbgl::Color{0.0, 0.0, 1.0, 1.0});
//                borderLayer->setLineWidth(PropertyExpression<float>(
//                    createExpression(R"(["case", ["boolean", ["feature-state", "hover"], false], 2, 1])")));
//                style.addLayer(std::move(borderLayer));
//            } else {
//                layer->setVisibility(layer->getVisibility() == mbgl::style::VisibilityType::Visible
//                                         ? mbgl::style::VisibilityType::None
//                                         : mbgl::style::VisibilityType::Visible);
//            }
//        } break;
//        case GLFW_KEY_U: {
//            auto bounds = map->getBounds();
//            if (bounds.minPitch == mbgl::util::PITCH_MIN * mbgl::util::RAD2DEG &&
//                bounds.maxPitch == mbgl::util::PITCH_MAX * mbgl::util::RAD2DEG) {
//                mbgl::Log::Info(mbgl::Event::General, "Limiting pitch bounds to [30, 40] degrees");
//                map->setBounds(mbgl::BoundOptions().withMinPitch(30).withMaxPitch(40));
//            } else {
//                mbgl::Log::Info(mbgl::Event::General, "Resetting pitch bounds to [0, 60] degrees");
//                map->setBounds(mbgl::BoundOptions().withMinPitch(0).withMaxPitch(60));
//            }
//        } break;
//        case GLFW_KEY_G: {
//            toggleLocationIndicatorLayer(false);
//            hideCurrentLineAnnotations();
//            puck = nullptr;
//            animateRouteCallback = nullptr;
//        } break;
//        case GLFW_KEY_Y: {
//            freeCameraDemoPhase = 0;
//            freeCameraDemoStartTime = mbgl::Clock::now();
//            invalidate();
//        } break;
//        }
//    }
//
//    if (action == GLFW_RELEASE || action == GLFW_REPEAT) {
//        switch (key) {
//        case GLFW_KEY_W: popAnnotation(); break;
//        case GLFW_KEY_M: addAnimatedAnnotation(); break;
//        }
//    }
}

namespace mbgl {
namespace util {

template <>
struct Interpolator<mbgl::LatLng> {
    mbgl::LatLng operator()(const mbgl::LatLng &a, const mbgl::LatLng &b, const double t) {
        return {
            interpolate<double>(a.latitude(), b.latitude(), t),
            interpolate<double>(a.longitude(), b.longitude(), t),
        };
    }
};

} // namespace util
} // namespace mbgl

void AndroidView::updateFreeCameraDemo() {
    const mbgl::LatLng trainStartPos = {60.171367, 24.941359};
    const mbgl::LatLng trainEndPos = {60.185147, 24.936668};
    const mbgl::LatLng cameraStartPos = {60.167443, 24.927176};
    const mbgl::LatLng cameraEndPos = {60.185107, 24.933366};
    const double cameraStartAlt = 1000.0;
    const double cameraEndAlt = 150.0;
    const double duration = 8.0;

    // Interpolate between starting and ending points
    std::chrono::duration<double> deltaTime = mbgl::Clock::now() - freeCameraDemoStartTime;
    freeCameraDemoPhase = deltaTime.count() / duration;

    auto trainPos = mbgl::util::interpolate(trainStartPos, trainEndPos, freeCameraDemoPhase);
    auto cameraPos = mbgl::util::interpolate(cameraStartPos, cameraEndPos, freeCameraDemoPhase);
    auto cameraAlt = mbgl::util::interpolate(cameraStartAlt, cameraEndAlt, freeCameraDemoPhase);

    mbgl::FreeCameraOptions camera;

    // Update camera position and focus point on the map with interpolated values
    camera.setLocation({cameraPos, cameraAlt});
    camera.lookAtPoint(trainPos);

    map->setFreeCameraOptions(camera);

    if (freeCameraDemoPhase > 1.0) {
        freeCameraDemoPhase = -1.0;
    }
}

mbgl::Color AndroidView::makeRandomColor() const {
    const float r = 1.0f * float(std::rand()) / float(RAND_MAX);
    const float g = 1.0f * float(std::rand()) / float(RAND_MAX);
    const float b = 1.0f * float(std::rand()) / float(RAND_MAX);
    return { r, g, b, 1.0f };
}

mbgl::Point<double> AndroidView::makeRandomPoint() const {
    const double x = width * double(std::rand()) / RAND_MAX;
    const double y = height * double(std::rand()) / RAND_MAX;
    mbgl::LatLng latLng = map->latLngForPixel({ x, y });
    return { latLng.longitude(), latLng.latitude() };
}

std::unique_ptr<mbgl::style::Image>
AndroidView::makeImage(const std::string& id, int width, int height, float pixelRatio) {
    const int r = 255 * (double(std::rand()) / RAND_MAX);
    const int g = 255 * (double(std::rand()) / RAND_MAX);
    const int b = 255 * (double(std::rand()) / RAND_MAX);

    const int w = std::ceil(pixelRatio * width);
    const int h = std::ceil(pixelRatio * height);

    mbgl::PremultipliedImage image({ static_cast<uint32_t>(w), static_cast<uint32_t>(h) });
    auto data = reinterpret_cast<uint32_t*>(image.data.get());
    const int dist = (w / 2) * (w / 2);
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            const int dx = x - w / 2;
            const int dy = y - h / 2;
            const int diff = dist - (dx * dx + dy * dy);
            if (diff > 0) {
                const int a = std::min(0xFF, diff) * 0xFF / dist;
                // Premultiply the rgb values with alpha
                data[w * y + x] =
                    (a << 24) | ((a * r / 0xFF) << 16) | ((a * g / 0xFF) << 8) | (a * b / 0xFF);
            }
        }
    }

    return std::make_unique<mbgl::style::Image>(id, std::move(image), pixelRatio);
}

void AndroidView::nextOrientation() {
    using NO = mbgl::NorthOrientation;
    switch (map->getMapOptions().northOrientation()) {
        case NO::Upwards: map->setNorthOrientation(NO::Rightwards); break;
        case NO::Rightwards: map->setNorthOrientation(NO::Downwards); break;
        case NO::Downwards: map->setNorthOrientation(NO::Leftwards); break;
        default: map->setNorthOrientation(NO::Upwards); break;
    }
}

void AndroidView::addTargetPointAnnotations(const mbgl::LatLng& tagPosition) {
    hideCurrentTargetPointAnnotations();
    map->addAnnotationImage(std::make_unique<mbgl::style::Image>("marker-destination",
                                                                 mbgl::decodeImage(mbgl::util::read_file(mbglPuckAssetsPath + "target.png")), 1.0));
    targetAnnotationIDs.push_back(map->addAnnotation(mbgl::SymbolAnnotation { { tagPosition.longitude(), tagPosition.latitude() }, "marker-destination" }));
}

void AndroidView::hideCurrentTargetPointAnnotations() {
    if(targetAnnotationIDs.size() > 0) {
        map->removeAnnotation(targetAnnotationIDs[0]);
        targetAnnotationIDs.clear();
    }
}

void AndroidView::addRandomPointAnnotations(int count) {
    for (int i = 0; i < count; ++i) {
        annotationIDs.push_back(map->addAnnotation(mbgl::SymbolAnnotation { makeRandomPoint(), "marker-destination" }));
    }
}


void AndroidView::addRoutePlans() {
    mbgl::LineString<double> lineString;
    
    lineString.push_back({ -74.013841, 40.702449 });
    lineString.push_back({ -74.013863, 40.702462 });
    lineString.push_back({ -74.013977, 40.702548 });
    lineString.push_back({ -74.014040, 40.702595 });
    lineString.push_back({ -74.014152, 40.702685 });
    lineString.push_back({ -74.014213, 40.702749 });
    lineString.push_back({ -74.014284, 40.702835 });
    lineString.push_back({ -74.014333, 40.702911 });
    lineString.push_back({ -74.014368, 40.702978 });
    lineString.push_back({ -74.014407, 40.703066 });
    lineString.push_back({ -74.014438, 40.703152 });
    lineString.push_back({ -74.014449, 40.703209 });
    lineString.push_back({ -74.014450, 40.703263 });
    lineString.push_back({ -74.014450, 40.703332 });
    lineString.push_back({ -74.014442, 40.703401 });
    lineString.push_back({ -74.014404, 40.703614 });
    lineString.push_back({ -74.014245, 40.704524 });
    lineString.push_back({ -74.014220, 40.704633 });
    lineString.push_back({ -74.014329, 40.704667 });
    lineString.push_back({ -74.014450, 40.704705 });
    lineString.push_back({ -74.014548, 40.704733 });
    lineString.push_back({ -74.014641, 40.704756 });
    lineString.push_back({ -74.014727, 40.704776 });
    lineString.push_back({ -74.014841, 40.704799 });
    lineString.push_back({ -74.014977, 40.704827 });
    lineString.push_back({ -74.015033, 40.704838 });
    lineString.push_back({ -74.015365, 40.704905 });
    lineString.push_back({ -74.015454, 40.704921 });
    lineString.push_back({ -74.015541, 40.704933 });
    lineString.push_back({ -74.015638, 40.704945 });
    lineString.push_back({ -74.015699, 40.704950 });
    lineString.push_back({ -74.015755, 40.704953 });
    lineString.push_back({ -74.015830, 40.704952 });
    lineString.push_back({ -74.015909, 40.704949 });
    lineString.push_back({ -74.016073, 40.704935 });
    lineString.push_back({ -74.016157, 40.704927 });
    lineString.push_back({ -74.016224, 40.704921 });
    lineString.push_back({ -74.016284, 40.704910 });
    lineString.push_back({ -74.016416, 40.704882 });
    lineString.push_back({ -74.016424, 40.704918 });
    lineString.push_back({ -74.016437, 40.704962 });
    lineString.push_back({ -74.016453, 40.705007 });
    lineString.push_back({ -74.016462, 40.705041 });
    lineString.push_back({ -74.016467, 40.705072 });
    lineString.push_back({ -74.016463, 40.705112 });
    lineString.push_back({ -74.016457, 40.705150 });
    lineString.push_back({ -74.016447, 40.705189 });
    lineString.push_back({ -74.016151, 40.705949 });
    lineString.push_back({ -74.016121, 40.706032 });
    lineString.push_back({ -74.016090, 40.706121 });
    lineString.push_back({ -74.016060, 40.706214 });
    lineString.push_back({ -74.016037, 40.706296 });
    lineString.push_back({ -74.016016, 40.706383 });
    lineString.push_back({ -74.016003, 40.706450 });
    lineString.push_back({ -74.015986, 40.706549 });
    lineString.push_back({ -74.015971, 40.706613 });
    lineString.push_back({ -74.015953, 40.706677 });
    lineString.push_back({ -74.015888, 40.706844 });
    lineString.push_back({ -74.015805, 40.707053 });
    lineString.push_back({ -74.015735, 40.707222 });
    lineString.push_back({ -74.015697, 40.707307 });
    lineString.push_back({ -74.015597, 40.707520 });
    lineString.push_back({ -74.015512, 40.707701 });
    lineString.push_back({ -74.015476, 40.707784 });
    lineString.push_back({ -74.015442, 40.707859 });
    lineString.push_back({ -74.015363, 40.708065 });
    lineString.push_back({ -74.015197, 40.708495 });
    lineString.push_back({ -74.014864, 40.709446 });
    lineString.push_back({ -74.014760, 40.709725 });
    lineString.push_back({ -74.014744, 40.709777 });
    lineString.push_back({ -74.014729, 40.709827 });
    lineString.push_back({ -74.014720, 40.709873 });
    lineString.push_back({ -74.014712, 40.709925 });
    lineString.push_back({ -74.014709, 40.709998 });
    lineString.push_back({ -74.014699, 40.710139 });
    lineString.push_back({ -74.014689, 40.710215 });
    lineString.push_back({ -74.014674, 40.710286 });
    lineString.push_back({ -74.014655, 40.710373 });
    lineString.push_back({ -74.014631, 40.710477 });
    lineString.push_back({ -74.014602, 40.710583 });
    lineString.push_back({ -74.014523, 40.710825 });
    lineString.push_back({ -74.014492, 40.710899 });
    lineString.push_back({ -74.014463, 40.710966 });
    lineString.push_back({ -74.014434, 40.711033 });
    lineString.push_back({ -74.014406, 40.711098 });
    lineString.push_back({ -74.014380, 40.711171 });
    lineString.push_back({ -74.014360, 40.711250 });
    lineString.push_back({ -74.014147, 40.712245 });
    
    std::vector<mbgl::TrafficInfo> infos;
    infos.push_back({ 0.08, 0 });
    infos.push_back({ 0.25, -1 });
    infos.push_back({ 0.30, 1 });
    infos.push_back({ 0.43, 2 });
    infos.push_back({ 0.50, -2 });
    infos.push_back({ 0.60, 3 });
    infos.push_back({ 0.70, 0 });
    infos.push_back({ 0.80, -3 });
    infos.push_back({ 0.90, 1 });
    infos.push_back({ 1.0, 0 });
    
    mbgl::RoutePlanID id = map->addRoutePlans(mbgl::LineRoutePlan(lineString, infos, true));
}

void AndroidView::addLineAnnotations(const mbgl::LatLng& tagPosition) {
    hideCurrentLineAnnotations();
    mbgl::LatLng mapCenter = map->getCameraOptions().center.value();
    mbgl::LineString<double> lineString;
    lineString.push_back({ mapCenter.longitude(), mapCenter.latitude() });
    lineString.push_back({ tagPosition.longitude(), tagPosition.latitude() });
    if(annotationIDs.size() > 0) {
        map->updateAnnotation(annotationIDs[0], mbgl::LineAnnotation { lineString, 1.0f, 3.0f, { mbgl::Color::red() } });
    } else {
        annotationIDs.push_back(map->addAnnotation(mbgl::LineAnnotation { lineString, 1.0f, 3.0f, { mbgl::Color::red() } }));
    }
}

void AndroidView::hideCurrentLineAnnotations() {
    if(annotationIDs.size() > 0) {
        mbgl::LatLng mapCenter = map->getCameraOptions().center.value();
        mbgl::LineString<double> lineString;
        lineString.push_back({ mapCenter.longitude(), mapCenter.latitude() });
        lineString.push_back({ mapCenter.longitude(), mapCenter.latitude() });
        map->updateAnnotation(annotationIDs[0], mbgl::LineAnnotation { lineString, 0.0f, 0.0f, { {0.0,0.0,0.0,0.0} } });
    }
}

void AndroidView::updateLineAnnotations(const mbgl::LatLng& orgPosition, const mbgl::LatLng& tagPosition) {
    mbgl::LineString<double> lineString;
    lineString.push_back({ orgPosition.longitude(), orgPosition.latitude() });
//    lineString.push_back({ tagPosition.longitude()-0.001, tagPosition.latitude()-0.001 });
//    lineString.push_back({ tagPosition.longitude()-0.002, tagPosition.latitude()-0.002 });
//    lineString.push_back({ tagPosition.longitude()+0.04, tagPosition.latitude()-0.05 });
    lineString.push_back({ tagPosition.longitude(), tagPosition.latitude() });
    if(annotationIDs.size() > 0) {
        map->updateAnnotation(annotationIDs[0], mbgl::LineAnnotation { lineString, 1.0f, 3.0f, { mbgl::Color::red() } });
    }
}

void AndroidView::addRandomShapeAnnotations(int count) {
    for (int i = 0; i < count; ++i) {
        mbgl::Polygon<double> triangle;
        triangle.push_back({ makeRandomPoint(), makeRandomPoint(), makeRandomPoint() });
        annotationIDs.push_back(map->addAnnotation(mbgl::FillAnnotation { triangle, 0.5f, { makeRandomColor() }, { makeRandomColor() } }));
    }
}

void AndroidView::addAnimatedAnnotation() {
//    const double started = glfwGetTime();
//    animatedAnnotationIDs.push_back(map->addAnnotation(mbgl::SymbolAnnotation { { 0, 0 } , "default_marker" }));
//    animatedAnnotationAddedTimes.push_back(started);
}

void AndroidView::updateAnimatedAnnotations() {
//    const double time = glfwGetTime();
//    for (size_t i = 0; i < animatedAnnotationIDs.size(); i++) {
//        auto dt = time - animatedAnnotationAddedTimes[i];
//
//        const double period = 10;
//        const double x = dt / period * 360 - 180;
//        const double y = std::sin(dt/ period * M_PI * 2.0) * 80;
//        map->updateAnnotation(animatedAnnotationIDs[i], mbgl::SymbolAnnotation { {x, y }, "default_marker" });
//    }
}

void AndroidView::cycleDebugOptions() {
    auto debug = map->getDebug();
#if not MBGL_USE_GLES2
    if (debug & mbgl::MapDebugOptions::StencilClip)
        debug = mbgl::MapDebugOptions::NoDebug;
    else if (debug & mbgl::MapDebugOptions::Overdraw)
        debug = mbgl::MapDebugOptions::StencilClip;
#else
    if (debug & mbgl::MapDebugOptions::Overdraw) debug = mbgl::MapDebugOptions::NoDebug;
#endif // MBGL_USE_GLES2
    else if (debug & mbgl::MapDebugOptions::Collision)
        debug = mbgl::MapDebugOptions::Overdraw;
    else if (debug & mbgl::MapDebugOptions::Timestamps)
        debug = debug | mbgl::MapDebugOptions::Collision;
    else if (debug & mbgl::MapDebugOptions::ParseStatus)
        debug = debug | mbgl::MapDebugOptions::Timestamps;
    else if (debug & mbgl::MapDebugOptions::TileBorders)
        debug = debug | mbgl::MapDebugOptions::ParseStatus;
    else
        debug = mbgl::MapDebugOptions::TileBorders;

    map->setDebug(debug);
}

void AndroidView::clearAnnotations() {
    for (const auto& id : annotationIDs) {
        map->removeAnnotation(id);
    }

    annotationIDs.clear();

    for (const auto& id : animatedAnnotationIDs) {
        map->removeAnnotation(id);
    }

    animatedAnnotationIDs.clear();
}

void AndroidView::popAnnotation() {
    if (annotationIDs.empty()) {
        return;
    }

    map->removeAnnotation(annotationIDs.back());
    annotationIDs.pop_back();
}

//void AndroidView::onScroll(GLFWwindow *window, double /*xOffset*/, double yOffset) {
//    auto *view = reinterpret_cast<AndroidView *>(glfwGetWindowUserPointer(window));
//    view->onScroll(yOffset);
//}

void AndroidView::onScroll(double yOffset) {
    
    if(puck) {
        if(puckFollowsCameraCenter) {
            puckFollowsCameraCenter = false;
            notFollowCounter = 0;
            nav::runtime::setViewMode(nav::runtime::ViewMode::Browse);
        }
        else {
            notFollowCounter = 0;
        }
    }
    
    double delta = yOffset * 40;

    bool isWheel = delta != 0 && std::fmod(delta, 4.000244140625) == 0;

    double absDelta = delta < 0 ? -delta : delta;
    double scale = 2.0 / (1.0 + std::exp(-absDelta / 100.0));

    // Make the scroll wheel a bit slower.
    if (!isWheel) {
        scale = (scale - 1.0) / 2.0 + 1.0;
    }

    // Zooming out.
    if (delta < 0 && scale != 0) {
        scale = 1.0 / scale;
    }

    if (_mouseHistory.size()) {
        map->scaleBy(scale, _mouseHistory[0].coord);
    }

    // #*# 需要改造
//#if defined(MBGL_RENDER_BACKEND_OPENGL) && !defined(MBGL_LAYER_CUSTOM_DISABLE_ALL)
//    if (puck && puckFollowsCameraCenter) {
//        mbgl::LatLng mapCenter = map->getCameraOptions().center.value();
//        puck->setLocation(toArray(mapCenter));
//    }
//#endif
}

void AndroidView::onWindowResize(int width, int height) {
//    view->map->setSize({ static_cast<uint32_t>(view->width), static_cast<uint32_t>(view->height) });
    map->setSize({ static_cast<uint32_t>(width), static_cast<uint32_t>(height) });
}

void AndroidView::onFramebufferResize(int width, int height) {
    if (renderBackend)
        renderBackend->setSize({ static_cast<uint32_t>(width), static_cast<uint32_t>(height) });

    // This is only triggered when the framebuffer is resized, but not the window. It can
    // happen when you move the window between screens with a different pixel ratio.
    // We are forcing a repaint my invalidating the view, which triggers a rerender with the
    // new framebuffer dimensions.
    invalidate();
}


void AndroidView::onMouseClick(int button, int action, int modifiers) {
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    double now_ts = std::chrono::duration_cast<std::chrono::seconds>(duration).count();
    
    std::time_t result = std::time(nullptr);
    
//    if (action == GLFW_PRESS) {
//        map->setGestureInProgress(true);
//        if (button == GLFW_MOUSE_BUTTON_LEFT) {
//            if (modifiers & GLFW_MOD_CONTROL) { // rotate & pitch
//                pitching = true;
//                rotating = false;
//            } else { // move
//                tracking = true;
//            }
//        }
//
//        _mouseHistory.newSequence();
//    } else if (action == GLFW_RELEASE) {
//        const Mouse& pos = _mouseHistory[0];
//        const Mouse& from = _mouseHistory.prefer(now-0.5);
//        const double lastTime = now - pos.time;
//        const float mean_v = pos.velocity(from);
//        const float instant_v = pos.velocity(_mouseHistory.prefer(now-0.1));
//
//        if (tracking && mean_v > 50 && instant_v > 100) {
//            const float duration = pos.time - from.time;
//            const mbgl::ScreenCoordinate moved(pos.coord.x - from.coord.x, pos.coord.y - from.coord.y);
//            const mbgl::ScreenCoordinate fling(moved.x / duration,
//                                               moved.y / duration * (moved.y > 0 ? .35 : 1.8));
//            map->moveBy(fling, mbgl::AnimationOptions{{mbgl::Milliseconds((long)(mean_v * 3))}});
//
//            nav::log::w("Fling", "t:%.1lfs v:%.1lf(%.1lf) moved(%.1lf,%.1lf) fling(%.1lf,%.1lf)",
//                        duration, mean_v, instant_v, moved.x, moved.y, fling.x, fling.y);
//        } else {
//            nav::log::w("Fling", "tracking:%d slow:%.1lfs v:%.1lf(%.1lf)",
//                        (int)tracking, lastTime, mean_v, instant_v);
//        }
//
//        map->setGestureInProgress(rotating = pitching = tracking = false);
//    }
}

void AndroidView::onMouseMove(double x, double y) {
//    double now = glfwGetTime();
//
//    const auto puckProgram = [&]() {
//        if(puck) {
//            notFollowCounter = 0;
//
//            if(puckFollowsCameraCenter) {
//                puckFollowsCameraCenter = false;
//                nav::runtime::setViewMode(nav::runtime::ViewMode::Browse);
//            }
//        }
//    };
//
//    if (tracking) {
//        const double dx = x - _mouseHistory[0].coord.x;
//        const double dy = y - _mouseHistory[0].coord.y;
//        if (dx || dy) {
//            map->moveBy(mbgl::ScreenCoordinate { dx, dy });
//        }
//        puckProgram();
//    }
//
//    if (pitching) {
//        map->pitchBy((y - _mouseHistory[0].coord.y) / 2);
//        if (abs(x - _mouseHistory.prefer(now - 0.3).coord.x) > 25) {
//            pitching = false;
//            rotating = true;
//        }
//        puckProgram();
//    } else if (rotating) {
//        if (now - _mouseHistory[0].time > .2) {
//            pitching = true;
//            rotating = false;
//        } else {
//            map->rotateBy(_mouseHistory[0].coord, { x, y });
//        }
//        puckProgram();
//    }
//
//    _mouseHistory.push_back({x,y}, now);
//
//
////#if defined(MBGL_RENDER_BACKEND_OPENGL) && !defined(MBGL_LAYER_CUSTOM_DISABLE_ALL)
//    // #*#目前逻辑没啥用
////    if (puck && puckFollowsCameraCenter) {
////        mbgl::LatLng mapCenter = map->getCameraOptions().center.value();
////        puck->setLocation(toArray(mapCenter));
////    }
////#endif
//
//    auto &style = map->getStyle();
//    if (style.getLayer("state-fills")) {
//        auto screenCoordinate = _mouseHistory[0].coord;
//        const mbgl::RenderedQueryOptions queryOptions({{{nav::stringid("state-fills")}}, {}});
//        auto result = rendererFrontend->getRenderer()->queryRenderedFeatures(screenCoordinate, queryOptions);
//        using namespace mbgl;
//        FeatureState newState;
//
//        if (!result.empty()) {
//            FeatureIdentifier id = result[0].id;
//            optional<std::string> idStr = featureIDtoString(id);
//
//            if (idStr) {
//                if (featureID && (*featureID != *idStr)) {
//                    newState["hover"] = false;
//                    rendererFrontend->getRenderer()->setFeatureState("states", {}, *featureID, newState);
//                    featureID = nullopt;
//                }
//
//                if (!featureID) {
//                    newState["hover"] = true;
//                    featureID = featureIDtoString(id);
//                    rendererFrontend->getRenderer()->setFeatureState("states", {}, *featureID, newState);
//                }
//            }
//        } else {
//            if (featureID) {
//                newState["hover"] = false;
//                rendererFrontend->getRenderer()->setFeatureState("states", {}, *featureID, newState);
//                featureID = nullopt;
//            }
//        }
//        invalidate();
//    }
}

void AndroidView::run() {
//    auto callback = [&] {
//        if (window && glfwWindowShouldClose(window)) {
//            runLoop.stop();
//        } else {
//            glfwPollEvents();
//
//            if (dirty && rendererFrontend) {
//                dirty = false;
//
//                const double started = glfwGetTime();
//
//                if (nav::runtime::update()) invalidate();
//
//                if (animateRouteCallback) animateRouteCallback(map);
//
//                updateAnimatedAnnotations();
//
//                mbgl::gfx::BackendScope scope { getRendererBackend() };
//
//                rendererFrontend->render();
//
//                if (freeCameraDemoPhase >= 0.0) updateFreeCameraDemo();
//
//                report(1000 * (glfwGetTime() - started));
//
//                if (benchmark) invalidate();
//            }
//        }
//    };
//
//    frameTick.start(mbgl::Duration::zero(), mbgl::Milliseconds(1000 / 60), callback);
//
//#if defined(__APPLE__)
////    while (window && !glfwWindowShouldClose(window)) {
////        runLoop.run();
////    }
//#else
//    runLoop.run();
//#endif
}

float AndroidView::getPixelRatio() const {
    return pixelRatio;
}

mbgl::Size AndroidView::getSize() const {
    return { static_cast<uint32_t>(width), static_cast<uint32_t>(height) };
}

void AndroidView::invalidate() {
    dirty = true;
//    glfwPostEmptyEvent();
}

void AndroidView::report(float duration) {
//    // Frame timer
//    static int frameCounter = 0;
//    static float frameCost = 0;
//    static double lastReported = 0;
//
//    frameCounter++;
//    frameCost += duration;
//
//    const double currentTime = glfwGetTime();
//    if (currentTime - lastReported >= 1) {
//        frameCost /= frameCounter;
//        mbgl::Log::Info(mbgl::Event::OpenGL, "Fps:%d (Avg-Cost:%6.2fms, Max-Fps:%6.2f)",
//                        frameCounter,
//                        frameCost,
//                        1000 / frameCost);
//
//        frameCounter = 0;
//        frameCost = 0;
//        lastReported = currentTime;
//    }
}

void AndroidView::setChangeStyleCallback(std::function<void()> callback) {
    changeStyleCallback = std::move(callback);
}

void AndroidView::setShouldClose() {
//    if (window) glfwSetWindowShouldClose(window, true);
//    glfwPostEmptyEvent();
}

void AndroidView::onDidFinishLoadingStyle() {
#if defined(MBGL_RENDER_BACKEND_OPENGL) && !defined(MBGL_LAYER_CUSTOM_DISABLE_ALL)
    puck = nullptr;
#endif
}

void AndroidView::toggleLocationIndicatorLayer(bool visibility) {
#if defined(MBGL_RENDER_BACKEND_OPENGL) && !defined(MBGL_LAYER_LOCATION_INDICATOR_DISABLE_ALL)
    puck = static_cast<mbgl::style::LocationIndicatorLayer *>(map->getStyle().getLayer("puck"));
    static const mbgl::LatLng puckLocation{35.683389, 139.76525}; // A location on the crossing of 4 tiles
    if (puck == nullptr) {
        auto puckLayer = std::make_unique<mbgl::style::LocationIndicatorLayer>("puck");

        puckLayer->setLocationTransition(mbgl::style::TransitionOptions(
            mbgl::Duration::zero(), mbgl::Duration::zero())); // Note: This is used here for demo purpose.
                                                              // SDKs should not use this, or else the location
                                                              // will "jump" to positions.
        puckLayer->setLocation(toArray2(puckLocation));
        puckLayer->setAccuracyRadius(15);
        puckLayer->setAccuracyRadiusColor(
            premultiply2(mbgl::Color{0.95, 0.95, 0.95, 0})); // Note: these must be fed premultiplied

        puckLayer->setBearingTransition(mbgl::style::TransitionOptions(mbgl::Duration::zero(), mbgl::Duration::zero()));
        puckLayer->setBearing(mbgl::style::Rotation(0.0));
        puckLayer->setAccuracyRadiusBorderColor(premultiply2(mbgl::Color{0.95, 0.95, 0.95, 0}));
        puckLayer->setTopImageSize(0.18);
        puckLayer->setBearingImageSize(0.26);
        puckLayer->setShadowImageSize(0.16);
        puckLayer->setImageTiltDisplacement(7.0f); // set to 0 for a "flat" puck
        puckLayer->setPerspectiveCompensation(0.9);

        map->getStyle().addImage(std::make_unique<mbgl::style::Image>(
            "puck.png", mbgl::decodeImage(mbgl::util::read_file(mbglPuckAssetsPath + "puck.png")), 1.0));

        map->getStyle().addImage(std::make_unique<mbgl::style::Image>(
            "puck_shadow.png", mbgl::decodeImage(mbgl::util::read_file(mbglPuckAssetsPath + "puck_shadow.png")), 1.0));

        map->getStyle().addImage(std::make_unique<mbgl::style::Image>(
            "puck_hat.png", mbgl::decodeImage(mbgl::util::read_file(mbglPuckAssetsPath + "puck_hat.png")), 1.0));

        puckLayer->setBearingImage(mbgl::style::expression::Image("puck.png"));
        puckLayer->setShadowImage(mbgl::style::expression::Image("puck_shadow.png"));
        puckLayer->setTopImage(mbgl::style::expression::Image("puck_hat.png"));

        puck = puckLayer.get();
        map->getStyle().addLayer(std::move(puckLayer));
    }

//    bool visible = puck->getVisibility() == mbgl::style::VisibilityType::Visible;
    if (visibility) {
        puck->setLocation(toArray2(puckLocation));
        puck->setVisibility(mbgl::style::VisibilityType(mbgl::style::VisibilityType::Visible));
        puckFollowsCameraCenter = true;
        routePaused = false;
    } else {
        puck->setLocation(toArray2(puckLocation));
        puck->setVisibility(mbgl::style::VisibilityType(mbgl::style::VisibilityType::None));
        puckFollowsCameraCenter = true;
        routePaused = true;
    }
#endif
}

using Nanoseconds = std::chrono::nanoseconds;

void AndroidView::onWillStartRenderingFrame() {
//#if defined(MBGL_RENDER_BACKEND_OPENGL) && !defined(MBGL_LAYER_LOCATION_INDICATOR_DISABLE_ALL)
//    puck = static_cast<mbgl::style::LocationIndicatorLayer *>(map->getStyle().getLayer("puck"));
//    if (puck) {
//        // #*# 导航箭头方向
//        if(!routePaused) {
//            uint64_t ns = mbgl::Clock::now().time_since_epoch().count();
//            const double bearing = double(ns % 2000000000) / 2000000000.0 * 360.0;
//            puck->setBearing(mbgl::style::Rotation(bearing));
//        }
//    }
//#endif
}
