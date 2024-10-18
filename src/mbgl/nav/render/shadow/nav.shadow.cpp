//
//  nav.shadow.cpp
//
//  Created by ZHM on 2024/10/11.
//

#include "mbgl/nav/render/shadow/nav.shadow.hpp"


namespace nav {

namespace shadow {

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

GLuint render(int width, int height, std::function<bool()> renderCallback, std::function<void()> bindScreenFbo) {
    depth::generate(depth::width, depth::height);
    
    if (bindScreenFbo) {
        bindScreenFbo();
    } else {
        glViewport(0, 0, depth::width, depth::height);
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
    
    if (!bindScreenFbo) glViewport(0, 0, width, height);

    return depth::buffer;
}

}   // end depth

}   // end shadow

}   // end nav



