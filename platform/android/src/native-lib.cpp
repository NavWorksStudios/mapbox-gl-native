#include <jni.h>
#include <android/log.h>

extern "C" JNIEXPORT void JNICALL
Java_com_example_navworksandroid_MainActivity_nativeMethod(JNIEnv *env, jobject thiz) {
    // 实现需要执行的逻辑
    __android_log_print(ANDROID_LOG_DEBUG, "Native", "Hello from native method");
}

extern "C" JNIEXPORT void JNICALL
Java_com_navworksstudios_navworksandroid_NWJNI_nativeMethod(JNIEnv *env, jobject thiz) {
    // 实现需要执行的逻辑
    __android_log_print(ANDROID_LOG_DEBUG, "Native", "Hello from native method");
}
