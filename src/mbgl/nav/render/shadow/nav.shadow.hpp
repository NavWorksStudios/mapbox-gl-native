//
//  nav.shadow.hpp
//
//  Created by ZHM on 2024/10/11.
//

#pragma once

#include <functional>
#include <gl_headers.hpp>

#include <mbgl/map/transform_state.hpp>
#include <mbgl/tile/tile_id.hpp>
#include <mbgl/util/vectors.hpp>

#include "mbgl/nav/render/mat4.h"

namespace nav {

namespace shadow {

namespace depth {

const int width = 4096 * 1.;
const int height = 4096 * 1.;

GLuint render(int width, int height,
              std::function<bool()> renderCallback,
              std::function<void()> bindScreen = nullptr);

}   // depth

namespace frustum {

namespace ortho {

/* AABB（Axis-Aligned Bounding Box）
 “轴平行包围盒”，是一种在三维空间中常用的几何包围体。
 它是一个长方体，其边与坐标轴平行，这使得它在计算和存储上相对简单。
 */
struct AABB {
    mbgl::vec3 min = { 0 }, max = { 0 };
    void invalidate();
};

struct Frumstum {
    void update(const mbgl::TransformState& state, const std::vector<mbgl::OverscaledTileID>& tileIDs);
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

}   // nav

