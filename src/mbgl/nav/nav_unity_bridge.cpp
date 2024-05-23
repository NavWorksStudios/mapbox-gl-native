//
//  nav_unity_bridge.cpp
//  Mapbox GL Native
//
//  Created by BernieZhao on 2024/5/14.
//

#include <stdlib.h>
#include "mbgl/nav/nav_unity_bridge.hpp"

#include "mbgl/nav/nav_log.hpp"
#include "mbgl/nav/nav_mb_layer.hpp"

namespace nav {
namespace unity {

ProjectionMatrixObserver projectionMatrixObserver = nullptr;

void setProjectionMatrixObserver(ProjectionMatrixObserver observer) {
    projectionMatrixObserver = observer;
}

void onProjectionMatrix(const double* matrix) {
    if (projectionMatrixObserver) {
        projectionMatrixObserver(matrix);
        
        nav::log::i("Bridge",
                    "Projection : [%lf,%lf,%lf,%lf][%lf,%lf,%lf,%lf][%lf,%lf,%lf,%lf][%lf,%lf,%lf,%lf]\n",
                    matrix[0], matrix[1], matrix[2], matrix[3],
                    matrix[4], matrix[5], matrix[6], matrix[7],
                    matrix[8], matrix[9], matrix[10], matrix[11],
                    matrix[12], matrix[13], matrix[14], matrix[15]);
    }
}



TileModelMatrixObserver tileModelMatrixObserver = nullptr;

void setTileModelMatrixObserver(TileModelMatrixObserver observer) {
    tileModelMatrixObserver = observer;
}

void onTileModelMatrix(const mbgl::CanonicalTileID& canonical, const double* matrix) {
    if (tileModelMatrixObserver) {
        tileModelMatrixObserver(canonical.x, canonical.y, canonical.z, matrix);
        
        nav::log::i("Bridge", 
                    "Model : (%d,%d,%d) [%lf,%lf,%lf,%lf][%lf,%lf,%lf,%lf][%lf,%lf,%lf,%lf][%lf,%lf,%lf,%lf]\n",
                    canonical.x, canonical.y, (int) canonical.z,
                    matrix[0], matrix[1], matrix[2], matrix[3],
                    matrix[4], matrix[5], matrix[6], matrix[7],
                    matrix[8], matrix[9], matrix[10], matrix[11],
                    matrix[12], matrix[13], matrix[14], matrix[15]);
    }
}



FillBucketObserver fillBucketObserver = nullptr;

void setFillBucketObserver(FillBucketObserver observer) {
    fillBucketObserver = observer;
}

void onFillBucketAddFeature(const mbgl::CanonicalTileID& canonical,
                            const std::string& layerId, const std::string& sourceLayer,
                            const uint16_t* vertices, int verticesCount,
                            const uint16_t* lines, int linesCount,
                            const uint16_t* lineSegments, int lineSegmentsCount,
                            const uint16_t* triangles, int trianglesCount,
                            const uint16_t* triangleSegments, int triangleSegmentsCount) {
    if (fillBucketObserver) {
        const FillBucketObserverParam param = {
            { (int) canonical.x, (int) canonical.y, (int) canonical.z, nav::mb::layerRenderIndex(layerId),
                layerId.c_str(), sourceLayer.c_str() },
            { vertices, verticesCount },
            { lines, linesCount },
            { lineSegments, lineSegmentsCount },
            { triangles, trianglesCount},
            { triangleSegments, triangleSegmentsCount } };
        fillBucketObserver(&param);
    }
}

LineBucketObserver lineBucketObserver = nullptr;

void setLineBucketObserver(LineBucketObserver observer) {
    lineBucketObserver = observer;
}

void onLineBucketAddFeature(const mbgl::CanonicalTileID& canonical,
                            const std::string& layerId, const std::string& sourceLayer,
                            const uint16_t* vertices, int verticesCount,
                            const uint16_t* triangles, int trianglesCount,
                            const uint16_t* segments, int segmentsCount) {
    if (lineBucketObserver) {
        const LineBucketObserverParam param = {
            { (int) canonical.x, (int) canonical.y, (int) canonical.z, nav::mb::layerRenderIndex(layerId),
                layerId.c_str(), sourceLayer.c_str() },
            { vertices, verticesCount },
            { triangles, trianglesCount },
            { segments, segmentsCount } };
        lineBucketObserver(&param);
    }
}

CycleBucketObserver cycleBucketObserver = nullptr;

void setCycleBucketObserver(CycleBucketObserver observer) {
    cycleBucketObserver = observer;
}

void onCycleBucketAddFeature(const mbgl::CanonicalTileID& canonical,
                             const std::string& layerId, const std::string& sourceLayer) {
    
}

SymbolBucketObserver symbolBucketObserver = nullptr;

void setSymbolBucketObserver(SymbolBucketObserver observer) {
    symbolBucketObserver = observer;
}

void onSymbolBucketAddFeature(const mbgl::CanonicalTileID& canonical,
                              const std::string& layerId, const std::string& sourceLayer) {
    
}

ExtrusionBucketObserver extrusionBucketObserver = nullptr;

void setExtrusionBucketObserver(ExtrusionBucketObserver observer) {
    extrusionBucketObserver = observer;
}

void onExtrusionBucketAddFeature(const mbgl::CanonicalTileID& canonical,
                                 const std::string& layerId, const std::string& sourceLayer,
                                 const uint16_t* vertices, int verticesCount,
                                 const uint16_t* triangles, int trianglesCount,
                                 const uint16_t* segments, int segmentsCount) {
    if (extrusionBucketObserver) {
        const ExtrusionBucketObserverParam param = {
            { (int) canonical.x, (int) canonical.y, (int) canonical.z, nav::mb::layerRenderIndex(layerId),
                layerId.c_str(), sourceLayer.c_str() },
            { vertices, verticesCount },
            { triangles, trianglesCount },
            { segments, segmentsCount } };
        extrusionBucketObserver(&param);
    }
}

BucketDestroyObserver bucketDestroyObserver = nullptr;

void setBucketDestroyObserver(BucketDestroyObserver observer) {
    bucketDestroyObserver = observer;
}

void onBucketDestroy(const mbgl::CanonicalTileID& canonical,
                     const std::string& layerId, const std::string& sourceLayer) {
    if (bucketDestroyObserver) {
        const BucketDestroyObserverParam param = {
            { (int) canonical.x, (int) canonical.y, (int) canonical.z, nav::mb::layerRenderIndex(layerId),
                layerId.c_str(), sourceLayer.c_str() } };
        bucketDestroyObserver(&param);
    }
}


}
}
