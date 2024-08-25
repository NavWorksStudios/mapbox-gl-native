

#define __CALLED_BY_ANDROID_JNI__
#ifdef __CALLED_BY_ANDROID_JNI__

#include <stdio.h>
#include <string.h>
#include <mbgl/platform/jni.h>
#include <iostream>
#include <mbgl/platform/GLES2/gl2.h>
#include <mbgl/platform/GLES2/gl2ext.h>
#include <mbgl/platform/GLES2/gl2platform.h>

#endif

#include "glfw_view.hpp"
#include "glfw_renderer_frontend.hpp"
#include "settings_json.hpp"

std::shared_ptr<GLFWView> view_android;
std::shared_ptr<mbgl::Map> map_android;
std::shared_ptr<GLFWRendererFrontend> rendererFrontend_android;

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
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
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
    







