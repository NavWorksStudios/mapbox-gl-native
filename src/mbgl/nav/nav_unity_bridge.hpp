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

typedef void *(*FillBucketObserver)(int x, int y, int z, uint32_t* data, size_t length);
void setFillBucketObserver(FillBucketObserver observer);
void onFillBucketAddFeature(const mbgl::CanonicalTileID& canonical, uint32_t* data, size_t length);

typedef void *(*LineBucketObserver)(int x, int y, int z);
void setLineBucketObserver(LineBucketObserver observer);
void onLineBucketAddFeature(const mbgl::CanonicalTileID& canonical);

}
}


#endif /* nav_unity_bridge_h */
