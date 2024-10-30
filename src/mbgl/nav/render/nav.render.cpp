//
//  nav.render.cpp
//  Mapbox GL Native
//
//  Created by BernieZhao on 2024/10/10.
//

#include "mbgl/nav/render/nav.render.hpp"

#include "mbgl/nav/nav.style.hpp"

#include "mbgl/nav/render/nav.blur.hpp"
#include "mbgl/nav/render/shaders.h"
#include "mbgl/nav/render/shadow/nav.shadow.hpp"
#include "mbgl/nav/render/ssao/nav.ssao.hpp"

#include <mbgl/programs/gl/nav.ssao.shader.hpp>
#include <mbgl/programs/fill_extrusion_ssao_program.hpp>
#include <mbgl/programs/fill_extrusion_shadow_program.hpp>

#include "mbgl/nav/render/vec3.h"
#include "mbgl/nav/render/mat4.h"

#include <mbgl/util/vectors.hpp>
#include <mbgl/util/mat4.hpp>
#include <mbgl/util/bounding_volumes.hpp>

#include <limits>


namespace nav {

static auto convertVec3 = [] (mbgl::vec3 v) {
    return Vec3((float) v[0], (float) v[1], (float) v[2]);
};

static auto convertMatrix4 = [] (mbgl::mat4 matrix) {
    Mat4 m;
    for (int i=0; i<16; i++) ((float*)&m)[i] = float(matrix[i]);
    return m;
};


namespace renderer {

GLint depthBuffer = 0;

namespace ground {

// Set up buffer for floor data, 6 triangles
const static GLfloat vertices[36] = {
    0.0f, 8192.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    8192.0f, 8192.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    
    8192.0f, 8192.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    8192.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f };

GLuint program() {
    static GLint pass = 0;
    if (!pass) {
        pass =
        createProgram(compileShader(GL_VERTEX_SHADER, mbgl::floorVertexShader()),
                      compileShader(GL_FRAGMENT_SHADER, mbgl::floorFragmentShader()));
    }
    
    return pass;
}

GLuint vao(GLuint program) {
    static GLuint vao = 0;
    if (!vao) {
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        
        static GLuint vbo;
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, 36 * sizeof(GLfloat), vertices, GL_STATIC_DRAW);
        
        static programs::AttribLocation a0(program, "a_pos");
        glEnableVertexAttribArray(a0);
        glVertexAttribPointer(a0, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void*>(0));
        
        static programs::AttribLocation a1(program, "a_normal_ed");
        glEnableVertexAttribArray(a1);
        glVertexAttribPointer(a1, 4, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void*>(2 * sizeof(GLfloat)));
        
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    
    return vao;
}

void render(const mbgl::mat4& mvp, const mbgl::mat4& mv, const mbgl::mat4& normal, const mbgl::mat4& lightmvp) {
    GLboolean cullfaceEnabled;
    glGetBooleanv(GL_CULL_FACE, &cullfaceEnabled);
    
    const GLint program = ground::program();
    glUseProgram(program);
    glDisable(GL_CULL_FACE); // for render ground
    
    {
        static programs::UniformLocation u0(program, "u_matrix");
        const Mat4 MVP = convertMatrix4(mvp);
        glUniformMatrix4fv(u0, 1, GL_FALSE, reinterpret_cast<const float*>(&MVP));
        
        static programs::UniformLocation u1(program, "u_model_view_matrix");
        const Mat4 MV = convertMatrix4(mv);
        glUniformMatrix4fv(u1, 1, GL_FALSE, reinterpret_cast<const float*>(&MV));
        
        static programs::UniformLocation u2(program, "u_normal_matrix");
        const Mat4 NORMAL = convertMatrix4(normal);
        glUniformMatrix4fv(u2, 1, GL_FALSE, reinterpret_cast<const float*>(&NORMAL));
        
        static programs::UniformLocation u3(program, "u_light_matrix");
        const Mat4 LIGHTMVP = convertMatrix4(lightmvp);
        glUniformMatrix4fv(u3, 1, GL_FALSE, reinterpret_cast<const float*>(&LIGHTMVP));
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, depthBuffer);
        static programs::UniformLocation u4(program, "u_shadow_map");
        glUniform1i(u4, 0);
    }
    
    glBindVertexArray(ground::vao(program));
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
    
    cullfaceEnabled ? glEnable(GL_CULL_FACE) : glDisable(GL_CULL_FACE);
}

} // ground


namespace deferred {

const float BUFFER_RATIO = 1.;

int width() {
    return nav::display::pixels::width() * BUFFER_RATIO;
}

int height() {
    return nav::display::pixels::height() * BUFFER_RATIO;
}

void render(float zoom, mbgl::mat4 projMatrix,
            std::function<bool()> shadowRenderDelegate,
            std::function<bool()> geoRenderDelegate) {
    
    if (zoom < 15.) return;
    
    GLfloat clearColor[4];
    glGetFloatv(GL_COLOR_CLEAR_VALUE, clearColor);
    
    GLboolean blendEnabled;
    glGetBooleanv(GL_BLEND, &blendEnabled);
    
    {
        GLint viewport[4];
        glGetIntegerv(GL_VIEWPORT, viewport);
        auto bindScreen = [viewport] () {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
            
        };
        
        const int w = width();
        const int h = height();
        glViewport(0, 0, w, h);
        
        // 1
        depthBuffer = nav::shadow::depth::render(w, h, shadowRenderDelegate);
        
        // 2
        nav::geo::renderGeoAndShadow(w, h, depthBuffer, geoRenderDelegate);
        
        // 3
        const GLint shadowAndAO = nav::ssao::render(w, h, zoom, convertMatrix4(projMatrix));
        
        // 4
        nav::blur::render(w, h, shadowAndAO, true, bindScreen);
        
        // debug info window
        if (1) {
            int x = 20;
            int y = 20;
            int ww = w / 6.;
            int hh = h / 6.;
            
            nav::blur::render(w, h, depthBuffer, false, [x, y, ww, hh] () {
                glBindFramebuffer(GL_FRAMEBUFFER, 0);
                glViewport(x, y, ww, hh);
            });
            
            x += ww + 20;
            
            nav::blur::render(w, h, shadowAndAO, false, [x, y, ww, hh] () {
                glBindFramebuffer(GL_FRAMEBUFFER, 0);
                glViewport(x, y, ww, hh);
            });
            
            glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
        }
        
    }
    
    glClearColor(clearColor[0], clearColor[1], clearColor[2], clearColor[3]);
    blendEnabled ? glEnable(GL_BLEND) : glDisable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA); // mapbox config
    
}

} // deffered


namespace util {

GLuint genTexture(GLint internalformat, GLsizei width, GLsizei height, GLenum format, GLenum type) {
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, internalformat, width, height, 0, format, type, NULL);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    return texture;
}

void renderQuad(GLint program) {
    static GLuint quadVAO = 0;
    if (!quadVAO) {
        float quadVertices[] = {
            // positions            // texture Coords
            -1.0f,  1.0f, 0.0f,     0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f,     0.0f, 0.0f,
            1.0f,  1.0f, 0.0f,     1.0f, 1.0f,
            1.0f, -1.0f, 0.0f,     1.0f, 0.0f,
        };
        
        static GLuint quadVBO;
        
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        
        static programs::AttribLocation a0(program, "aPos");
        glEnableVertexAttribArray(a0);
        glVertexAttribPointer(a0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        
        static programs::AttribLocation a1(program, "aTexCoords");
        glEnableVertexAttribArray(a1);
        glVertexAttribPointer(a1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

} // util

} // renderer


namespace sunlight {

AABB frustumAABB;

const AABB& getFrustum() {
    return frustumAABB;
}

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

template <int MIN>
float height(float baseZ, float zoom) {
    return baseZ * pow(2, fmax(0, zoom - MIN));
}

void updateFrustum(const mbgl::TransformState& state, const std::vector<mbgl::OverscaledTileID>& tileIDs) {
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

} // sunlight

} // nav
