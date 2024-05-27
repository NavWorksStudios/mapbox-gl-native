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

    namespace matrix {
    
        // Projection Matrix
        typedef void *(*ProjectionMatrixObserver)(const double* matrix); // double[16]
        void setProjectionMatrixObserver(ProjectionMatrixObserver observer);
        void onProjectionMatrix(const double* matrix);
    
        // Projection Transform
        typedef void *(*ProjectionTransformObserver)(const double* position, // double[3]
                                                     const double* rotation, // double[3]
                                                     const double* scale); // double[3]
        void setProjectionTransformObserver(ProjectionTransformObserver observer);
        void onProjectionTransform(const double* position, const double* scale, const double* rotation);

        // Tile Model Matrix
        typedef void *(*TileModelMatrixObserver)(const TileId* tileId, 
                                                 const double* matrix); // double[16]
        void setTileModelMatrixObserver(TileModelMatrixObserver observer);
        void onTileModelMatrix(const TileId* tileId, const double* matrix);
    
        // Tile Model Transform
        typedef void *(*TileModelTransformObserver)(const TileId* tileId,
                                                    const double* position, // double[3]
                                                    const double* scale); // double[3]
        void setTileModelTransformObserver(TileModelTransformObserver observer);
        void onTileModelTransform(const TileId* tileId, const double* position, const double* scale);

    }

    namespace layer {

        template <typename T> struct Buffer {
            const T* ptr;
            int count;
        };
    
        struct Feature {
            const TileId* tileId;
            int layerRenderIndex;
            const char* layerId;
            const char* sourceLayer;
            std::string toString() const;
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
