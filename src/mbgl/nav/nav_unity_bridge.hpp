//
//  nav_unity_bridge.h
//  Mapbox GL Native
//
//  Created by BernieZhao on 2024/5/14.
//

#ifndef nav_unity_bridge_h
#define nav_unity_bridge_h

#include <mbgl/tile/tile_id.hpp>
#include "mbgl/nav/nav_mb_layer.hpp"

namespace nav {

    struct TileId {
        uint32_t x;
        uint32_t y;
        uint8_t z;
    };

    namespace matrix {

        struct Transform {
            struct Position { double x, y, z; } position;
            struct Rotation { double x, y, z; } rotation;
            struct Scale { double x, y, z; } scale;
        };

    
        // Projection Matrix
        typedef void *(*ProjectionMatrixObserver)(const double* matrix); // double[16]
        void setProjectionMatrixObserver(ProjectionMatrixObserver observer);
        // Projection Transform
        typedef void *(*ProjectionTransformObserver)(const Transform* transform);
        void setProjectionTransformObserver(ProjectionTransformObserver observer);
        // notify
        void onProjectionMatrix(const double* matrix);


        // Model Matrix
        typedef void *(*TileModelMatrixObserver)(const TileId* tileId, const double* matrix); // double[16]
        void setTileModelMatrixObserver(TileModelMatrixObserver observer);
        // Model Transform
        typedef void *(*TileModelTransformObserver)(const TileId* tileId, const Transform* transform);
        void setTileModelTransformObserver(TileModelTransformObserver observer);
        // notify
        void onTileModelMatrix(const TileId* tileId, const double* matrix);

    }

    namespace layer {

        template <typename T> struct Buffer {
            const T* ptr;
            int count;
        };
    
        struct Feature {
            TileId tileId;
            int layerRenderIndex;
            const char* layerId;
            const char* sourceLayer;
        };


        // Fill Bucket data
        struct FillBucket {
            const Feature feature;
            const Buffer<uint16_t> vertices;
            const Buffer<uint16_t> lines, lineSegments;
            const Buffer<uint16_t> triangles, triangleSegments;
        };
        typedef void *(*FillBucketObserver)(const FillBucket* param);
        void setFillBucketObserver(FillBucketObserver observer);
        void onAddFillBucket(const FillBucket* param);


        // Line Bucket data
        struct LineBucket {
            const Feature feature;
            const Buffer<uint16_t> vertices;
            const Buffer<uint16_t> triangles, triangleSegments;
        };
        typedef void *(*LineBucketObserver)(const LineBucket* param);
        void setLineBucketObserver(LineBucketObserver observer);
        void onAddLineBucket(const LineBucket* param);


        // Cycle Bucket data
        struct CycleBucket {
            const Feature feature;
        };
        typedef void *(*CycleBucketObserver)(const CycleBucket* param);
        void setCycleBucketObserver(CycleBucketObserver observer);
        void onAddCycleBucket(const CycleBucket* param);


        // Symbol Bucket data
        struct SymbolBucket {
            const Feature feature;
        };
        typedef void *(*SymbolBucketObserver)(const SymbolBucket* param);
        void setSymbolBucketObserver(SymbolBucketObserver observer);
        void onAddSymbolBucket(const SymbolBucket* param);


        // Extrusion Bucket data
        struct ExtrusionBucket {
            const Feature feature;
            const Buffer<uint16_t> vertices;
            const Buffer<uint16_t> triangles, triangleSegments;
        };
        typedef void *(*ExtrusionBucketObserver)(const ExtrusionBucket* param);
        void setExtrusionBucketObserver(ExtrusionBucketObserver observer);
        void onAddExtrusionBucket(const ExtrusionBucket* param);

        // Bucket Destroy
        typedef void *(*BucketDestroyObserver)(const Feature* param);
        void setBucketDestroyObserver(BucketDestroyObserver observer);
        void onBucketDestroy(const Feature* param);

    }
}


#endif /* nav_unity_bridge_h */
