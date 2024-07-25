rm -r ./build   

cmake . -B  build \
  -DCMAKE_TOOLCHAIN_FILE=/Users/liujunqi/Library/Android/sdk/ndk/27.0.12077973/build/cmake/android.toolchain.cmake \
  -DCMAKE_SYSTEM_NAME=Android \
  -DCMAKE_SYSTEM_VERSION=27 \
  -DANDROID_PLATFORM=27 \
  -DCMAKE_ANDROID_ARCH_ABI=arm64 \
  -DCMAKE_ANDROID_NDK=/Users/liujunqi/Library/Android/sdk/ndk/27.0.12077973 \
  -DCMAKE_ANDROID_STL_TYPE=gnustl_shared \

#  -DCMAKE_ANDROID_PLATFORM=android-24 \
# set(CMAKE_SYSTEM_NAME Android)
# set(CMAKE_SYSTEM_VERSION 21) # API level
# set(CMAKE_ANDROID_ARCH_ABI arm64-v8a)

cmake --build build     