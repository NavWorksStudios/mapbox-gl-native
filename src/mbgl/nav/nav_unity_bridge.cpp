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

namespace matrix {

ProjectionMatrixObserver projectionMatrixObserver = nullptr;
void setProjectionMatrixObserver(ProjectionMatrixObserver observer) { projectionMatrixObserver = observer; }
void onProjectionMatrix(const double* matrix) {
//    nav::log::i("Bridge",
//                "Projection : (%d,%d,%d) [%lf,%lf,%lf,%lf][%lf,%lf,%lf,%lf][%lf,%lf,%lf,%lf][%lf,%lf,%lf,%lf]\n",
//                0, 0, 0,
//                matrix[0], matrix[1], matrix[2], matrix[3],
//                matrix[4], matrix[5], matrix[6], matrix[7],
//                matrix[8], matrix[9], matrix[10], matrix[11],
//                matrix[12], matrix[13], matrix[14], matrix[15]);
    
    if (projectionMatrixObserver) projectionMatrixObserver(matrix);
}

ProjectionTransformObserver projectionTransformObserver = nullptr;
void setProjectionTransformObserver(ProjectionTransformObserver observer) { projectionTransformObserver = observer; }
void onProjectionTransform(const double* position, const double* scale, const double* rotation) {
    if (projectionTransformObserver) projectionTransformObserver(position, scale, rotation);
}

TileModelMatrixObserver tileModelMatrixObserver = nullptr;
void setTileModelMatrixObserver(TileModelMatrixObserver observer) { tileModelMatrixObserver = observer; }
void onTileModelMatrix(const TileId* tileId, const double* matrix) {
//    nav::log::i("Bridge",
//                "Model : (%d,%d,%d) [%lf,%lf,%lf,%lf][%lf,%lf,%lf,%lf][%lf,%lf,%lf,%lf][%lf,%lf,%lf,%lf]\n",
//                tileId->x, tileId->y, tileId->z,
//                matrix[0], matrix[1], matrix[2], matrix[3],
//                matrix[4], matrix[5], matrix[6], matrix[7],
//                matrix[8], matrix[9], matrix[10], matrix[11],
//                matrix[12], matrix[13], matrix[14], matrix[15]);
    
    if (tileModelMatrixObserver) tileModelMatrixObserver(tileId, matrix);
}


TileModelTransformObserver tileModelTransformObserver = nullptr;
void setTileModelTransformObserver(TileModelTransformObserver observer) { tileModelTransformObserver = observer; }
void onTileModelTransform(const TileId* tileId, const double* position, const double* scale) {
    if (tileModelTransformObserver) {
        tileModelTransformObserver(tileId, position, scale);
    } else {
        nav::log::i("Bridge",
                    "Model : (%d,%d,%d) T[%lf,%lf,%lf] S[%lf,%lf,%lf]\n",
                    tileId->x, tileId->y, tileId->z,
                    position[0], position[1], position[2],
                    scale[0], scale[1], scale[2]);
    }
}

}

namespace layer {

FillBucketObserver fillBucketObserver = nullptr;
void setFillBucketObserver(FillBucketObserver observer) { fillBucketObserver = observer; }
void onAddFillBucket(const FillBucket* param) { if (fillBucketObserver) fillBucketObserver(param); }

LineBucketObserver lineBucketObserver = nullptr;
void setLineBucketObserver(LineBucketObserver observer) { lineBucketObserver = observer; }
void onAddLineBucket(const LineBucket* param) { if (lineBucketObserver) lineBucketObserver(param); }

CycleBucketObserver cycleBucketObserver = nullptr;
void setCycleBucketObserver(CycleBucketObserver observer) { cycleBucketObserver = observer; }
void onAddCycleBucket(const CycleBucket* param) { if (cycleBucketObserver) cycleBucketObserver(param); }

SymbolBucketObserver symbolBucketObserver = nullptr;
void setSymbolBucketObserver(SymbolBucketObserver observer) { symbolBucketObserver = observer; }
void onAddSymbolBucket(const SymbolBucket* param) { if (symbolBucketObserver) symbolBucketObserver(param); }

ExtrusionBucketObserver extrusionBucketObserver = nullptr;
void setExtrusionBucketObserver(ExtrusionBucketObserver observer) { extrusionBucketObserver = observer; } 
void onAddExtrusionBucket(const ExtrusionBucket* param) { if (extrusionBucketObserver) extrusionBucketObserver(param); }

BucketDestroyObserver bucketDestroyObserver = nullptr;
void setBucketDestroyObserver(BucketDestroyObserver observer) { bucketDestroyObserver = observer; }
void onBucketDestroy(const Feature* param) { if (bucketDestroyObserver) bucketDestroyObserver(param); }

}

}
