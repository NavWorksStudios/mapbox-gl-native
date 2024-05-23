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

struct TileDataKey {
    int x, y, z, layerRenderIndex;
    const char* layerId;
    const char* sourceLayer;
};

struct Array16 {
    const uint16_t* data;
    int count;
};


// Fill Bucket data
struct FillBucketObserverParam {
    TileDataKey key;
    const Array16 vertices;
    const Array16 lines;
    const Array16 lineSegments;
    const Array16 triangles;
    const Array16 triangleSegments;

};
typedef void *(*FillBucketObserver)(const FillBucketObserverParam* param);
void setFillBucketObserver(FillBucketObserver observer);
void onFillBucketAddFeature(const mbgl::CanonicalTileID& canonical, 
                            const std::string& layerId, const std::string& sourceLayer,
                            const uint16_t* vertices, int verticesCount,
                            const uint16_t* lines, int linesCount,
                            const uint16_t* lineSegments, int lineSegmentsCount,
                            const uint16_t* triangles, int trianglesCount,
                            const uint16_t* triangleSegments, int triangleSegmentsCount);


// Line Bucket data
struct LineBucketObserverParam {
    TileDataKey key;
    const Array16 vertices;
    const Array16 triangles;
    const Array16 triangleSegments;

};
typedef void *(*LineBucketObserver)(const LineBucketObserverParam* param);
void setLineBucketObserver(LineBucketObserver observer);
void onLineBucketAddFeature(const mbgl::CanonicalTileID& canonical,
                            const std::string& layerId, const std::string& sourceLayer,
                            const uint16_t* vertices, int verticesCount,
                            const uint16_t* triangles, int trianglesCount,
                            const uint16_t* segments, int segmentsCount);


// Cycle Bucket data
struct CycleBucketObserverParam {
    TileDataKey key;
};
typedef void *(*CycleBucketObserver)(const CycleBucketObserverParam* param);
void setCycleBucketObserver(CycleBucketObserver observer);
void onCycleBucketAddFeature(const mbgl::CanonicalTileID& canonical,
                             const std::string& layerId, const std::string& sourceLayer);


// Symbol Bucket data
struct SymbolBucketObserverParam {
    TileDataKey key;
};
typedef void *(*SymbolBucketObserver)(const SymbolBucketObserverParam* param);
void setSymbolBucketObserver(SymbolBucketObserver observer);
void onSymbolBucketAddFeature(const mbgl::CanonicalTileID& canonical,
                              const std::string& layerId, const std::string& sourceLayer);


// Extrusion Bucket data
struct ExtrusionBucketObserverParam {
    TileDataKey key;
    const Array16 vertices;
    const Array16 triangles;
    const Array16 triangleSegments;

};
typedef void *(*ExtrusionBucketObserver)(const ExtrusionBucketObserverParam* param);
void setExtrusionBucketObserver(ExtrusionBucketObserver observer);
void onExtrusionBucketAddFeature(const mbgl::CanonicalTileID& canonical,
                                 const std::string& layerId, const std::string& sourceLayer,
                                 const uint16_t* vertices, int verticesCount,
                                 const uint16_t* triangles, int trianglesCount,
                                 const uint16_t* segments, int segmentsCount);

// Bucket Destroy
struct BucketDestroyObserverParam {
    TileDataKey key;
};
typedef void *(*BucketDestroyObserver)(const BucketDestroyObserverParam* param);
void setBucketDestroyObserver(BucketDestroyObserver observer);
void onBucketDestroy(const mbgl::CanonicalTileID& canonical,
                    const std::string& layerId, const std::string& sourceLayer);

}
}


#endif /* nav_unity_bridge_h */
