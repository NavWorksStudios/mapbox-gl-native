//
//  nav.render.cpp
//  Mapbox GL Native
//
//  Created by BernieZhao on 2024/10/10.
//

#include "mbgl/nav/render/nav.render.hpp"
#include "mbgl/nav/nav.style.hpp"
#include "mbgl/nav/render/mat4.h"
#include "mbgl/nav/render/nav.shadow.hpp"
#include "mbgl/nav/render/nav.shadow.frustum.hpp"
#include "mbgl/nav/render/nav.geo.hpp"
#include "mbgl/nav/render/nav.ssao.hpp"
#include "mbgl/nav/render/nav.quad.hpp"


namespace nav {

static auto convertMatrix4 = [] (mbgl::mat4 matrix) {
    Mat4 m;
    for (int i=0; i<16; i++) ((float*)&m)[i] = float(matrix[i]);
    return m;
};

GLuint genTexture(GLint internalformat, GLsizei width, GLsizei height, GLenum format, GLenum type) {
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, internalformat, width, height, 0, format, type, NULL);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    return texture;
}


namespace renderer {

namespace deferred {

const float BUFFER_RATIO = 1.;

int width() {
    return nav::display::pixels::width() * BUFFER_RATIO;
}

int height() {
    return nav::display::pixels::height() * BUFFER_RATIO;
}

GLuint getRenderBuffer(int width, int height) {
    static GLuint buffer = 0;

    static int w = 0, h = 0;
    if (w != width || h != height) {
        w = width;
        h = height;

        glDeleteTextures(1, &buffer);
        buffer = genTexture(GL_RED, width, height, GL_RED, GL_FLOAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    
    return buffer;
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

        const auto renderBuffer = getRenderBuffer(w, h);
        
        const auto shadowDepth = nav::shadow::render(w, h, shadowRenderDelegate);
        nav::shadow::setDepthBuffer(shadowDepth);
        
        const auto gbuffer = nav::geo::renderGeoAndShadow(w, h, renderBuffer, shadowDepth, geoRenderDelegate);
        
        nav::ssao::render(w, h, renderBuffer, gbuffer, zoom, convertMatrix4(projMatrix));
        
        nav::quad::renderBlur(w, h, renderBuffer, bindScreen);
        
        // debug info window
        if (1) {
            int x = 10;
            int ww = w / 8.;
            
            auto fboBinder = [&] () {
                glBindFramebuffer(GL_FRAMEBUFFER, 0);
                glViewport(x, 10, ww, h/8.);
            };
            
            nav::shadow::frustum::ortho::sunlight().render(fboBinder);
            nav::quad::renderMono(w, h, shadowDepth, fboBinder);
            
            
            x += ww + 80;
            nav::quad::render(w, h, gbuffer[0], fboBinder);
            
            x += ww + 10;
            nav::quad::render(w, h, gbuffer[1], fboBinder);
            
            x += ww + 10;
            nav::quad::render(w, h, gbuffer[2], fboBinder);
            
            
            x += ww + 80;
            nav::quad::renderMono(w, h, renderBuffer, fboBinder);
            
            glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
        }
        
    }
    
    glClearColor(clearColor[0], clearColor[1], clearColor[2], clearColor[3]);
    blendEnabled ? glEnable(GL_BLEND) : glDisable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA); // mapbox config
    
}

} // deffered

} // renderer

} // nav
