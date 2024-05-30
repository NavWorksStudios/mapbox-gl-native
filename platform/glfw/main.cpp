#include "glfw_view.hpp"
#include "glfw_renderer_frontend.hpp"
#include "settings_json.hpp"

#include <mbgl/gfx/backend.hpp>
#include <mbgl/renderer/renderer.hpp>
#include <mbgl/storage/database_file_source.hpp>
#include <mbgl/storage/file_source_manager.hpp>
#include <mbgl/style/style.hpp>
#include <mbgl/util/default_styles.hpp>
#include <mbgl/util/logging.hpp>
#include <mbgl/util/platform.hpp>
#include <mbgl/util/string.hpp>

#include <args.hxx>

#include <csignal>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <array>

#include "mbgl/nav/nav_unity_bridge.hpp"

std::shared_ptr<GLFWView> view;
std::shared_ptr<mbgl::Map> map;
std::shared_ptr<GLFWRendererFrontend> rendererFrontend;

mbgl::Size size = { 1280, 720 };

struct Settings : mbgl::Settings_JSON {
    bool fullscreen = false;
    bool benchmark = false;
    
    std::string token;
    std::string style;
    std::string cacheDB;
    std::string testDir;
    
    void load() {
        args::ArgumentParser argumentParser("Mapbox GL GLFW example");
        args::HelpFlag helpFlag(argumentParser, "help", "Display this help menu", {'h', "help"});
        
        args::Flag fullscreenFlag(argumentParser, "fullscreen", "Toggle fullscreen", {'f', "fullscreen"});
        args::Flag benchmarkFlag(argumentParser, "benchmark", "Toggle benchmark", {'b', "benchmark"});
        args::Flag offlineFlag(argumentParser, "offline", "Toggle offline", {'o', "offline"});
        
        args::ValueFlag<std::string> testDirValue(argumentParser, "directory", "Root directory for test generation", {"testDir"});
        args::ValueFlag<std::string> backendValue(argumentParser, "backend", "Rendering backend", {"backend"});
        args::ValueFlag<std::string> styleValue(argumentParser, "URL", "Map stylesheet", {'s', "style"});
        args::ValueFlag<std::string> cacheDBValue(argumentParser, "file", "Cache database file name", {'c', "cache"});
        args::ValueFlag<double> lonValue(argumentParser, "degrees", "Longitude", {'x', "lon"});
        args::ValueFlag<double> latValue(argumentParser, "degrees", "Latitude", {'y', "lat"});
        args::ValueFlag<double> zoomValue(argumentParser, "number", "Zoom level", {'z', "zoom"});
        args::ValueFlag<double> bearingValue(argumentParser, "degrees", "Bearing", {'b', "bearing"});
        args::ValueFlag<double> pitchValue(argumentParser, "degrees", "Pitch", {'p', "pitch"});
        
        online = !offlineFlag;
        if (lonValue) longitude = args::get(lonValue);
        if (latValue) latitude = args::get(latValue);
        if (zoomValue) zoom = args::get(zoomValue);
        if (bearingValue) bearing = args::get(bearingValue);
        if (pitchValue) pitch = args::get(pitchValue);
        
        fullscreen = fullscreenFlag ? args::get(fullscreenFlag) : false;
        benchmark = benchmarkFlag ? args::get(benchmarkFlag) : false;
        style = styleValue ? args::get(styleValue) : "";
        cacheDB = cacheDBValue ? args::get(cacheDBValue) : "/tmp/mbgl-cache.db";
        testDir = testDirValue ? args::get(testDirValue) : "";
        
        if (benchmark) {
            mbgl::Log::Info(mbgl::Event::General, "BENCHMARK MODE: Some optimizations are disabled.");
        }
        
        if (!style.empty() && style.find("://") == std::string::npos) {
            style = std::string("file://") + style;
        }
        
        // #*#*# 香港维多利亚港附近
        latitude = 22.291923137288844;
        longitude = 114.15912584919271;
        bearing = 161;
        zoom = 14.8;
        pitch = 60;
        
//        // #*#*# 西单
//        latitude = 39.905755502875508;
//        longitude = 116.36792889240701;
//        bearing = -5.3674255908200372;
//        zoom = 17.040101376780214;
//        pitch = 60;
        
        debug = mbgl::EnumType(mbgl::MapDebugOptions::NoDebug);
        
        // Set access token if present
        token = getenv("MAPBOX_ACCESS_TOKEN") ? : "";
        if (token.empty()) {
            mbgl::Log::Warning(mbgl::Event::Setup, "no access token set. mapbox.com tiles won't work.");
        }
    }
    
    void save() {
        mbgl::CameraOptions camera = map->getCameraOptions();
        latitude = camera.center->latitude();
        longitude = camera.center->longitude();
        zoom = *camera.zoom;
        bearing = *camera.bearing;
        pitch = *camera.pitch;
        debug = mbgl::EnumType(map->getDebug());
        
        mbgl::Settings_JSON::save();
        mbgl::Log::Info(mbgl::Event::General,
                        R"(Exit location: --lat="%f" --lon="%f" --zoom="%f" --bearing "%f")",
                        latitude, longitude, zoom, bearing);
    }

} settings;

void init(bool headless) {
    mbgl::ResourceOptions resourceOptions;
    resourceOptions.withCachePath(settings.cacheDB).withAccessToken(settings.token);
    
    view = std::make_shared<GLFWView>(settings.fullscreen, settings.benchmark, resourceOptions, headless);
    if (!settings.testDir.empty()) view->setTestDirectory(settings.testDir);
    
    // Resource loader controls top-level request processing and can resume / pause all managed sources simultaneously.
    std::shared_ptr<mbgl::FileSource> resourceFile =
    mbgl::FileSourceManager::get()->getFileSource(mbgl::FileSourceType::ResourceLoader, resourceOptions);

    view->setPauseResumeCallback([resourceFile]() {
        static bool isPaused = false;
        if (isPaused) {
            resourceFile->resume();
        } else {
            resourceFile->pause();
        }
        isPaused = !isPaused;
    });
    
    // Online file source.
    std::shared_ptr<mbgl::FileSource> onlineFile =
    mbgl::FileSourceManager::get()->getFileSource(mbgl::FileSourceType::Network, resourceOptions);
    if (!settings.online) {
        if (onlineFile) {
            onlineFile->setProperty("online-status", false);
            mbgl::Log::Warning(mbgl::Event::Setup, "Application is offline. Press `O` to toggle online status.");
        } else {
            mbgl::Log::Warning(mbgl::Event::Setup, "Network resource provider is not available, only local requests are supported.");
        }
    }
    view->setOnlineStatusCallback([onlineFile]() {
        if (!onlineFile) {
            mbgl::Log::Warning(mbgl::Event::Setup, "Cannot change online status. Network resource provider is not available.");
        } else {
            settings.online = !settings.online;
            onlineFile->setProperty("online-status", settings.online);
            mbgl::Log::Info(mbgl::Event::Setup, "Application is %s. Press `O` to toggle online status.", settings.online ? "online" : "offline");
        }
    });
    
    // Database file source.
    std::shared_ptr<mbgl::DatabaseFileSource> databaseFile =
    std::static_pointer_cast<mbgl::DatabaseFileSource>(std::shared_ptr<mbgl::FileSource>(mbgl::FileSourceManager::get()->getFileSource(mbgl::FileSourceType::Database, resourceOptions)));
    view->setResetCacheCallback([databaseFile]() {
        databaseFile->resetDatabase([](const std::exception_ptr& ex) {
            if (ex) {
                mbgl::Log::Error(mbgl::Event::Database, "Failed to reset cache:: %s", mbgl::util::toString(ex).c_str());
            }
        });
    });


    rendererFrontend = std::make_shared<GLFWRendererFrontend>(std::make_unique<mbgl::Renderer>(view->getRendererBackend(), view->getPixelRatio()), *view);
    
    if (!headless) {
        size = view->getSize();
    }
    
    map = std::make_shared<mbgl::Map>(*rendererFrontend,
                                      *view,
                                      mbgl::MapOptions().withSize(size).withPixelRatio(view->getPixelRatio()),
                                      resourceOptions);
    
    view->setMap(map.get());
    
    map->jumpTo(mbgl::CameraOptions()
                .withCenter(mbgl::LatLng {settings.latitude, settings.longitude})
                .withZoom(settings.zoom)
                .withBearing(settings.bearing)
                .withPitch(settings.pitch));
    
    map->setDebug(mbgl::MapDebugOptions(settings.debug));

    // Load style
    if (settings.style.empty()) {
        const char *url = getenv("MAPBOX_STYLE_URL");
        if (url == nullptr) {
            mbgl::util::default_styles::DefaultStyle newStyle = mbgl::util::default_styles::orderedStyles[0];
            settings.style = newStyle.url;
            view->setWindowTitle(newStyle.name);
        } else {
            settings.style = url;
            view->setWindowTitle(url);
        }
    }

    map->getStyle().loadURL(settings.style);
    
    view->setChangeStyleCallback([] () {
        static uint8_t currentStyleIndex;
        
        if (++currentStyleIndex == mbgl::util::default_styles::numOrderedStyles) {
            currentStyleIndex = 0;
        }
        
        mbgl::util::default_styles::DefaultStyle newStyle = mbgl::util::default_styles::orderedStyles[currentStyleIndex];
        map->getStyle().loadURL(newStyle.url);
        view->setWindowTitle(newStyle.name);
        
        mbgl::Log::Info(mbgl::Event::Setup, "Changed style to: %s", newStyle.name);
    });
}

void destroy() {
    settings.save();
    
    view->setShouldClose();
    
    // keep the delete order
    map = nullptr;
    rendererFrontend = nullptr;
    view = nullptr;
}

void quit_handler(int) {
    if (view) {
        mbgl::Log::Info(mbgl::Event::Setup, "waiting for quit...");
        view->setShouldClose();
    } else {
        exit(0);
    }
}

int main(int argc, char *argv[]) {
    args::ArgumentParser argumentParser("Mapbox GL GLFW example");

    try {
        argumentParser.ParseCLI(argc, argv);
    } catch (const args::Help&) {
        std::cout << argumentParser;
        exit(0);
    } catch (const args::ParseError& e) {
        std::cerr << e.what() << std::endl;
        std::cerr << argumentParser;
        exit(1);
    } catch (const args::ValidationError& e) {
        std::cerr << e.what() << std::endl;
        std::cerr << argumentParser;
        exit(2);
    }
    
    // sigint handling
    struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = quit_handler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, nullptr);

    settings.load();
    
    init(false);

    view->run();
    
    destroy();

    return 0;
}


extern "C" {

__attribute__((visibility ("default")))
void nav_init(int width, int height) {
    size.width = width;
    size.height = height;
    
    settings.load();
    init(true);
}

__attribute__((visibility ("default")))
void nav_update() {
    view->runOnce();
}

__attribute__((visibility ("default")))
void nav_destroy() {
    destroy();
}

__attribute__((visibility ("default")))
void nav_set_projection_matrix_observer(nav::projection::MatrixObserver observer) {
    nav::projection::setMatrixObserver(observer);
}

__attribute__((visibility ("default")))
void nav_set_projection_transform_observer(nav::projection::TransformObserver observer) {
    nav::projection::setTransformObserver(observer);
}

__attribute__((visibility ("default")))
void nav_set_tile_model_matrix_observer(nav::model::TileMatrixObserver observer) {
    nav::model::setTileMatrixObserver(observer);
}

__attribute__((visibility ("default")))
void nav_set_tile_model_transform_observer(nav::model::TileTransformObserver observer) {
    nav::model::setTileTransformObserver(observer);
}

__attribute__((visibility ("default")))
void nav_set_fill_bucket_observer(nav::layer::FillBucketObserver observer) {
    nav::layer::setFillBucketObserver(observer);
}

__attribute__((visibility ("default")))
void nav_set_line_bucket_observer(nav::layer::LineBucketObserver observer) {
    nav::layer::setLineBucketObserver(observer);
}

__attribute__((visibility ("default")))
void nav_set_symbol_bucket_observer(nav::layer::SymbolBucketObserver observer) {
    nav::layer::setSymbolBucketObserver(observer);
}

__attribute__((visibility ("default")))
void nav_set_extrusion_bucket_observer(nav::layer::ExtrusionBucketObserver observer) {
    nav::layer::setExtrusionBucketObserver(observer);
}

__attribute__((visibility ("default")))
void nav_set_bucket_destroy_observer(nav::layer::BucketDestroyObserver observer) {
    nav::layer::setBucketDestroyObserver(observer);
}

__attribute__((visibility ("default")))
void nav_on_key(int key, int action, int mods) {
    view->onKey(key, action, mods);
}

__attribute__((visibility ("default")))
void nav_on_scroll(double yoffset) {
    view->onScroll(yoffset);
}

__attribute__((visibility ("default")))
void nav_on_mouse_click(int button, int action, int modifiers) {
    view->onMouseClick(button, action, modifiers);
}

__attribute__((visibility ("default")))
void nav_on_mouse_move(double x, double y) {
    view->onMouseMove(x, y);
}

}


