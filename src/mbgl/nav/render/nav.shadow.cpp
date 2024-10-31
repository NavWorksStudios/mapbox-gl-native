//
//  nav.shadow.cpp
//
//  Created by ZHM on 2024/10/11.
//

#include "mbgl/nav/render/nav.shadow.hpp"

#include "mbgl/nav/render/nav.render.hpp"
#include "mbgl/nav/nav.log.hpp"

#include <mbgl/util/mat4.hpp>
#include <mbgl/util/bounding_volumes.hpp>

#include <limits>


struct Plane {
    mbgl::vec3 normal;
    float distance;
};

struct Linesegment {
    mbgl::vec3 from;
    mbgl::vec3 to;
    
    mbgl::vec3 direction() const {
        return { to[0] - from[0], to[1] - from[1], to[2] - from[2] };
    }
};

mbgl::vec3 getIntersect(const Plane& plane, const Linesegment& line) {
    const auto dir = line.direction();
    const auto& nor = plane.normal;
    
    double denominator = nor[0] * dir[0] + nor[1] * dir[1] + nor[2] * dir[2];
    if (denominator == 0) {
        printf("getIntersect 线段与平面平行，无交点");
        assert(0);
    }
    
    mbgl::vec3 intersect;
    double t = -(nor[0] * line.from[0] + nor[1] * line.from[1] + nor[2] * line.from[2] + plane.distance) / denominator;
    if (t >= 0 && t <= 1) {
        intersect[0] = line.from[0] + t * dir[0];
        intersect[1] = line.from[1] + t * dir[1];
        intersect[2] = line.from[2] + t * dir[2];
    } else {
        printf("getIntersect 交点不在线段上");
        assert(0);
    }
    
    return intersect;
}


namespace nav {

namespace shadow {

namespace depth {

GLuint fbo = 0;
GLuint buffer = 0;

void generate(int width, int height) {
    static int w = 0, h = 0;
    if (w != width || h != height) {
        w = width;
        h = height;
        
        if (!fbo) glGenFramebuffers(1, &fbo);

        // shadow depth buffer
        glDeleteTextures(1, &buffer);
        glGenTextures(1, &buffer);
        glBindTexture(GL_TEXTURE_2D, buffer);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        float bordercolor[] = { 1.0,1.0f,1.0f,1.0f };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, bordercolor);
        
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, buffer, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}

GLuint render(int width, int height, std::function<bool()> renderCallback, std::function<void()> bindScreen) {
    depth::generate(depth::width, depth::height);
    
    if (bindScreen) {
        bindScreen();
    } else {
        glViewport(0, 0, depth::width, depth::height);
        glBindFramebuffer(GL_FRAMEBUFFER, depth::fbo);
        
        GLboolean depthMaskValue;
        glGetBooleanv(GL_DEPTH_WRITEMASK, &depthMaskValue);
        glDepthMask(GL_TRUE);
        glClear(GL_DEPTH_BUFFER_BIT);
        depthMaskValue ? glDepthMask(GL_TRUE) : glDepthMask(GL_FALSE);
    }
    
    {
        GLboolean enableCullface;
        glGetBooleanv(GL_CULL_FACE, &enableCullface);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);
        
        renderCallback();
        
        glCullFace(GL_BACK);
        enableCullface ? glEnable(GL_CULL_FACE) : glDisable(GL_CULL_FACE);
    }
    
    if (!bindScreen) glViewport(0, 0, width, height);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return depth::buffer;
}

}   // end depth


namespace frustum {

namespace ortho {

void AABB::invalidate() {
    const double MAX = std::numeric_limits<double>::max();
    min = {  MAX, MAX, MAX, };
    max = { -MAX, -MAX, -MAX, };
}

template <int MIN>
float height(float baseZ, float zoom) {
    return baseZ * pow(2, fmax(0, zoom - MIN));
}

void Frumstum::update(const mbgl::TransformState& state, const std::vector<mbgl::OverscaledTileID>& tileIDs) {
    if (tileIDs.size() == 0) return;
    
    const auto& lightViewMatrix = state.getSunlightWorldToViewMatrix();
    
    const float H_Z15_8192 = 5000.;
    const float H_Z15_512 = mbgl::util::tileSize * H_Z15_8192 / mbgl::util::EXTENT;
    
    // 可见楼块tile在光照空间的AABB
    AABB tileAABB;
    {
        // 计算最小外接
        for (const auto& tile : tileIDs) {
            // model pos
            const double h = height<15>(H_Z15_8192, tile.canonical.z);
            const mbgl::vec4 envelope[8] = {
                { 0, 0, 0, 1 },
                { 0, mbgl::util::EXTENT, 0, 1 },
                { mbgl::util::EXTENT, 0, 0, 1 },
                { mbgl::util::EXTENT, mbgl::util::EXTENT, 0, 1 },

                { 0, 0, H_Z15_8192, 1 },
                { 0, mbgl::util::EXTENT, H_Z15_8192, 1 },
                { mbgl::util::EXTENT, 0, H_Z15_8192, 1 },
                { mbgl::util::EXTENT, mbgl::util::EXTENT, H_Z15_8192, 1 },
            };
            
            mbgl::mat4 modelmatrix;
            state.matrixFor(modelmatrix, tile.toUnwrapped());

            for (int i=0; i<8; i++) {
                mbgl::vec4 pos = envelope[i];
                mbgl::matrix::transformMat4(pos, pos, modelmatrix); // to world space
                mbgl::matrix::transformMat4(pos, pos, lightViewMatrix); // to light view space
                
                const double x = pos[0];
                const double y = pos[1];
                const double z = -pos[2];
                
                auto& min = tileAABB.min;
                min[0] = fmin(min[0], x);
                min[1] = fmin(min[1], y);
                min[2] = fmin(min[2], z);
                
                auto& max = tileAABB.max;
                max[0] = fmax(max[0], x);
                max[1] = fmax(max[1], y);
                max[2] = fmax(max[2], z);
            }
        }
        
        nav::log::i("sunlight", "tile min(%8.2lf, %8.2lf, %8.2lf) max(%8.2lf, %8.2lf, %8.2lf) %d",
                    tileAABB.min[0], tileAABB.min[1], tileAABB.min[2],
                    tileAABB.max[0], tileAABB.max[1], tileAABB.max[2],
                    tileIDs.size());
    }
    
    // 可见地面在光照空间的AABB
    AABB groundAABB;
    {
        // 构造主相机视锥体
        const auto worldSize = mbgl::Projection::worldSize(state.getScale()) / mbgl::util::tileSize;
        const auto flippedY = state.getViewportMode() == mbgl::ViewportMode::FlippedY;
        const auto frustum = mbgl::util::Frustum::fromInvProjMatrix(state.getInvProjectionMatrix(), worldSize, state.getZoom(), flippedY);

//            camera
//              A                   //             far
//            near                  //  [0] ================== [1]
//            /  |                  //      \                /
//           /   |                  //     0 \______________/ 1
//          /    |                  //        \            /
//         /     |                  //         \__________/
//     ___/======|_______           //      [2]            [3]
//  top  /       |  bottom          //             near
//          far

        // 计算主相机视锥体与地面交点
        const auto& frustumPos = frustum.getPoints();
        const Plane ground = { { 0, 0, 1 }, 0 };
        const double h = height<15>(H_Z15_512, state.getZoom());
        
        mbgl::vec3 projection[4] = {
            getIntersect(ground, { frustumPos[0], frustumPos[4] }),
            getIntersect(ground, { frustumPos[1], frustumPos[5] }),
            getIntersect(ground, { frustumPos[2], frustumPos[6] }),
            getIntersect(ground, { frustumPos[3], frustumPos[7] }),
        };
        
        // 动态视野 (pitch, zoom)
        {
            // zoom-15 z(0)
            // 俯视 p(0) -> 视野100%
            // 平视 p(1) -> 视野20%
            
            // zoom-20 z(1)
            // 俯视 p(0) -> 视野100%
            // 平视 p(1) -> 视野10%

            const double z = fmax(0., fmin(1., (state.getZoom() - 15.) / 4.)); // (0, 1) 15-20
            const double p = state.getPitch() / (3.141592653589793 * 70. / 180.); // (0, 1) 俯视, 平视
            const double r = 1. - (.1 + .1 * z) * p;
            printf("DynamicView : zoom(%lf) pitch(%lf) | z(%lf) p(%lf) r(%lf)\n", state.getZoom(), state.getPitch(), z, p, r);

            auto& p0 = projection[0];
            auto& p2 = projection[2];
            p0[0] = p2[0] + (p0[0] - p2[0]) * r; // x
            p0[1] = p2[1] + (p0[1] - p2[1]) * r; // y

            auto& p1 = projection[1];
            auto& p3 = projection[3];
            p1[0] = p3[0] + (p1[0] - p3[0]) * r; // x
            p1[1] = p3[1] + (p1[1] - p3[1]) * r; // y
        }

        // 计算最小外接
        for (int i=0; i<4; i++) {
            const auto& p = projection[i];
            mbgl::vec4 vertex[2] = {
                { p[0], p[1], 0, 1 }, // vec4底
                { p[0], p[1], H_Z15_512, 1 }, // vec4顶
            };
            
            for (int i=0; i<2; i++) {
                auto& v = vertex[i];
                mbgl::matrix::transformMat4(v, v, lightViewMatrix); // to light view space
                const double x = v[0];
                const double y = v[1];
                const double z = -v[2];
                
                auto& min = groundAABB.min;
                min[0] = fmin(min[0], x);
                min[1] = fmin(min[1], y);
                min[2] = fmin(min[2], z);
                
                auto& max = groundAABB.max;
                max[0] = fmax(max[0], x);
                max[1] = fmax(max[1], y);
                max[2] = fmax(max[2], z);
            }
        }
        
        nav::log::i("sunlight", "proj min(%8.2lf, %8.2lf, %8.2lf) max(%8.2lf, %8.2lf, %8.2lf)",
                    groundAABB.min[0], groundAABB.min[1], groundAABB.min[2],
                    groundAABB.max[0], groundAABB.max[1], groundAABB.max[2]);
    }
    
    // 求tileAABB和groundAABB交集，得frustumAABB
    {
        frustumAABB.min = {
            fmax(tileAABB.min[0], groundAABB.min[0]),
            fmax(tileAABB.min[1], groundAABB.min[1]),
            fmax(tileAABB.min[2], groundAABB.min[2]),
        };
        
        frustumAABB.max = {
            fmin(tileAABB.max[0], groundAABB.max[0]),
            fmin(tileAABB.max[1], groundAABB.max[1]),
            fmin(tileAABB.max[2], groundAABB.max[2]),
        };
        
        nav::log::i("sunlight", "frus min(%8.2lf, %8.2lf, %8.2lf) max(%8.2lf, %8.2lf, %8.2lf)",
                    frustumAABB.min[0], frustumAABB.min[1], frustumAABB.min[2],
                    frustumAABB.max[0], frustumAABB.max[1], frustumAABB.min[2]);
    }
    
}

void Frumstum::render(std::function<void()> bindScreen) {
    
}

Frumstum& sunlight() {
    static Frumstum frustum;
    return frustum;
}

}


}


}   // end shadow

}   // end nav



