//
//  nav.render.cpp
//  Mapbox GL Native
//
//  Created by BernieZhao on 2024/10/10.
//

#include "mbgl/nav/nav.render.hpp"

#include "mbgl/nav/nav.blur.hpp"
#include "mbgl/nav/shadow/nav.shadow.hpp"
#include "mbgl/nav/ssao/v2/nav.ssao.hpp"

#include "mbgl/nav/nav.style.hpp"

#include "mbgl/nav/ssao/v1/shaders.h"
#include <mbgl/programs/gl/nav_ssao.hpp>

#include <mbgl/programs/fill_extrusion_ssao_program.hpp>


namespace nav {
namespace render {


static auto convertMatrix = [] (mbgl::mat4 matrix) {
    Mat4 m;
    for (int i=0; i<16; i++) ((float*)&m)[i] = float(matrix[i]);
    return m;
};


void deferred(float zoom, mbgl::mat4 projMatrix, std::function<void()> renderCallback) {
    
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);

    const float BUFFER_SCALE = .7;
    const int w = nav::display::pixels::width() * BUFFER_SCALE;
    const int h = nav::display::pixels::height() * BUFFER_SCALE;
    
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    glViewport(0, 0, w, h);
    
    nav::ssao::initResource(w, h);
    
    const GLint shadowDepth = nav::shadow::renderDepthBuffer(w, h);

    nav::ssao::renderGeoAndShadowBuffer(shadowDepth, renderCallback);

    const GLint renderBuffer = nav::ssao::renderAOBuffer(w, h, zoom, convertMatrix(projMatrix));
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
    
    nav::blur::render(renderBuffer, w, h);

}


namespace floor {

GLuint program() {
    static GLint pass = 0;
    if (!pass) {
        pass =
        createProgram(compileShader(GL_VERTEX_SHADER, mbgl::vertexShader()),
                      compileShader(GL_FRAGMENT_SHADER, mbgl::fragmentShader()));
    }

    return pass;
}

GLuint data() {
    static GLuint tileFloorBuf = 0;
    if (!tileFloorBuf) {
        // Set up buffer for floor data, 6 triangles
        const static GLfloat tileFloorData[36] = {
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
        
        glGenBuffers(1, &tileFloorBuf);
        glBindBuffer(GL_ARRAY_BUFFER, tileFloorBuf);
        glBufferData(GL_ARRAY_BUFFER, 36 * sizeof(GLfloat), tileFloorData, GL_STATIC_DRAW);
    }

    return tileFloorBuf;
}

}


void renderTileFloor(const mbgl::mat4& mvp, const mbgl::mat4& mv, const mbgl::mat4& normal) {
    const GLint program = floor::program();

    glUseProgram(program);
    
    // convert 3*matrix mbgl::mat4 to Mat4
    const Mat4 MVP = convertMatrix(mvp);
    const Mat4 MV = convertMatrix(mv);
    const Mat4 NORMAL = convertMatrix(normal);
    
    // floor uniforms
    {
        static programs::UniformLocation u0(program, "u_matrix");
        glUniformMatrix4fv(u0, 1, GL_FALSE, reinterpret_cast<const float*>(&MVP));
        
        static programs::UniformLocation u1(program, "u_model_view_matrix");
        glUniformMatrix4fv(u1, 1, GL_FALSE, reinterpret_cast<const float*>(&MV));
        
        static programs::UniformLocation u2(program, "u_normal_matrix");
        glUniformMatrix4fv(u2, 1, GL_FALSE, reinterpret_cast<const float*>(&NORMAL));
    }

    
    // floor attributes
    {
        glBindBuffer(GL_ARRAY_BUFFER, floor::data());
        
        static programs::AttribLocation a0(program, "a_pos");
        glEnableVertexAttribArray(a0);
        glVertexAttribPointer(a0, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void*>(0));
        
        static programs::AttribLocation a1(program, "a_normal_ed");
        glEnableVertexAttribArray(a1);
        glVertexAttribPointer(a1, 4, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void*>(2*sizeof(GLfloat)));
    }

    glDrawArrays(GL_TRIANGLES, 0, 6);
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



}
}
