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

namespace projection {

MatrixObserver matrixObserver = nullptr;
void setMatrixObserver(MatrixObserver observer) { matrixObserver = observer; }
void onMatrix(const double* matrix) {
    //    nav::log::i("Bridge",
    //                "Projection : [%lf,%lf,%lf,%lf][%lf,%lf,%lf,%lf][%lf,%lf,%lf,%lf][%lf,%lf,%lf,%lf]\n",
    //                matrix[0], matrix[1], matrix[2], matrix[3],
    //                matrix[4], matrix[5], matrix[6], matrix[7],
    //                matrix[8], matrix[9], matrix[10], matrix[11],
    //                matrix[12], matrix[13], matrix[14], matrix[15]);
    
    if (matrixObserver) {
        matrixObserver(matrix);
    }
}

TransformObserver transformObserver = nullptr;
void setTransformObserver(TransformObserver observer) { transformObserver = observer; }
void onTransform(const double* position, const double* scale, const double* rotation) {
    //    nav::log::i("Bridge",
    //                "Projection : T[%lf,%lf,%lf] S[%lf,%lf,%lf] R[%lf,%lf,%lf]\n",
    //                position[0], position[1], position[2],
    //                scale[0], scale[1], scale[2],
    //                rotation[0], rotation[1], rotation[2]);
    
    if (transformObserver) {
        transformObserver(position, scale, rotation);
    }
}

}


namespace model {

TileMatrixObserver tileMatrixObserver = nullptr;
void setTileMatrixObserver(TileMatrixObserver observer) { tileMatrixObserver = observer; }
void onTileMatrix(const TileId* tileId, const double* matrix) {
//    nav::log::i("Bridge",
//                "Model : (%d,%d,%d) [%lf,%lf,%lf,%lf][%lf,%lf,%lf,%lf][%lf,%lf,%lf,%lf][%lf,%lf,%lf,%lf]\n",
//                tileId->x, tileId->y, tileId->z,
//                matrix[0], matrix[1], matrix[2], matrix[3],
//                matrix[4], matrix[5], matrix[6], matrix[7],
//                matrix[8], matrix[9], matrix[10], matrix[11],
//                matrix[12], matrix[13], matrix[14], matrix[15]);
    
    if (tileMatrixObserver) {
        tileMatrixObserver(tileId, matrix);
    }
}

TileTransformObserver tileTransformObserver = nullptr;
void setTileTransformObserver(TileTransformObserver observer) { tileTransformObserver = observer; }
void onTileTransform(const TileId* tileId, const double* position, const double* scale) {
    nav::log::i("Bridge",
                "Model : (%d,%d,%d) T[%lf,%lf,%lf] S[%lf,%lf,%lf]\n",
                tileId->x, tileId->y, tileId->z,
                position[0], position[1], position[2],
                scale[0], scale[1], scale[2]);
    
    if (tileTransformObserver) {
        tileTransformObserver(tileId, position, scale);
    }
}

}

namespace layer {

std::string Feature::toString() const {
    char tileid[32];
    sprintf(tileid, "(%d,%d,%d)", tileId->x, tileId->y, (int)tileId->z);
    
    std::string tile;
    tile += tileid; while (tile.length() < 20) tile += " ";
    tile += layerId; while (tile.length() < 60) tile += " ";
    tile += sourceLayer; while (tile.length() < 80) tile += " ";
    return tile;
}


FillBucketObserver fillBucketObserver = nullptr;
void setFillBucketObserver(FillBucketObserver observer) { fillBucketObserver = observer; }
void onAddFillBucket(const FillBucket* param) {
    nav::log::i("BucketObserver", "Fill : %s vertices:%d\n",
                param->feature.toString().c_str(),
                param->vertices.count);
    
    if (fillBucketObserver) {
        fillBucketObserver(param);
    }
}

LineBucketObserver lineBucketObserver = nullptr;
void setLineBucketObserver(LineBucketObserver observer) { lineBucketObserver = observer; }
void onAddLineBucket(const LineBucket* param) {
    nav::log::i("BucketObserver", "Line : %s vertices:%d\n",
                param->feature.toString().c_str(),
                param->vertices.count);
    
    if (lineBucketObserver) {
        lineBucketObserver(param);
    }
}

CycleBucketObserver cycleBucketObserver = nullptr;
void setCycleBucketObserver(CycleBucketObserver observer) { cycleBucketObserver = observer; }
void onAddCycleBucket(const CycleBucket* param) {
    if (cycleBucketObserver) {
        cycleBucketObserver(param);
    }
}

SymbolBucketObserver symbolBucketObserver = nullptr;
void setSymbolBucketObserver(SymbolBucketObserver observer) { symbolBucketObserver = observer; }
void onAddSymbolBucket(const SymbolBucket* param) {
    if (symbolBucketObserver) {
        symbolBucketObserver(param);
    }
}

ExtrusionBucketObserver extrusionBucketObserver = nullptr;
void setExtrusionBucketObserver(ExtrusionBucketObserver observer) { extrusionBucketObserver = observer; } 
void onAddExtrusionBucket(const ExtrusionBucket* param) {
    nav::log::i("BucketObserver", "Extrusion : %s vertices:%d\n",
                param->feature.toString().c_str(),
                param->vertices.count);
    
    if (extrusionBucketObserver) {
        extrusionBucketObserver(param);
    }
}

BucketDestroyObserver bucketDestroyObserver = nullptr;
void setBucketDestroyObserver(BucketDestroyObserver observer) { bucketDestroyObserver = observer; }
void onBucketDestroy(const Feature* param) {
    if (bucketDestroyObserver) {
        bucketDestroyObserver(param);
    }
}

}

}
