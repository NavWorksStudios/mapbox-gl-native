//
//  nav.shadow.frustum.hpp
//
//  Created by BernieZhao on 2024/10/31.
//

#pragma once

#include <mbgl/map/transform_state.hpp>
#include <mbgl/tile/tile_id.hpp>
#include <mbgl/util/vectors.hpp>

namespace nav {
namespace render {
namespace shadow {

namespace frustum {
namespace ortho {

/* AABB（Axis-Aligned Bounding Box）
 “轴平行包围盒”，是一种在三维空间中常用的几何包围体。
 它是一个长方体，其边与坐标轴平行，这使得它在计算和存储上相对简单。
 */
struct AABB {
    mbgl::vec3 min = { NAN }, max = { NAN };
    void invalidate();
    bool valid() const;
    void include(double x, double y, double z);
    AABB intersect(const AABB& aabb) const;
};

struct Frumstum {
    void update(const mbgl::TransformState& state, const std::vector<mbgl::OverscaledTileID>& tileIDs);
    void render();
    
    inline const AABB& getTile() const { return tileAABB; }
    inline const AABB& getGround() const { return groundAABB; }
    inline const AABB& getFrustum() const { return frustumAABB; }
    
private:
    AABB tileAABB;
    AABB groundAABB;
    AABB frustumAABB;
};

Frumstum& sunlight();

}   // ortho
}   // frustum

}   // shadow
}   // render
}   // nav

