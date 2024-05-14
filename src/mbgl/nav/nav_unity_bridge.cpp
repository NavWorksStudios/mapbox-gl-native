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

void onFillBucketAddFeature(const mbgl::CanonicalTileID& canonical, uint32_t* data, size_t length) {
    if (fillBucketObserver) fillBucketObserver(canonical.x, canonical.y, canonical.z, data, length);
}


LineBucketObserver lineBucketObserver = nullptr;

void setLineBucketObserver(LineBucketObserver observer) {
    lineBucketObserver = observer;
}

void onLineBucketAddFeature(const mbgl::CanonicalTileID& canonical) {
    
}

}
}
