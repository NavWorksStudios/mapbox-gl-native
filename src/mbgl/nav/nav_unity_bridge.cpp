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

void onFillBucketAddFeature(const mbgl::CanonicalTileID& canonical, const char* layerId, const char* sourceLayer,
                            const int16_t* vertices, int verticesCount,
                            const uint16_t* lines, int linesCount,
                            const uint16_t* lineSegments, int lineSegmentsCount,
                            const uint16_t* triangles, int trianglesCount,
                            const uint16_t* triangleSegments, int triangleSegmentsCount) {
    if (fillBucketObserver)
        fillBucketObserver(canonical.x, canonical.y, canonical.z, layerId, sourceLayer,
                           vertices, verticesCount,
                           lines, linesCount, lineSegments, lineSegmentsCount,
                           triangles, trianglesCount, triangleSegments, triangleSegmentsCount);
}

LineBucketObserver lineBucketObserver = nullptr;

void setLineBucketObserver(LineBucketObserver observer) {
    lineBucketObserver = observer;
}

void onLineBucketAddFeature(const mbgl::CanonicalTileID& canonical, const char* layerId, const char* sourceLayer,
                            const int16_t* vertices, int verticesCount,
                            const uint16_t* triangles, int trianglesCount,
                            const uint16_t* segments, int segmentsCount) {
    if (lineBucketObserver)
        lineBucketObserver(canonical.x, canonical.y, canonical.z, layerId, sourceLayer,
                           vertices, verticesCount,
                           triangles, trianglesCount,
                           segments, segmentsCount);
}

CycleBucketObserver cycleBucketObserver = nullptr;

void setCycleBucketObserver(CycleBucketObserver observer) {
    cycleBucketObserver = observer;
}

void onCycleBucketAddFeature(const mbgl::CanonicalTileID& canonical, const char* layerId, const char* sourceLayer) {
    
}

SymbolBucketObserver symbolBucketObserver = nullptr;

void setSymbolBucketObserver(SymbolBucketObserver observer) {
    symbolBucketObserver = observer;
}

void onSymbolBucketAddFeature(const mbgl::CanonicalTileID& canonical, const char* layerId, const char* sourceLayer) {
    
}

ExtrusionBucketObserver extrusionBucketObserver = nullptr;

void setExtrusionBucketObserver(ExtrusionBucketObserver observer) {
    extrusionBucketObserver = observer;
}

void onExtrusionBucketAddFeature(const mbgl::CanonicalTileID& canonical, const char* layerId, const char* sourceLayer,
                                 const int16_t* vertices, int verticesCount,
                                 const uint16_t* triangles, int trianglesCount,
                                 const uint16_t* segments, int segmentsCount) {
    if (extrusionBucketObserver)
        extrusionBucketObserver(canonical.x, canonical.y, canonical.z, layerId, sourceLayer,
                                vertices, verticesCount,
                                triangles, trianglesCount,
                                segments, segmentsCount);
    
}


}
}
