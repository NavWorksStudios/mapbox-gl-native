//
//  nav.emissive.cpp
//
//  Created by BernieZhao on 2024/11/9.
//

#include "mbgl/nav/render/nav.emissive.hpp"
#include "mbgl/nav/render/nav.glvalue.hpp"


namespace nav {
namespace render {

GLuint genTexture(GLint internalformat, GLsizei width, GLsizei height, GLenum format, GLenum type);

namespace emissive {

GLuint fbo = 0;
GLuint buffer = 0;
GLuint rboDepth = 0;

void initResource(int width, int height) {
    static int w = 0, h = 0;
    if (w != width || h != height) {
        w = width;
        h = height;
        
        if (!fbo) glGenFramebuffers(1, &fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        
        // color buffer
        glDeleteTextures(1, &buffer);
        buffer = genTexture(GL_RGBA, width, height, GL_RGBA, GL_FLOAT);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, buffer, 0);
        glDrawBuffer(GL_COLOR_ATTACHMENT0);
        
        // create and attach depth buffer (renderbuffer)
        glDeleteRenderbuffers(1, &rboDepth);
        glGenRenderbuffers(1, &rboDepth);
        glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}

GLuint render(uint32_t width, uint32_t height, std::function<void()> renderDelegate) {
    initResource(width, height);
    
    gl::Value<BindFramebuffer> bindFramebuffer;
    gl::Value<Viewport> viewport;
    gl::Value<DepthMask> depthMask;
    gl::Value<ClearColor> clearColor;
    
    {
        BindFramebuffer::Set(fbo);
        Viewport::Set({ 0, 0, { width, height } });
        DepthMask::Set(DepthMaskType::ReadWrite);
        ClearColor::Set({0, 0, 0, 0});

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        renderDelegate();
    }
    
    return buffer;
}

}   // end emissive
}   // end render
}   // end nav



