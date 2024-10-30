//
//  nav.render.hpp
//  Mapbox GL Native
//
//  Created by BernieZhao on 2024/10/10.
//

#pragma once

#include <functional>
#include <gl_headers.hpp>

#include <mbgl/util/mat4.hpp>
#include <mbgl/tile/tile_id.hpp>
#include <mbgl/map/transform_state.hpp>

namespace nav {

namespace renderer {

namespace ground {
void render(const mbgl::mat4& mvp, const mbgl::mat4& mv, const mbgl::mat4& normal, const mbgl::mat4& lightmvp);
}

namespace deferred {
int width();
int height();

void render(float zoom, mbgl::mat4 projMatrix,
            std::function<bool()> shadowRenderDelegate,
            std::function<bool()> geoRenderDelegate);
}

namespace util {
GLuint genTexture(GLint internalformat, GLsizei width, GLsizei height, GLenum format, GLenum type);
void renderQuad(GLint program);
}

}

namespace sunlight {

// AABB（Axis-Aligned Bounding Box）
// “轴平行包围盒”，是一种在三维空间中常用的几何包围体。它是一个长方体，其边与坐标轴平行，这使得它在计算和存储上相对简单。
struct AABB {
    const double MAX = std::numeric_limits<double>::max();

    mbgl::vec3 min = {  MAX, MAX, MAX, };
    mbgl::vec3 max = { -MAX, -MAX, -MAX, };
};

const AABB& getFrustum();
void updateFrustum(const mbgl::TransformState& state, const std::vector<mbgl::OverscaledTileID>& tileIDs);
}

}

