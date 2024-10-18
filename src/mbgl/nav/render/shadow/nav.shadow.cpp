//
//  nav.shadow.cpp
//
//  Created by ZHM on 2024/10/11.
//

#include "mbgl/nav/render/shadow/nav.shadow.hpp"

#include <random>

#include "mbgl/nav/render/shaders.h"
#include "mbgl/nav/render/vec3.h"

#include <mbgl/util/mat4.hpp>

#include <mbgl/programs/gl/nav.ssao.shader.hpp>


#include "mbgl/nav/render/ssao/v1/rply.h"

#include "mbgl/nav/nav.style.hpp"
#include "mbgl/nav/nav.palette.hpp"


#define SCR_WIDTH nav::display::pixels::width()
#define SCR_HEIGHT nav::display::pixels::height()


namespace nav {

namespace shadow {

// the camera info
Vec3 eye = Vec3(0.25, 1.5, 0.25);
Vec3 lookat = Vec3(0, 0, 0);
Vec3 light_eye = Vec3(-0.75, 2.5, 0.75);

Mat4 l_mvp;
void genrateLightMvp() {

    // set lighting uniform
    const static double pi = acos(0.0) * 2;
    Mat4 ident = Mat4::identityMatrix();
    Mat4 model = Mat4(1.0);
    Mat4 view, viewNorm;
    Mat4::lookAtMatrix(light_eye, lookat, Vec3(0, 1, 0), view, viewNorm);
    Mat4 proj = Mat4::perspectiveMatrix(pi * 0.5, 1.333333f, 1.0f, 7.5f);
    Mat4 mvp = proj * view;
    l_mvp = mvp;
    
}

namespace depth {

GLuint fbo;
GLuint buffer;

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
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, buffer, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}

}

void renderDBuffer(std::function<void()> renderCallback,
                   std::function<void()> bindScreenFbo) {
    if (bindScreenFbo) {
        bindScreenFbo();
    } else {
        glBindFramebuffer(GL_FRAMEBUFFER, depth::fbo);
        glClear(GL_DEPTH_BUFFER_BIT);
    }
    
    GLboolean enabledCullface;
    glGetBooleanv(GL_CULL_FACE, &enabledCullface);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    
    renderCallback();
    
    glCullFace(GL_BACK);
    enabledCullface ? glEnable(GL_CULL_FACE) : glDisable(GL_CULL_FACE);
}

GLuint renderShadowDepthBuffer(int width, int height, std::function<bool()> renderCallback, std::function<void()> bindScreenFbo) {
    depth::generate(width, height);
    renderDBuffer(renderCallback, bindScreenFbo);
    return depth::buffer;
}

}   // end shadow

}   // end nav



