//
//  nav_unity_bridge.cpp
//  Mapbox GL Native
//
//  Created by BernieZhao on 2024/5/14.
//

#include <stdlib.h>
#include "mbgl/nav/nav_unity_bridge.hpp"

namespace nav {
namespace unity {

FillBucketObserver fillBucketObserver = nullptr;

void setFillBucketObserver(FillBucketObserver observer) {
    fillBucketObserver = observer;
}

void onFillBucketAddFeature(const mbgl::CanonicalTileID& canonical, const char* layerId, const char* sourceLayer, size_t obj,
                            const void* vertices, size_t vertices_bytes,
                            const void* lines, size_t lines_bytes,
                            const void* lineSegments, size_t lineSegments_bytes,
                            const void* triangles, size_t triangles_bytes,
                            const void* triangleSegments, size_t triangleSegments_bytes) {
    if (fillBucketObserver)
        fillBucketObserver(canonical.x, canonical.y, canonical.z, layerId, sourceLayer, obj, vertices,
                           vertices_bytes, lines, lines_bytes, lineSegments, lineSegments_bytes,
                           triangles, triangles_bytes, triangleSegments, triangleSegments_bytes);
}

LineBucketObserver lineBucketObserver = nullptr;

void setLineBucketObserver(LineBucketObserver observer) {
    lineBucketObserver = observer;
}

void onLineBucketAddFeature(const mbgl::CanonicalTileID& canonical, const char* layerId, const char* sourceLayer, size_t obj,
                            const void* vertices, size_t vertices_bytes,
                            const void* triangles, size_t triangles_bytes,
                            const void* segments, size_t segments_bytes) {
    if (lineBucketObserver)
        lineBucketObserver(canonical.x, canonical.y, canonical.z, layerId, sourceLayer, obj,
                           vertices, vertices_bytes,
                           triangles, triangles_bytes,
                           segments, segments_bytes);
}

CycleBucketObserver cycleBucketObserver = nullptr;

void setCycleBucketObserver(CycleBucketObserver observer) {
    cycleBucketObserver = observer;
}

void onCycleBucketAddFeature(const mbgl::CanonicalTileID& canonical, const char* layerId, const char* sourceLayer, size_t obj) {
    
}

SymbolBucketObserver symbolBucketObserver = nullptr;

void setSymbolBucketObserver(SymbolBucketObserver observer) {
    symbolBucketObserver = observer;
}

void onSymbolBucketAddFeature(const mbgl::CanonicalTileID& canonical, const char* layerId, const char* sourceLayer, size_t obj) {
    
}

ExtrusionBucketObserver extrusionBucketObserver = nullptr;

void setExtrusionBucketObserver(ExtrusionBucketObserver observer) {
    extrusionBucketObserver = observer;
}

void onExtrusionBucketAddFeature(const mbgl::CanonicalTileID& canonical, const char* layerId, const char* sourceLayer, size_t obj,
                                 const void* vertices, size_t vertices_bytes,
                                 const void* triangles, size_t triangles_bytes,
                                 const void* segments, size_t segments_bytes) {
    if (extrusionBucketObserver)
        extrusionBucketObserver(canonical.x, canonical.y, canonical.z, layerId, sourceLayer, obj,
                                vertices, vertices_bytes,
                                triangles, triangles_bytes,
                                segments, segments_bytes);
    
}


}
}
