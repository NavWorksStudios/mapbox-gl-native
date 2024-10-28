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

const float BUFFER_RATIO = .7;

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
            int ww = w / 5.;
            int hh = h / 5.;
            
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

std::array<double, 6> frustumAABB;

const std::array<double, 6>& getFrustum() {
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

mbgl::optional<mbgl::vec3> getIntersect(const Plane& plane, const Linesegment& line) {
    const auto dir = line.direction();
    const auto& nor = plane.normal;
    
    double denominator = nor[0] * dir[0] + nor[1] * dir[1] + nor[2] * dir[2];
    if (denominator == 0) {
        // 线段与平面平行，无交点
        return mbgl::nullopt;
    }
    
    double t = -(nor[0] * line.from[0] + nor[1] * line.from[1] + nor[2] * line.from[2] + plane.distance) / denominator;
    if (t >= 0 && t <= 1) {
        mbgl::vec3 intersection;
        intersection[0] = line.from[0] + t * dir[0];
        intersection[1] = line.from[1] + t * dir[1];
        intersection[2] = line.from[2] + t * dir[2];
        return { intersection };
    } else {
        // 交点不在线段上
        return mbgl::nullopt;
    }
}


// AABB（Axis-Aligned Bounding Box）
// “轴平行包围盒”，是一种在三维空间中常用的几何包围体。
// 它是一个长方体，其边与坐标轴平行，这使得它在计算和存储上相对简单。
const double MAX = std::numeric_limits<double>::max();
struct AABB {
    mbgl::vec3 min = {  MAX, MAX, MAX, };
    mbgl::vec3 max = { -MAX, -MAX, -MAX, };
};

void updateFrustum(const mbgl::TransformState& state, const std::vector<mbgl::OverscaledTileID>& tileIDs) {
    if (tileIDs.size() == 0) return;
    
    const auto& viewMatrix = state.getSunlightWorldToViewMatrix();
    const double h = 30. * pow(2, fmax(0, state.getZoom() - 15));
    
    // (light space) 主相机可见楼块tile的AABB
    AABB tileAABB;
    {
        // model pos
        const mbgl::vec4 tileEnvelope[8] = {
            { 0, 0, 0, 1 },
            { 0, mbgl::util::EXTENT, 0, 1 },
            { mbgl::util::EXTENT, 0, 0, 1 },
            { mbgl::util::EXTENT, mbgl::util::EXTENT, 0, 1 },

            { 0, 0, h, 1 },
            { 0, mbgl::util::EXTENT, h, 1 },
            { mbgl::util::EXTENT, 0, h, 1 },
            { mbgl::util::EXTENT, mbgl::util::EXTENT, h, 1 },
        };
        
        // 计算最小外接
        for (const auto& tile : tileIDs) {
            mbgl::mat4 modelmatrix;
            state.matrixFor(modelmatrix, tile.toUnwrapped());
            
            for (int i=0; i<8; i++) {
                mbgl::vec4 pos = tileEnvelope[i];
                mbgl::matrix::transformMat4(pos, pos, modelmatrix); // to world space
                mbgl::matrix::transformMat4(pos, pos, viewMatrix); // to view space
                
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
    
    // (light space) 主相机可见地面的最小外接矩形
    AABB groundAABB;
    {
        // 相机视锥体与地面交点
        const auto worldSize = mbgl::Projection::worldSize(state.getScale()) / mbgl::util::tileSize;
        const auto flippedY = state.getViewportMode() == mbgl::ViewportMode::FlippedY;
        const auto frustum = mbgl::util::Frustum::fromInvProjMatrix(state.getInvProjectionMatrix(), worldSize, state.getZoom(), flippedY);

//            camera
//              A
//            near
//            /  |
//           /   |
//          /    |
//         /     |
//     ___/______|_______
//  top  /       |  bottom
//          far

        auto& points = frustum.getPoints();
        const Plane ground = { { 0, 0, 1 }, 0 };
        mbgl::optional<mbgl::vec3> projection[4];
        // far
        projection[0] = getIntersect(ground, { points[0], points[4] }); // near-tl, far-tl
        projection[1] = getIntersect(ground, { points[1], points[5] }); // near-tr, far-tr
        // near
        projection[2] = getIntersect(ground, { points[2], points[6] }); // near-bl, far-bl
        projection[3] = getIntersect(ground, { points[3], points[7] }); // near-br, far-br
        
        {
            // zoom大时，减小平视距离
            // p(1),z(20)=1        p(1),z(15)=1
            // p(0),z(20)=0.05     p(0),z(15)=1
            const double p = fmax(.05, 1. - sin(state.getPitch())); // 俯视1，平视0
            const double z = 1. - pow(fmin(1., (state.getZoom() - 15.) / 5.), 0.1); // 15等于1，17等于0.2, 20等于0
            const double r = fmin(1., p + z);

            auto& p0 = *projection[0];
            auto& p2 = *projection[2];
            p0[0] = p2[0] + (p0[0] - p2[0]) * r;
            p0[1] = p2[1] + (p0[1] - p2[1]) * r;
            
            auto& p1 = *projection[1];
            auto& p3 = *projection[3];
            p1[0] = p3[0] + (p1[0] - p3[0]) * r;
            p1[1] = p3[1] + (p1[1] - p3[1]) * r;
        }
        
        // 计算最小外接
        for (int i=0; i<4; i++) {
            assert(projection[i].has_value());
            const auto& p = *projection[i];
            mbgl::vec4 pos[2] = {
                { p[0], p[1], 0, 1 },
                { p[0], p[1], h, 1 },
            };
            
            mbgl::matrix::transformMat4(pos[0], pos[0], viewMatrix); // to view space
            mbgl::matrix::transformMat4(pos[1], pos[1], viewMatrix); // to view space
            
            for (int i=0; i<2; i++) {
                const double x = pos[i][0];
                const double y = pos[i][1];
                const double z = -pos[i][2];
                
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
    
    // (light space) 求tileAABB和groundAABB交集，得frustumAABB
    {
        const mbgl::vec3 min = {
            fmax(tileAABB.min[0], groundAABB.min[0]),
            fmax(tileAABB.min[1], groundAABB.min[1]),
            fmax(tileAABB.min[2], groundAABB.min[2]),
        };
        
        const mbgl::vec3 max = {
            fmin(tileAABB.max[0], groundAABB.max[0]),
            fmin(tileAABB.max[1], groundAABB.max[1]),
            fmin(tileAABB.max[2], groundAABB.max[2]),
        };
        
        frustumAABB = {
            fmin(min[0], max[0]), fmax(min[0], max[0]),       // minX, maxX
            fmin(min[1], max[1]), fmax(min[1], max[1]),       // minY, maxY
            fmin(min[2], max[2]), fmax(min[2], max[2]),       // minZ, maxZ
        };
        
        nav::log::i("sunlight", "frus min(%8.2lf, %8.2lf, %8.2lf) max(%8.2lf, %8.2lf, %8.2lf)",
                    frustumAABB[0], frustumAABB[2], frustumAABB[4],
                    frustumAABB[1], frustumAABB[3], frustumAABB[5]);
        
//        frustumAABB = {
//            tileAABB.min[0], tileAABB.max[0],
//            tileAABB.min[1], tileAABB.max[1],
//            tileAABB.min[2], tileAABB.max[2],
//        };

//        frustumAABB = {
//            groundAABB.min[0], groundAABB.max[0],
//            groundAABB.min[1], groundAABB.max[1],
//            groundAABB.min[2], groundAABB.max[2],
//        };
    }
    
}

} // sunlight

} // nav
