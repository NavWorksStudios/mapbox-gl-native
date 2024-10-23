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


namespace shadow {

GLuint program() {
    static GLint pass = 0;
    if (!pass) {
        pass =
        createProgram(compileShader(GL_VERTEX_SHADER, mbgl::floorShadowVertexShader()),
                      compileShader(GL_FRAGMENT_SHADER, mbgl::floorShadowFragmentShader()));
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
        
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    
    return vao;
}

}

GLint shadowDepthBuffer;

}


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
        glBindTexture(GL_TEXTURE_2D, floor::shadowDepthBuffer);
        static programs::UniformLocation u4(program, "u_shadow_map");
        glUniform1i(u4, 0);
    }

    glBindVertexArray(floor::ssao::vao(program));
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
    
    cullfaceEnabled ? glEnable(GL_CULL_FACE) : glDisable(GL_CULL_FACE);
}

void renderTileFloor(const mbgl::mat4& lightmvp) {
    GLboolean cullfaceEnabled;
    glGetBooleanv(GL_CULL_FACE, &cullfaceEnabled);
    
    const GLint program = floor::shadow::program();
    glUseProgram(program);
    glDisable(GL_CULL_FACE); // for render ground

    {
        static programs::UniformLocation u0(program, "u_matrix");
        const Mat4 LIGHTMVP = convertMatrix4(lightmvp);
        glUniformMatrix4fv(u0, 1, GL_FALSE, reinterpret_cast<const float*>(&LIGHTMVP));
    }

    glBindVertexArray(floor::shadow::vao(program));
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
        floor::shadowDepthBuffer = nav::shadow::depth::render(w, h, shadowRenderDelegate);

        // 2
        nav::geo::renderGeoBufferAndShadowBuffer(w, h, floor::shadowDepthBuffer, geoRenderDelegate);

        // 3
        const GLint renderBuffer = nav::ssao::render(w, h, zoom, convertMatrix4(projMatrix));

        // 4
        bindScreen();
        nav::blur::render(w, h, renderBuffer);
        
#if 1
        nav::shadow::depth::render(w, h, shadowRenderDelegate, [w, h] () {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glViewport(50, 50, w / 3., h / 3.);
        });
        bindScreen();
#endif

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

std::array<float, 6> envelope;

const std::array<float, 6>& getEnvelope() {
    return envelope;
}

void updateEnvelope(const mbgl::TransformState& state, const std::vector<mbgl::OverscaledTileID>& tileIDs) {
    envelope = {
        std::numeric_limits<float>::max(),
        std::numeric_limits<float>::lowest(),
        std::numeric_limits<float>::max(),
        std::numeric_limits<float>::lowest(),
        std::numeric_limits<float>::max(),
        std::numeric_limits<float>::lowest() };
    
    const auto& lightSpaceMatrix = state.getWorldToSunlightMatrix();
    
    // model pos
    const mbgl::vec4 vertices[4] = {
        { 0, 0, 0, 1 },
        { 0, mbgl::util::EXTENT, 0, 1 },
        { mbgl::util::EXTENT, 0, 0, 1 },
        { mbgl::util::EXTENT, mbgl::util::EXTENT, 0, 1 },
    };
    
    printf("light ++++++++++++++++++++++++++++++\n");
    
    for (const auto& tile : tileIDs) {
        mbgl::mat4 matrix;
        state.matrixFor(matrix, tile.toUnwrapped());
        
//        printf("light ++++\n");
        
        for (int i=0; i<4; i++) {
            mbgl::vec4 pos = vertices[i];
            mbgl::matrix::transformMat4(pos, pos, matrix); // to world pos
            mbgl::matrix::transformMat4(pos, pos, lightSpaceMatrix); // to light space pos
            
            pos[0] = -pos[0];
            pos[1] = -pos[1];
            pos[2] = -pos[2];

            // envelope box
            const float x = pos[0];
            const float y = pos[1];
            const float z = pos[2];
            
            envelope[0] = fmin(envelope[0], x);
            envelope[1] = fmax(envelope[1], x);
            envelope[2] = fmin(envelope[2], y);
            envelope[3] = fmax(envelope[3], y);
            envelope[4] = fmin(envelope[4], z);
            envelope[5] = fmax(envelope[5], z);
            
//            printf("light (%f,%f,%f) -> (%f,%f,%f,%f,%f,%f) \n", x, y, z,
//                   envelope[0], envelope[1], envelope[2], envelope[3], envelope[4], envelope[5]);
        }
        
//        printf("light ----\n");
    }
    
    printf("light (%f,%f,%f, \n", envelope[0], envelope[1], envelope[2]);
    printf("light %f,%f,%f) \n", envelope[3], envelope[4], envelope[5]);
    printf("light -----------------------------\n");

}

}



}
}
