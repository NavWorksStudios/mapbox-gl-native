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

typedef void *(*FillBucketObserver)(int x, int y, int z, const char* layerId, const char* sourceLayer, size_t obj,
                                    const void* vertices, size_t vertices_bytes,
                                    const void* lines, size_t lines_bytes,
                                    const void* lineSegments, size_t lineSegments_bytes,
                                    const void* triangles, size_t triangles_bytes,
                                    const void* triangleSegments, size_t triangleSegments_bytes);
void setFillBucketObserver(FillBucketObserver observer);
void onFillBucketAddFeature(const mbgl::CanonicalTileID& canonical, const char* layerId, const char* sourceLayer, size_t obj,
                            const void* vertices, size_t vertices_bytes,
                            const void* lines, size_t lines_bytes,
                            const void* lineSegments, size_t lineSegments_bytes,
                            const void* triangles, size_t triangles_bytes,
                            const void* triangleSegments, size_t triangleSegments_bytes);



typedef void *(*LineBucketObserver)(int x, int y, int z, const char* layerId, const char* sourceLayer, size_t obj,
                                    const void* vertices, size_t vertices_bytes,
                                    const void* triangles, size_t triangles_bytes,
                                    const void* segments, size_t segments_bytes);
void setLineBucketObserver(LineBucketObserver observer);
void onLineBucketAddFeature(const mbgl::CanonicalTileID& canonical, const char* layerId, const char* sourceLayer, size_t obj,
                            const void* vertices, size_t vertices_bytes,
                            const void* triangles, size_t triangles_bytes,
                            const void* segments, size_t segments_bytes);



typedef void *(*CycleBucketObserver)(int x, int y, int z, const char* layerId, const char* sourceLayer, size_t obj);
void setCycleBucketObserver(CycleBucketObserver observer);
void onCycleBucketAddFeature(const mbgl::CanonicalTileID& canonical, const char* layerId, const char* sourceLayer, size_t obj);



typedef void *(*SymbolBucketObserver)(int x, int y, int z, const char* layerId, const char* sourceLayer, size_t obj);
void setSymbolBucketObserver(SymbolBucketObserver observer);
void onSymbolBucketAddFeature(const mbgl::CanonicalTileID& canonical, const char* layerId, const char* sourceLayer, size_t obj);



typedef void *(*ExtrusionBucketObserver)(int x, int y, int z, const char* layerId, const char* sourceLayer, size_t obj,
                                         const void* vertices, size_t vertices_bytes,
                                         const void* triangles, size_t triangles_bytes,
                                         const void* segments, size_t segments_bytes);
void setExtrusionBucketObserver(ExtrusionBucketObserver observer);
void onExtrusionBucketAddFeature(const mbgl::CanonicalTileID& canonical, const char* layerId, const char* sourceLayer, size_t obj,
                                 const void* vertices, size_t vertices_bytes,
                                 const void* triangles, size_t triangles_bytes,
                                 const void* segments, size_t segments_bytes);

}
}


#endif /* nav_unity_bridge_h */
