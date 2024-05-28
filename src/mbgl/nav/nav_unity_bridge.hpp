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
#include "mbgl/nav/nav_log.hpp"

namespace nav {

    using TileId = mbgl::CanonicalTileID;

    namespace projection {
        typedef void *(*MatrixObserver)(const double* matrix /*[16]*/);
        void setMatrixObserver(MatrixObserver observer);
        void onMatrix(const double* matrix);

        typedef void *(*TransformObserver)(const double* position /*[3]*/, const double* rotation /*[3]*/, const double* scale /*[3]*/);
        void setTransformObserver(TransformObserver observer);
        void onTransform(const double* position, const double* scale, const double* rotation);
    }

    namespace model {
        typedef void *(*TileMatrixObserver)(const TileId* tileId, const double* matrix /*[16]*/);
        void setTileMatrixObserver(TileMatrixObserver observer);
        void onTileMatrix(const TileId* tileId, const double* matrix);

        typedef void *(*TileTransformObserver)(const TileId* tileId, const double* position /*[3]*/, const double* scale /*[3]*/);
        void setTileTransformObserver(TileTransformObserver observer);
        void onTileTransform(const TileId* tileId, const double* position, const double* scale);
    }

    namespace layer {
        struct Feature {
            const TileId* tileId;
            int layerRenderIndex;
            const char* layerId;
            const char* sourceLayer;
            std::string toString() const;
        };
    
        template <typename T> struct Buffer {
            const T* ptr;
            int count;
        };

    
        // Fill Bucket data
        struct FillBucket {
            const Feature feature;
            const Buffer<uint16_t> vertices;
            const Buffer<uint16_t> lines;
            const Buffer<uint16_t> triangles;
        };
        typedef void *(*FillBucketObserver)(const FillBucket* param);
        void setFillBucketObserver(FillBucketObserver observer);
        void onAddFillBucket(const FillBucket* param);

    
        // Line Bucket data
        struct LineBucket {
            const Feature feature;
            const Buffer<uint16_t> vertices;
            const Buffer<uint16_t> triangles;
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
            const Buffer<uint16_t> triangles;
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
