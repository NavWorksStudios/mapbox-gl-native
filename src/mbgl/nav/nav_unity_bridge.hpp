//
//  nav_unity_bridge.h
//  Mapbox GL Native
//
//  Created by BernieZhao on 2024/5/14.
//

#ifndef nav_unity_bridge_h
#define nav_unity_bridge_h

#include <mbgl/tile/tile_id.hpp>

namespace nav {
namespace unity {

// Projection Matrix
typedef void *(*ProjectionMatrixObserver)(const double* matrix); // double[16]
void setProjectionMatrixObserver(ProjectionMatrixObserver observer);
void onProjectionMatrix(const double* matrix);



// Tile Matrix
typedef void *(*TileModelMatrixObserver)(int x, int y, int z, const double* matrix); // double[16]
void setTileModelMatrixObserver(TileModelMatrixObserver observer);
void onTileModelMatrix(const mbgl::CanonicalTileID& canonical, const double* matrix);



// Fill Bucket data
typedef void *(*FillBucketObserver)(int x, int y, int z, const char* layerId, const char* sourceLayer,
                                    const int16_t* vertices, int verticesCount,
                                    const uint16_t* lines, int linesCount,
                                    const uint16_t* lineSegments, int lineSegmentsCount,
                                    const uint16_t* triangles, int trianglesCount,
                                    const uint16_t* triangleSegments, int triangleSegmentsCount);
void setFillBucketObserver(FillBucketObserver observer);
void onFillBucketAddFeature(const mbgl::CanonicalTileID& canonical, const char* layerId, const char* sourceLayer,
                            const int16_t* vertices, int verticesCount,
                            const uint16_t* lines, int linesCount,
                            const uint16_t* lineSegments, int lineSegmentsCount,
                            const uint16_t* triangles, int trianglesCount,
                            const uint16_t* triangleSegments, int triangleSegmentsCount);


// Line Bucket data
typedef void *(*LineBucketObserver)(int x, int y, int z, const char* layerId, const char* sourceLayer,
                                    const int16_t* vertices, int verticesCount,
                                    const uint16_t* triangles, int trianglesCount,
                                    const uint16_t* segments, int segmentsCount);
void setLineBucketObserver(LineBucketObserver observer);
void onLineBucketAddFeature(const mbgl::CanonicalTileID& canonical, const char* layerId, const char* sourceLayer,
                            const int16_t* vertices, int verticesCount,
                            const uint16_t* triangles, int trianglesCount,
                            const uint16_t* segments, int segmentsCount);


// Cycle Bucket data
typedef void *(*CycleBucketObserver)(int x, int y, int z, const char* layerId, const char* sourceLayer);
void setCycleBucketObserver(CycleBucketObserver observer);
void onCycleBucketAddFeature(const mbgl::CanonicalTileID& canonical, const char* layerId, const char* sourceLayer);


// Symbol Bucket data
typedef void *(*SymbolBucketObserver)(int x, int y, int z, const char* layerId, const char* sourceLayer);
void setSymbolBucketObserver(SymbolBucketObserver observer);
void onSymbolBucketAddFeature(const mbgl::CanonicalTileID& canonical, const char* layerId, const char* sourceLayer);


// Extrusion Bucket data
typedef void *(*ExtrusionBucketObserver)(int x, int y, int z, const char* layerId, const char* sourceLayer,
                                         const int16_t* vertices, int verticesCount,
                                         const uint16_t* triangles, int trianglesCount,
                                         const uint16_t* segments, int segmentsCount);
void setExtrusionBucketObserver(ExtrusionBucketObserver observer);
void onExtrusionBucketAddFeature(const mbgl::CanonicalTileID& canonical, const char* layerId, const char* sourceLayer, 
                                 const int16_t* vertices, int verticesCount,
                                 const uint16_t* triangles, int trianglesCount,
                                 const uint16_t* segments, int segmentsCount);

}
}


#endif /* nav_unity_bridge_h */
