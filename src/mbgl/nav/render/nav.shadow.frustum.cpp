//
//  nav.shadow.frustum.cpp
//
//  Created by BernieZhao on 2024/10/31.
//

#include "mbgl/nav/render/nav.shadow.frustum.hpp"
#include "mbgl/nav/nav.log.hpp"
#include "mbgl/nav/render/vec3.h"
#include "mbgl/nav/render/mat4.h"

#include "mbgl/nav/render/shaders.h"
#include <mbgl/util/mat4.hpp>
#include "mbgl/nav/render/programs/nav.program.frustum.hpp"

#include <mbgl/util/bounding_volumes.hpp>
#include <array>


namespace nav {

namespace render {

namespace shadow {

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


void print(const char* name, const frustum::ortho::AABB& aabb) {
    nav::log::i("sunlight", "%s min(%8.2lf, %8.2lf, %8.2lf) max(%8.2lf, %8.2lf, %8.2lf)",
                name,
                aabb.min[0], aabb.min[1], aabb.min[2],
                aabb.max[0], aabb.max[1], aabb.max[2]);
}

void print(const char* name, const std::array<mbgl::vec3, 4>& points) {
    nav::log::i("sunlight", "%s (%8.2lf, %8.2lf, %8.2lf) (%8.2lf, %8.2lf, %8.2lf) (%8.2lf, %8.2lf, %8.2lf) (%8.2lf, %8.2lf, %8.2lf)",
                name,
                (float) points[0][0], (float) points[0][1], (float) points[0][2],
                (float) points[3][0], (float) points[3][1], (float) points[3][2],
                (float) points[1][0], (float) points[1][1], (float) points[1][2],
                (float) points[2][0], (float) points[2][1], (float) points[2][2]);
};

void print(const char* name, const std::array<mbgl::vec4, 4>& points) {
    nav::log::i("sunlight", "%s (%8.2lf, %8.2lf, %8.2lf) (%8.2lf, %8.2lf, %8.2lf) (%8.2lf, %8.2lf, %8.2lf) (%8.2lf, %8.2lf, %8.2lf)",
                name,
                (float) points[0][0], (float) points[0][1], (float) points[0][2],
                (float) points[3][0], (float) points[3][1], (float) points[3][2],
                (float) points[1][0], (float) points[1][1], (float) points[1][2],
                (float) points[2][0], (float) points[2][1], (float) points[2][2]);
};

namespace frustum {

namespace ortho {

void AABB::invalidate() {
    min = max = { NAN, NAN, NAN };
}

bool AABB::valid() const {
    if (std::isnan(min[0]) || std::isnan(min[1]) || std::isnan(min[2])) return false;
    if (std::isnan(max[0]) || std::isnan(max[1]) || std::isnan(max[2])) return false;
    return min[0] <= max[0] && min[1] <= max[1] && min[2] <= max[2];
}

void AABB::include(const mbgl::vec3& vertex) {
    min[0] = std::isnan(min[0]) ? vertex[0] : fmin(min[0], vertex[0]);
    min[1] = std::isnan(min[1]) ? vertex[1] : fmin(min[1], vertex[1]);
    min[2] = std::isnan(min[2]) ? vertex[2] : fmin(min[2], vertex[2]);
    
    max[0] = std::isnan(max[0]) ? vertex[0] : fmax(max[0], vertex[0]);
    max[1] = std::isnan(max[1]) ? vertex[1] : fmax(max[1], vertex[1]);
    max[2] = std::isnan(max[2]) ? vertex[2] : fmax(max[2], vertex[2]);
};

AABB AABB::intersect(const AABB& aabb) const {
    assert(valid());
    assert(aabb.valid());
    
    AABB intersection;
    
    intersection.min = {
        fmax(min[0], aabb.min[0]),
        fmax(min[1], aabb.min[1]),
        fmax(min[2], aabb.min[2]),
    };
    
    intersection.max = {
        fmin(max[0], aabb.max[0]),
        fmin(max[1], aabb.max[1]),
        fmin(max[2], aabb.max[2]),
    };
    
    return intersection;
}



enum HeightFormat {
    IN_WORLD = 5000,
    IN_PIXEL = int(IN_WORLD * mbgl::util::tileSize / mbgl::util::EXTENT),
};

namespace maxheight {

template <HeightFormat H> float get(float zoom) {
#if 0
    const int MIN_Z = 15;
    return H * pow(2, fmax(0, fmin(zoom, 17.) - MIN_Z));
#else
    return H;
#endif
}

}

void makeABetterFieldOfShadow(const mbgl::TransformState& state, std::array<mbgl::vec3, 4>& projection) {
    const double zf = fmax(0., fmin(1., (state.getZoom() - 15.) / 4.)); // (0, 1) 15-20
    const double pf = fmin(state.getPitch() / M_PI * 180. / 70., 1.); // (0, 1) 俯视, 平视
    
    // 查表操作，可以调整zp曲线
    //       俯视                                       平视
    //  z\p |.0 |.1 |.2 |.3 |.4 |.5 |.6 |.7 |.8 |.9 |1. |
    // .0   |   |   |   |   |   |   |1. |   |   |   |.4 |
    //                              /                   |
    //                            /                     |
    //                          /                       |
    //                  left  /                         | right
    //                      /                           |
    //                    /                             |
    //                  /                               |
    // .1   |   |   |.5 |   |   |   |   |   |   |   |.05|
    // 近
    
    const double p[2] = { .6 - .4 * zf, 1. };
    const double r[2] = { 1. - .5 * zf, .4 - .35 * zf };
    const double result = r[0] + fmax(pf - p[0], 0.) / (p[1] - p[0]) * (r[1] - r[0]);
    
    static auto shrink = [] (mbgl::vec3& near, mbgl::vec3& far, float shrink) {
        far[0] = near[0] + (far[0] - near[0]) * shrink; // x
        far[1] = near[1] + (far[1] - near[1]) * shrink; // y
    };

    enum { tl = 0, tr = 1, br = 2, bl = 3, };
    shrink(projection[bl], projection[tl], result);
    shrink(projection[br], projection[tr], result);
    
//    nav::log::i("sunlight", "zoom(%lf) pitch(%lf) | z(%lf) p(%lf) r(%lf)\n", state.getZoom(), state.getPitch(), zf, pf, result);
}

void Frumstum::update(const mbgl::TransformState& state, const std::vector<mbgl::OverscaledTileID>& tileIDs) {
    if (tileIDs.size() == 0) return;
    
    const auto& lightViewMatrix = state.getSunlightWorldToViewMatrix();
    
    // 所有tile在光照空间的AABB
    {
        tileAABB.invalidate();
        
        // 计算最小外接
        for (const auto& tile : tileIDs) {
            mbgl::mat4 modelMatrix;
            state.matrixFor(modelMatrix, tile.toUnwrapped());

            mbgl::mat4 mvMatrix;
            mbgl::matrix::multiply(mvMatrix, lightViewMatrix, modelMatrix);
            
            // model pos
            const auto h = maxheight::get<IN_WORLD>(tile.canonical.z);
            mbgl::vec4 envelope[8] = {
                { 0, 0, 0, 1 },
                { 0, mbgl::util::EXTENT, 0, 1 },
                { mbgl::util::EXTENT, 0, 0, 1 },
                { mbgl::util::EXTENT, mbgl::util::EXTENT, 0, 1 },
                
                { 0, 0, h, 1 },
                { 0, mbgl::util::EXTENT, h, 1 },
                { mbgl::util::EXTENT, 0, h, 1 },
                { mbgl::util::EXTENT, mbgl::util::EXTENT, h, 1 },
            };
            
            // to world space to light view space
            for (int i=0; i<8; i++) {
                auto& v = envelope[i];
                mbgl::matrix::transformMat4(v, v, mvMatrix);
                tileAABB.include({ v[0], v[1], -v[2] });
            }
        }

        print("tile", tileAABB);
    }
    
    // 主相机地面投影在光照空间的AABB
    {
        groundAABB.invalidate();
        
        //            camera
        //              A                                far
        //            near                 tl [0] ================== [1] tr
        //            /  |                        \                /
        //           /   |                       0 \______________/ 1
        //          /    |                          \            /
        //         /     |                           \__________/
        //     ___/======|_______              bl [3]            [2] br
        //  top  /       |  bottom                       near
        //          far
        
        enum {  near_tl = 0,    near_tr = 1,    near_br = 2,    near_bl = 3,
                far_tl = 4,     far_tr = 5,     far_br = 6,     far_bl = 7,     };
        
        // 构造主相机视锥体
        const auto worldSize = mbgl::Projection::worldSize(state.getScale());
        const auto flippedY = state.getViewportMode() == mbgl::ViewportMode::FlippedY;
        const auto frustum = mbgl::util::Frustum::fromInvProjMatrix(state.getInvProjectionMatrix(), worldSize, state.getZoom(), flippedY);
        
        // 视锥体顶点的世界坐标 world pos
        auto points = frustum.getPoints();
        for (auto& p : points) p = mbgl::vec3Scale(p, mbgl::util::tileSize);
        const auto debug_camera_pos = state.getCameraPosition();

        // 视锥体与地面投影
        const Plane ground = { { 0, 0, 1 }, 0 };
        std::array<mbgl::vec3, 4> projection = {
            getIntersect(ground, { points[near_tl], points[far_tl] }),
            getIntersect(ground, { points[near_tr], points[far_tr] }),
            getIntersect(ground, { points[near_br], points[far_br] }),
            getIntersect(ground, { points[near_bl], points[far_bl] }),
        };

        makeABetterFieldOfShadow(state, projection);
        
        // 计算最小外接
        const double h = maxheight::get<IN_PIXEL>(state.getZoom());
        for (int i=0; i<4; i++) {
            const auto& p = projection[i];
            
            mbgl::vec4 top;
            mbgl::matrix::transformMat4(top, { p[0], p[1], h, 1 }, lightViewMatrix);
            groundAABB.include({ top[0], top[1], -top[2] });
            
            mbgl::vec4 bottom;
            mbgl::matrix::transformMat4(bottom, { p[0], p[1], 0, 1 }, lightViewMatrix);
            groundAABB.include({ bottom[0], bottom[1], -bottom[2] });
            lightViewSpaceGround[i] = { bottom[0], bottom[1], -bottom[2] };
        }
        
        print("ground", groundAABB);
    }
    
    // 求tileAABB和groundAABB交集，得frustumAABB
    {
        frustumAABB = tileAABB.intersect(groundAABB);
        print("frustum", frustumAABB);
    }
    
    assert(tileAABB.valid());
    assert(groundAABB.valid());
    assert(frustumAABB.valid());
    
}

GLuint program() {
    static GLint pass = createProgram(compileShader(GL_VERTEX_SHADER, nav::programs::frustum::vertexShader()),
                                      compileShader(GL_FRAGMENT_SHADER, nav::programs::frustum::fragmentShader()));
    return pass;
}

void Frumstum::renderArea(const mbgl::mat4& lightProjMatrix) {
    const GLint program = ortho::program();
    glUseProgram(program);
    
    auto draw = [&] (const std::array<mbgl::vec3, 4>& points, const mbgl::vec4& color) {
        
        float vertices[] = {
            (float) points[0][0], (float) points[0][1], (float) points[0][2],
            (float) points[3][0], (float) points[3][1], (float) points[3][2],
            (float) points[1][0], (float) points[1][1], (float) points[1][2],
            (float) points[2][0], (float) points[2][1], (float) points[2][2],
        };

        
        std::array<mbgl::vec4, 4> ps;
        mbgl::matrix::transformMat4(ps[0], { points[0][0], points[0][1], points[0][2], 1 }, lightProjMatrix);
        mbgl::matrix::transformMat4(ps[1], { points[1][0], points[1][1], points[1][2], 1 }, lightProjMatrix);
        mbgl::matrix::transformMat4(ps[2], { points[2][0], points[2][1], points[2][2], 1 }, lightProjMatrix);
        mbgl::matrix::transformMat4(ps[3], { points[3][0], points[3][1], points[3][2], 1 }, lightProjMatrix);
        print("ps", ps);
        
        float vertices_debug[] = {
            (float) ps[0][0], (float) ps[0][1], (float) ps[0][2],
            (float) ps[3][0], (float) ps[3][1], (float) ps[3][2],
            (float) ps[1][0], (float) ps[1][1], (float) ps[1][2],
            (float) ps[2][0], (float) ps[2][1], (float) ps[2][2],
        };
        
        
        GLuint vao = 0;
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        GLuint vbo = 0;
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_debug), vertices_debug, GL_STATIC_DRAW);
        
        {
            static programs::UniformLocation u0(program, "u_matrix");
            glUniformMatrix4fv(u0, 1, GL_FALSE, reinterpret_cast<const float*>(&lightProjMatrix));
            
            static programs::UniformLocation u1(program, "u_color");
            glUniform4f(u1, color[0], color[1], color[2], color[3]);
            
            static programs::AttribLocation a0(program, "a_pos");
            glEnableVertexAttribArray(a0);
            glVertexAttribPointer(a0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), reinterpret_cast<void*>(0));

            glDisable(GL_CULL_FACE);
            glBindVertexArray(vao);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
            glBindVertexArray(0);
            glEnable(GL_CULL_FACE);
        }

        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &vbo);
    };

    draw(lightViewSpaceGround, { 0, 1, 0, .2 });
    print("ground_d", lightViewSpaceGround);
}

Frumstum& sunlight() {
    static Frumstum frustum;
    return frustum;
}

}   // end ortho

}   // end frustum

}   // end shadow

}   // end render

}   // end nav



