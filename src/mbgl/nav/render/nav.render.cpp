//
//  nav.render.cpp
//  Mapbox GL Native
//
//  Created by BernieZhao on 2024/10/10.
//

#include "mbgl/nav/render/nav.render.hpp"
#include "mbgl/nav/nav.style.hpp"
#include "mbgl/nav/nav.runtime.hpp"
#include "mbgl/nav/render/mat4.h"
#include "mbgl/nav/render/nav.shadow.hpp"
#include "mbgl/nav/render/nav.shadow.frustum.hpp"
#include "mbgl/nav/render/nav.halo.hpp"
#include "mbgl/nav/render/nav.geo.hpp"
#include "mbgl/nav/render/nav.ssao.hpp"
#include "mbgl/nav/render/nav.quad.hpp"
#include "mbgl/nav/render/nav.glvalue.hpp"
#include "mbgl/nav/render/nav.skybox.hpp"

namespace nav {

namespace render {

static auto convertMatrix4 = [] (mbgl::mat4 matrix) {
    Mat4 m;
    for (int i=0; i<16; i++) ((float*)&m)[i] = float(matrix[i]);
    return m;
};

GLuint genTexture(GLint internalformat, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid* pixels) {
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, internalformat, width, height, 0, format, type, pixels);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    return texture;
}

GLuint genTexture(GLint internalformat, GLsizei width, GLsizei height, GLenum format, GLenum type) {
    return genTexture(internalformat, width, height, format, type, nullptr);
}

bool _showDebugWindow = true;

void switchDebugWindow() {
    _showDebugWindow = !_showDebugWindow;
}

namespace renderbuffer {

const float BUFFER_RATIO = 1.;
int width() { return nav::display::pixels::width() * BUFFER_RATIO; }
int height() { return nav::display::pixels::height() * BUFFER_RATIO; }

static GLuint buffer = 0;

GLuint get() {
    if (!buffer) {
        glDeleteTextures(1, &buffer);
        buffer = genTexture(GL_RED, width(), height(), GL_RED, GL_FLOAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    
    return buffer;
}
}

void renderDeferred(const mbgl::PaintParameters& parameters,
                    std::function<void()> shadowRenderDelegate,
                    std::function<void()> haloRenderDelegate,
                    std::function<void()> geoRenderDelegate) {
    const float zoom = parameters.state.getZoom();
    if (zoom < 15.) return;
    
    gl::Value<Viewport> viewPort;
    gl::Value<BindFramebuffer> bindFramebuffer;
    
    const int w = renderbuffer::width();
    const int h = renderbuffer::height();
    const auto renderBuffer = renderbuffer::get();
    
    // 1
    const auto shadowBuffer = shadow::render(w, h, shadowRenderDelegate);
    
    // 2
    const auto haloBuffer = halo::render(w, h, haloRenderDelegate);

    // 3
    const auto gbuffer = geo::renderGeoAndShadow(w, h, renderBuffer, shadowBuffer, geoRenderDelegate);
    
    // 4
    const auto& projMatrix = convertMatrix4(parameters.state.getViewToClipMatrix());
    ssao::render(w, h, zoom, projMatrix, renderBuffer, haloBuffer, gbuffer);
    
    // 5
    bindFramebuffer.restore();
    viewPort.restore();
    quad::renderBlur(renderBuffer, w, h);
    
    // 6
    if (_showDebugWindow) {
        int x = 20;
        const mbgl::Size size = { uint32_t(w / 8.), uint32_t(h / 8.) };
        
        {
            Viewport::Set({x, 20, size});
            
            const auto& projMatrix = parameters.state.getSunlightViewToClipMatrix();
            shadow::frustum::ortho::sunlight().renderArea(projMatrix);
            
            quad::renderRedChannel(shadowBuffer, .8);
        }
        
        {
            Viewport::Set({x += size.width + 20, 20, size});
            quad::renderStandard(haloBuffer, .8);
        }
        
        {
            Viewport::Set({x += size.width + 20, 20, size});
            quad::renderStandard(gbuffer[1], .8);
        }
        
        {
            Viewport::Set({x += size.width + 20, 20, size});
            quad::renderRedChannel(renderBuffer, .8);
        }
    }
}

void renderLogo(const mbgl::PaintParameters& parameters) {
    gl::Value<Viewport> viewPort;
    
    const int w = renderbuffer::width();
    const int h = renderbuffer::height();
    const auto logo = nav::runtime::texture::logo();
    const auto& size = std::get<0>(logo);
    Viewport::Set({ int(w - size.width), 0, size });
    quad::renderStandard(std::get<1>(logo));
}

void renderSkybox(const mbgl::PaintParameters& parameters) {
//    mbgl::mat4 projMatrix;
//    parameters.state.getProjMatrix(projMatrix);
//    skybox::render(projMatrix);
    
    skybox::render(parameters.state.getViewToClipMatrix());
}

} // renderer

} // nav
