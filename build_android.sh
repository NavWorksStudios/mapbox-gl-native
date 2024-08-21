rm -r ./build   
# cmake . -A arm64-v8a -B  build \
cmake . -B  build \
  -DCMAKE_TOOLCHAIN_FILE=/Users/liujunqi/Library/Android/sdk/ndk/27.0.12077973/build/cmake/android.toolchain.cmake \
  -DANDROID_ABI=arm64-v8a \
  -DANDROID_NDK=/Users/liujunqi/Library/Android/sdk/ndk/27.0.12077973 \
  -DANDROID_PLATFORM=android-27 \
  -DANDROID_STL=c++_shared \
  -DSYSTEM_NAME=Android \
  -DCMAKE_SYSTEM_VERSION=27 \
  -DCMAKE_ANDROID_ARCH_ABI=arm64-v8a
  

#  -DCMAKEANDROID_STL=c++_shared 
#  -DCMAKEANDROID_PLATFORM=android-27 \
#  -DCMAKEANDROID_STL=c++_shared \
#   -DCMAKEANDROID_NDK=/Users/liujunqi/Library/Android/sdk/ndk/27.0.12077973 \
#   -DCMAKEANDROID_ABI=arm64-v8a \
#  -DCMAKE_ANDROID_PLATFORM=android-24 \
# set(CMAKE_SYSTEM_NAME Android)
# set(CMAKE_SYSTEM_VERSION 21) # API level
# set(CMAKE_ANDROID_ARCH_ABI arm64-v8a)

cmake --build build
#  -- VERBOSE=1

#file ./build/libmbgl-vendor-sqlite.so 
# file ./build/libmbgl-core.so
