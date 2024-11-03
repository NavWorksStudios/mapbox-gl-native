//
//  nav.quad.cpp
//  Mapbox GL Native
//
//  Created by BernieZhao on 2024/10/10.
//

#include "mbgl/nav/render/nav.quad.hpp"

#include "mbgl/nav/render/shaders.h"
#include "mbgl/nav/render/nav.render.hpp"
#include "mbgl/nav/render/programs/nav.program.quad.hpp"

namespace nav {
namespace quad {

void render(GLint program) {
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


template <const char*(*Frag)()>
GLint getProgram() {
    static GLint program = 0;
    if (!program) {
        GLuint vs = compileShader(GL_VERTEX_SHADER, nav::programs::quad::vertexShader());
        GLuint fs = compileShader(GL_FRAGMENT_SHADER, Frag());
        program = createProgram(vs, fs);
    }
    return program;
}

void doRender(GLint program, int width, int height, GLint buffer, std::function<void()> bindScreen) {
    
    if (bindScreen) bindScreen();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glUseProgram(program);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, buffer);
    static programs::UniformLocation u0(program, "u_buffer");
    glUniform1i(u0, 0);

    render(program);
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void render(int width, int height, GLint buffer, std::function<void()> bindScreen) {
    GLint program = getProgram<nav::programs::quad::standardFragmentShader>();
    doRender(program, width, height, buffer, bindScreen);
}

void renderMono(int width, int height, GLint buffer, std::function<void()> bindScreen) {
    GLint program = getProgram<nav::programs::quad::monoFragmentShader>();
    doRender(program, width, height, buffer, bindScreen);
}

void renderBlur(int width, int height, GLint buffer, std::function<void()> bindScreen) {
    GLint program = getProgram<nav::programs::quad::blurFragmentShader>();
    
    glUseProgram(program);
    static programs::UniformLocation u2(program, "u_offset");
    glUniform2f(u2, 1.2 / width, 1.2 / height);

    doRender(program, width, height, buffer, bindScreen);
}

}
}
