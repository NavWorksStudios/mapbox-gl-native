

#define __CALLED_BY_ANDROID_JNI__
#ifdef __CALLED_BY_ANDROID_JNI__

#include <stdio.h>
#include <string.h>
#include <iostream>
#include <mbgl/platform/GLES2/gl2.h>
#include <mbgl/platform/GLES2/gl2ext.h>
#include <mbgl/platform/GLES2/gl2platform.h>

#if __APPLE__
#include <mbgl/platform/jni.h>
#else // #elif defined(__ANDROID__)
#include <jni.h>
#include "../../platform/android/src/jni.hpp"
#endif
#endif

#include <mbgl/gfx/backend.hpp>
#include <mbgl/renderer/renderer.hpp>
#include <mbgl/storage/database_file_source.hpp>
#include <mbgl/storage/file_source_manager.hpp>
#include <mbgl/style/style.hpp>
#include <mbgl/util/default_styles.hpp>
#include <mbgl/util/logging.hpp>
#include <mbgl/util/platform.hpp>
#include <mbgl/util/string.hpp>

#include "android_view.hpp"
#include "android_renderer_frontend.hpp"
#include "settings_json.hpp"

std::shared_ptr<AndroidView> view_android;
std::shared_ptr<mbgl::Map> map_android;
std::shared_ptr<AndroidRendererFrontend> rendererFrontend_android;

void init() {
#if 1
    mbgl::ResourceOptions resourceOptions;
    resourceOptions.withCachePath("/tmp/mbgl-cache.db").withAccessToken("");
    
    view_android = std::make_shared<AndroidView>(false, false, resourceOptions, false);
    
    // Resource loader controls top-level request processing and can resume / pause all managed sources simultaneously.
    std::shared_ptr<mbgl::FileSource> resourceFile =
    mbgl::FileSourceManager::get()->getFileSource(mbgl::FileSourceType::ResourceLoader, resourceOptions);
    
    view_android->setPauseResumeCallback([resourceFile]() {
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
    if (onlineFile) {
        onlineFile->setProperty("online-status", false);
        mbgl::Log::Warning(mbgl::Event::Setup, "Application is offline. Press `O` to toggle online status.");
    } else {
        mbgl::Log::Warning(mbgl::Event::Setup, "Network resource provider is not available, only local requests are supported.");
    }
    
    view_android->setOnlineStatusCallback([onlineFile]() {
        if (!onlineFile) {
            mbgl::Log::Warning(mbgl::Event::Setup, "Cannot change online status. Network resource provider is not available.");
        } else {
            onlineFile->setProperty("online-status", true);
        }
    });
    
    // Database file source.
    std::shared_ptr<mbgl::DatabaseFileSource> databaseFile =
    std::static_pointer_cast<mbgl::DatabaseFileSource>(std::shared_ptr<mbgl::FileSource>(mbgl::FileSourceManager::get()->getFileSource(mbgl::FileSourceType::Database, resourceOptions)));
    view_android->setResetCacheCallback([databaseFile]() {
        databaseFile->resetDatabase([](const std::exception_ptr& ex) {
            if (ex) {
                mbgl::Log::Error(mbgl::Event::Database, "Failed to reset cache:: %s", mbgl::util::toString(ex).c_str());
            }
        });
    });


    // 渲染前端
    rendererFrontend_android = std::make_shared<AndroidRendererFrontend>(std::make_unique<mbgl::Renderer>(view_android->getRendererBackend(), view_android->getPixelRatio()), *view_android);
    
    // 地图引擎和渲染后端
    map_android = std::make_shared<mbgl::Map>(*rendererFrontend_android,
                                      *view_android,
                                      mbgl::MapOptions().withSize(view_android->getSize()).withPixelRatio(view_android->getPixelRatio()),
                                      resourceOptions);
    
    view_android->setMap(map_android.get());
    map_android->setDebug(mbgl::MapDebugOptions::NoDebug);

    // Load style from default theme config
    map_android->getStyle().loadURL("");
    
    view_android->setChangeStyleCallback([] () {
        static uint8_t currentStyleIndex;
        
        if (++currentStyleIndex == mbgl::util::default_styles::numOrderedStyles) {
            currentStyleIndex = 0;
        }
        
        mbgl::util::default_styles::DefaultStyle newStyle = mbgl::util::default_styles::orderedStyles[currentStyleIndex];
        map_android->getStyle().loadURL(newStyle.url);
        
        mbgl::Log::Info(mbgl::Event::Setup, "Changed style to: %s", newStyle.name);
    });
    

    mbgl::CameraOptions cameraOptions;
    cameraOptions.center = mbgl::LatLng { 31.239624, 121.496711 };
    cameraOptions.zoom = 16.257626;
    cameraOptions.pitch = 69.576946;
    cameraOptions.bearing = 78.500186;
    
    mbgl::AnimationOptions animationOptions(mbgl::Seconds(10));
    map_android->flyTo(cameraOptions, animationOptions);
#endif
}




// API exported
JavaVM* theJVM;
jobject glesView;

extern "C" JNIEXPORT void JNICALL
Java_com_navworksstudios_navworksandroid_GLESView_Init(
        JNIEnv* env,
        jobject pthis,
        jobject glesview) {
    glesView = env->NewGlobalRef(glesview);;
    env->GetJavaVM(&theJVM);
#if __APPLE__
#else
    mbgl::android::theJVM = theJVM;
    mbgl::android::registerNatives(theJVM);
#endif
    init();
    
    glClearColor(1.0f, 0.0f, 1.0f, 1.0f);
}
 
extern "C" JNIEXPORT void JNICALL
Java_com_navworksstudios_navworksandroid_GLESView_OnViewportChanged(
        JNIEnv* env,
        jobject,
        jint width,
        jint height) {
    glViewport(0, 0, width, height);
}

extern "C" JNIEXPORT void JNICALL
Java_com_navworksstudios_navworksandroid_GLESView_StartNav(
        JNIEnv* env,
        jobject /* this */) {
    
}

extern "C" JNIEXPORT void JNICALL
Java_com_navworksstudios_navworksandroid_GLESView_StopNav(
        JNIEnv* env,
        jobject /* this */) {
    
}

extern "C" JNIEXPORT void JNICALL
Java_com_navworksstudios_navworksandroid_GLESView_Render(
        JNIEnv* env,
        jobject /* this */) {
//    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
    view_android->run();
}

extern "C" JNIEXPORT void JNICALL
Java_com_navworksstudios_navworksandroid_GLESView_SetZoom(   // 设置相机zoom级别
        JNIEnv* env,
        jobject /* this */,
        jdouble zoom) {
    
}

extern "C" JNIEXPORT void JNICALL
Java_com_navworksstudios_navworksandroid_GLESView_SetCamerPostion(   // 设置相机位(经纬度)
        JNIEnv* env,
        jobject /* this */,
        jdouble longitude,
        jdouble latitude) {
    
}

extern "C" JNIEXPORT void JNICALL
Java_com_navworksstudios_navworksandroid_GLESView_SetBearing(    // 设置相机方位
        JNIEnv* env,
        jobject /* this */,
        jdouble bearing) {
    
}

extern "C" JNIEXPORT void JNICALL
Java_com_navworksstudios_navworksandroid_GLESView_SetTilt(   // 设置相机倾角
        JNIEnv* env,
        jobject /* this */,
        jdouble pitch) {
    
}

extern "C" JNIEXPORT void JNICALL
Java_com_navworksstudios_navworksandroid_GLESView_SetPetelltColor(   // 设置主题基色，color(r,g,b,a)
        JNIEnv* env,
        jobject /* this */,
        jint rgb_r,
        jint rgb_g,
        jint rgb_b,
        jint rgb_a) {
    
}

extern "C" JNIEXPORT void JNICALL
Java_com_navworksstudios_navworksandroid_GLESView_SetTheme(   // 设置主题(编号)
        JNIEnv* env,
        jobject /* this */,
        jint theme) {
    
}

extern "C" JNIEXPORT void JNICALL
Java_com_navworksstudios_navworksandroid_GLESView_OnMouseClick(   // 鼠标(屏幕)点击
        JNIEnv* env,
        jobject /* this */,
        jint action) {
    view_android->onMouseClick(0, action, 0);
}

extern "C" JNIEXPORT void JNICALL
Java_com_navworksstudios_navworksandroid_GLESView_OnKey(   // 按键事件
        JNIEnv* env,
        jobject /* this */,
        jint key,
        jint action) {
    view_android->onKey(key, action, 0);
}

extern "C" JNIEXPORT void JNICALL
Java_com_navworksstudios_navworksandroid_GLESView_OnMouseMove(   // 鼠标(屏幕触摸点)移动
        JNIEnv* env,
        jobject /* this */,
        double x,
        double y) {
    view_android->onMouseMove(x, y);
}

void jni_navRequestRender() {
    JNIEnv* theENV;
    theJVM->AttachCurrentThread(&theENV, NULL);
    if(!glesView)
        return;
    jobject glesview = glesView;//theENV->NewGlobalRef(glesView);
    //  1.找到类
    jclass GLESViewClass = theENV->FindClass("com/navworksstudios/navworksandroid/GLESView");//第一种方式
    //  2.GLESView类里面的函数
    jmethodID navRequestRender = theENV->GetMethodID(GLESViewClass, "navRequestRender", "()V");
    //  3.调用 navRequestRender
    theENV->CallVoidMethod(glesview, navRequestRender);
}
    






