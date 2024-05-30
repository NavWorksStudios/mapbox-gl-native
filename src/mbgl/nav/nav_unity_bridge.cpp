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


#define MATH_FLOAT_SMALL            1.0e-37f
#define MATH_TOLERANCE              2e-37f
#define MATH_PIOVER2                1.57079632679489661923f
#define MATH_EPSILON                0.000001f

struct Vec3 {
    double x, y, z;
    Vec3() = default;
    Vec3(double x, double y, double z) : x(x), y(y), z(z) { }
    inline float length() const { return std::sqrt(x * x + y * y + z * z); }
};

struct Quaternion {
    double x, y, z, w;
};

namespace mat4 {

    float determinant(const double* m) {
        float a0 = m[0] * m[5] - m[1] * m[4];
        float a1 = m[0] * m[6] - m[2] * m[4];
        float a2 = m[0] * m[7] - m[3] * m[4];
        float a3 = m[1] * m[6] - m[2] * m[5];
        float a4 = m[1] * m[7] - m[3] * m[5];
        float a5 = m[2] * m[7] - m[3] * m[6];
        float b0 = m[8] * m[13] - m[9] * m[12];
        float b1 = m[8] * m[14] - m[10] * m[12];
        float b2 = m[8] * m[15] - m[11] * m[12];
        float b3 = m[9] * m[14] - m[10] * m[13];
        float b4 = m[9] * m[15] - m[11] * m[13];
        float b5 = m[10] * m[15] - m[11] * m[14];

        // Calculate the determinant.
        return (a0 * b5 - a1 * b4 + a2 * b3 + a3 * b2 - a4 * b1 + a5 * b0);
    }

    bool decompose(const double* m, Vec3* scale, Quaternion* rotation, Vec3* translation) {
        // Extract the translation.
        translation->x = m[12];
        translation->y = m[13];
        translation->z = m[14];

        // Extract the scale.
        // This is simply the length of each axis (row/column) in the matrix.
        Vec3 xaxis(m[0], m[1], m[2]);
        const double& scaleX = scale->x = xaxis.length();

        Vec3 yaxis(m[4], m[5], m[6]);
        const double& scaleY = scale->y = yaxis.length();

        Vec3 zaxis(m[8], m[9], m[10]);
        const double& scaleZ = scale->z = zaxis.length();

        // Determine if we have a negative scale (true if determinant is less than zero).
        // In this case, we simply negate a single axis of the scale.
        float det = determinant(m);
        if (det < 0) scale->z = -scale->z;


//        // Scale too close to zero, can't decompose rotation.
//        if (scaleX < MATH_TOLERANCE || scaleY < MATH_TOLERANCE || std::abs(scaleZ) < MATH_TOLERANCE)
//            return false;

        float rn;

        // Factor the scale out of the matrix axes.
        rn = 1.0f / scaleX;
        xaxis.x *= rn;
        xaxis.y *= rn;
        xaxis.z *= rn;

        rn = 1.0f / scaleY;
        yaxis.x *= rn;
        yaxis.y *= rn;
        yaxis.z *= rn;

        rn = 1.0f / scaleZ;
        zaxis.x *= rn;
        zaxis.y *= rn;
        zaxis.z *= rn;

        // Now calculate the rotation from the resulting matrix (axes).
        float trace = xaxis.x + yaxis.y + zaxis.z + 1.0f;

        if (trace > MATH_EPSILON)
        {
            float s = 0.5f / std::sqrt(trace);
            rotation->w = 0.25f / s;
            rotation->x = (yaxis.z - zaxis.y) * s;
            rotation->y = (zaxis.x - xaxis.z) * s;
            rotation->z = (xaxis.y - yaxis.x) * s;
        }
        else
        {
            // Note: since xaxis, yaxis, and zaxis are normalized,
            // we will never divide by zero in the code below.
            if (xaxis.x > yaxis.y && xaxis.x > zaxis.z)
            {
                float s = 0.5f / std::sqrt(1.0f + xaxis.x - yaxis.y - zaxis.z);
                rotation->w = (yaxis.z - zaxis.y) * s;
                rotation->x = 0.25f / s;
                rotation->y = (yaxis.x + xaxis.y) * s;
                rotation->z = (zaxis.x + xaxis.z) * s;
            }
            else if (yaxis.y > zaxis.z)
            {
                float s = 0.5f / std::sqrt(1.0f + yaxis.y - xaxis.x - zaxis.z);
                rotation->w = (zaxis.x - xaxis.z) * s;
                rotation->x = (yaxis.x + xaxis.y) * s;
                rotation->y = 0.25f / s;
                rotation->z = (zaxis.y + yaxis.z) * s;
            }
            else
            {
                float s = 0.5f / std::sqrt(1.0f + zaxis.z - xaxis.x - yaxis.y);
                rotation->w = (xaxis.y - yaxis.x ) * s;
                rotation->x = (zaxis.x + xaxis.z ) * s;
                rotation->y = (zaxis.y + yaxis.z ) * s;
                rotation->z = 0.25f / s;
            }
        }

        return true;
    }

}



namespace nav {

namespace projection {

MatrixObserver matrixObserver = nullptr;
void setMatrixObserver(MatrixObserver observer) { matrixObserver = observer; }

TransformObserver transformObserver = nullptr;
void setTransformObserver(TransformObserver observer) { transformObserver = observer; }

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
    
    if (transformObserver) {
        Vec3 scale;
        Quaternion rotation;
        Vec3 translation;
        mat4::decompose(matrix, &scale, &rotation, &translation);
        transformObserver(&translation.x, &scale.x, &rotation.x);
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
