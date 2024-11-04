//
//  nav.geo.cpp
//
//  Created by BernieZhao on 2024/10/31.
//

#include "mbgl/nav/render/nav.geo.hpp"

#include "mbgl/nav/render/vec3.h"
#include "mbgl/nav/render/shaders.h"
#include "mbgl/nav/render/nav.shadow.hpp"
#include "mbgl/nav/render/programs/nav.program.ssao.hpp"

#include <random>


namespace nav {

GLuint genTexture(GLint internalformat, GLsizei width, GLsizei height, GLenum format, GLenum type);

namespace geo {

GLuint fbo = 0;

GLuint position = 0;
GLuint normal = 0;
GLuint albedo = 0;

GLuint rboDepth = 0;

GLuint shadow = 0;

void initResource(int width, int height) {
    static int w = 0, h = 0;
    if (w != width || h != height) {
        w = width;
        h = height;

        if (!fbo) glGenFramebuffers(1, &fbo);

        // position color buffer
        glDeleteTextures(1, &position);
        position = genTexture(GL_RGB16F, width, height, GL_RGB, GL_FLOAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        // normal color buffer
        glDeleteTextures(1, &normal);
        normal = genTexture(GL_RGB16F, width, height, GL_RGB, GL_FLOAT);

        // color + specular color buffer
        glDeleteTextures(1, &albedo);
        albedo = genTexture(GL_RGB16F, width, height, GL_RGB, GL_FLOAT);

        // create and attach depth buffer (renderbuffer)
        glDeleteRenderbuffers(1, &rboDepth);
        glGenRenderbuffers(1, &rboDepth);
        glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    }
}

void bindFbo(GLuint shadow) {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    if (geo::shadow != shadow) {
        geo::shadow = shadow;

        // color attachment
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, position, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, normal, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, albedo, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, shadow, 0);
        
        GLenum attachments[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
        glDrawBuffers(4, attachments);
        
        // depth attachment
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
    }

}

static GLint delegateProgram = 0;

void setCurrentProgram() {
    if (!delegateProgram) {
        glGetIntegerv(GL_CURRENT_PROGRAM, &delegateProgram);
    }
}

GBuffer renderGeoAndShadow(int width, int height, GLuint shadow, GLuint shadowDepth,
                           std::function<void()> renderDelegate, std::function<void()> bindScreen) {
    initResource(width, height);

    if (bindScreen) {
        bindScreen();
    } else {
        geo::bindFbo(shadow);
    }

    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // 清空所有颜色附件
    
    glDisable(GL_BLEND);

    if (delegateProgram) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, shadowDepth);
        static programs::UniformLocation u0(delegateProgram, "u_shadow_map");
        glUniform1i(u0, 0);

        static programs::UniformLocation u1(delegateProgram, "u_shadow_offset");
        glUniform2f(u1, .5 / nav::shadow::width, .5 / nav::shadow::height);
    }
    
    renderDelegate();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    return { position, normal, albedo };
}

}

}

