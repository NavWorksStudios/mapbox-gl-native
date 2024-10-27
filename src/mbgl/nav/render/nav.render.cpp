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

#include <mbgl/util/mat4.hpp>
#include <mbgl/util/bounding_volumes.hpp>

#include <limits>


namespace nav {
namespace render {


static auto convertVec3 = [] (mbgl::vec3 v) {
    return Vec3((float) v[0], (float) v[1], (float) v[2]);
};

static auto convertMatrix4 = [] (mbgl::mat4 matrix) {
    Mat4 m;
    for (int i=0; i<16; i++) ((float*)&m)[i] = float(matrix[i]);
    return m;
};


namespace floor {

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

namespace ssao {

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

}

}

GLint depthBuffer = 0;

void renderTileFloor(const mbgl::mat4& mvp, const mbgl::mat4& mv, const mbgl::mat4& normal, const mbgl::mat4& lightmvp) {
    GLboolean cullfaceEnabled;
    glGetBooleanv(GL_CULL_FACE, &cullfaceEnabled);
    
    const GLint program = floor::ssao::program();
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

    glBindVertexArray(floor::ssao::vao(program));
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
    
    cullfaceEnabled ? glEnable(GL_CULL_FACE) : glDisable(GL_CULL_FACE);
}

const float BUFFER_RATIO = .7;

int width() {
    return nav::display::pixels::width() * BUFFER_RATIO;
}

int height() {
    return nav::display::pixels::height() * BUFFER_RATIO;
}

void deferred(float zoom,
              mbgl::mat4 projMatrix,
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

}

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


std::array<float, 6> envelope;

const std::array<float, 6>& getEnvelope() {
    return envelope;
}

const std::array<float, 4> EMPTY_REGION = {
    std::numeric_limits<float>::max(),
    -std::numeric_limits<float>::max(),
    std::numeric_limits<float>::max(),
    -std::numeric_limits<float>::max() };

void updateEnvelope(const mbgl::TransformState& state, const std::vector<mbgl::OverscaledTileID>& tileIDs) {
    if (tileIDs.size() == 0) return;
    
    std::array<float, 4> tilecover = EMPTY_REGION;
    {
        // model pos
        const mbgl::vec4 model[4] = {
            { 0, 0, 0, 1 },
            { 0, mbgl::util::EXTENT, 0, 1 },
            { mbgl::util::EXTENT, 0, 0, 1 },
            { mbgl::util::EXTENT, mbgl::util::EXTENT, 0, 1 },
        };

        for (const auto& tile : tileIDs) {
            mbgl::mat4 matrix;
            state.matrixFor(matrix, tile.toUnwrapped());

            for (int i=0; i<4; i++) {
                mbgl::vec4 pos = model[i];
                mbgl::matrix::transformMat4(pos, pos, matrix); // to world pos

                const float x = pos[0];
                tilecover[0] = fmin(tilecover[0], x);
                tilecover[1] = fmax(tilecover[1], x);
                
                const float y = pos[1];
                tilecover[2] = fmin(tilecover[2], y);
                tilecover[3] = fmax(tilecover[3], y);
            }
        }
        
        nav::log::i("updateEnvelope", "tilecover x(%6.1f,%6.1f) y(%6.1f,%6.1f)",
                    tilecover[0], tilecover[1], tilecover[2], tilecover[3]);
    }
    
    std::array<float, 4> projection = EMPTY_REGION;
    {
        const auto worldSize = mbgl::Projection::worldSize(state.getScale());
        const auto flippedY = state.getViewportMode() == mbgl::ViewportMode::FlippedY;
        const auto frustum = mbgl::util::Frustum::fromInvProjMatrix(state.getInvProjectionMatrix(), worldSize, state.getZoom(), flippedY);
        auto& points = frustum.getPoints();
        
        const Plane ground = { { 0, 0, 1 }, 0 };
        mbgl::optional<mbgl::vec3> intersection[4];
        intersection[0] = getIntersect(ground, { points[0], points[4] });
        intersection[1] = getIntersect(ground, { points[1], points[5] });
        intersection[2] = getIntersect(ground, { points[2], points[6] });
        intersection[3] = getIntersect(ground, { points[3], points[7] });
        
        for (int i=0; i<4; i++) {
            const mbgl::vec3& pos = *intersection[i];

            const float x = pos[0];
            projection[0] = fmin(projection[0], x);
            projection[1] = fmax(projection[1], x);
            
            const float y = pos[1];
            projection[2] = fmin(projection[2], y);
            projection[3] = fmax(projection[3], y);
        }

        nav::log::i("updateEnvelope", "projection x(%6.1f,%6.1f) y(%6.1f,%6.1f)",
                    projection[0], projection[1], projection[2], projection[3]);
    }
    
    {
//        const auto& lightSpaceMatrix = state.getWorldToSunlightMatrix();
//        mbgl::matrix::transformMat4(pos, pos, lightSpaceMatrix); // to light space pos
        
        envelope = {
            fmax(tilecover[0], projection[0]), fmin(tilecover[1], projection[1]),
            fmax(tilecover[2], projection[2]), fmin(tilecover[3], projection[3]),
            0, 0
        };
        
        nav::log::i("updateEnvelope", "envelope x(%6.1f,%6.1f) y(%6.1f,%6.1f) z(%6.1f,%6.1f)",
                    envelope[0], envelope[1], envelope[2], envelope[3], envelope[4], envelope[5]);
    }

}

}



}
}
