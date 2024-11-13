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

#include <mbgl/gl/value.hpp>

using namespace mbgl::gl::value;
using namespace mbgl::gfx;


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

struct GLConfigAutoRestore {
    template <typename T> struct Value {
        typename T::Type v = T::Default;
        Value() { v = T::Get(); }
        ~Value() { restore(); }
        void restore() { T::Set(v); }
        operator typename T::Type () { return v; }
        operator const typename T::Type () const { return v; }
    };
    
    Value<ClearDepth> clearDepth;
    Value<ClearColor> clearColor;
    Value<ClearStencil> clearStencil;
    Value<StencilMask> stencilMask;
    Value<DepthMask> depthMask;
    Value<ColorMask> colorMask;
    Value<StencilFunc> stencilFunc;
    Value<StencilTest> stencilTest;
    Value<StencilOp> stencilOp;
    Value<DepthRange> depthRange;
    Value<DepthTest> depthTest;
    Value<DepthFunc> depthFunc;
    Value<Blend> blend;
    Value<BlendEquation> blendEquation;
    Value<BlendFunc> blendFunc;
    Value<BlendColor> blendColor;
    Value<Program> program;
    Value<Viewport> viewPort;
    Value<ScissorTest> scissorTest;
    Value<BindFramebuffer> bindFramebuffer;
    Value<CullFace> cullFace;
    Value<CullFaceSide> cullFaceSide;
    Value<CullFaceWinding> cullFaceWinding;
};

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
                    std::function<void()> renderShadowDelegate,
                    std::function<void()> renderHaloDelegate,
                    std::function<void()> renderGeoDelegate) {
    const float zoom = parameters.state.getZoom();
    if (zoom < 15.) return;
    
    GLConfigAutoRestore config;

    static auto resetDrawMode = [] () {
        DepthTest::Set(true);
        DepthMask::Set(DepthMaskType::ReadWrite);
        DepthFunc::Set(DepthFunctionType::LessEqual);
        
        StencilTest::Set(false);
        
        CullFace::Set(true);
        CullFaceSide::Set(CullFaceSideType::Back);
        
        Blend::Set(true);
        BlendFunc::Set({ColorBlendFactorType::One, ColorBlendFactorType::OneMinusSrcAlpha});
    };
    
    const int w = renderbuffer::width();
    const int h = renderbuffer::height();
    const auto renderBuffer = renderbuffer::get();
    
    // 1
    resetDrawMode();
    const auto shadowBuffer = shadow::render(w, h, renderShadowDelegate);
    
    // 2
    resetDrawMode();
    const auto haloBuffer = halo::render(w, h, renderHaloDelegate);

    // 3
    resetDrawMode();
    const auto gbuffer = geo::renderGeoAndShadow(w, h, renderBuffer, shadowBuffer, renderGeoDelegate);
    
    // 4
    resetDrawMode();
    const auto& projMatrix = convertMatrix4(parameters.state.getViewToClipMatrix());
    ssao::render(w, h, zoom, projMatrix, renderBuffer, haloBuffer, gbuffer);
    
    // 5
    resetDrawMode();
    config.bindFramebuffer.restore();
    config.viewPort.restore();
    quad::renderBlur(renderBuffer, w, h);
    
    // 6
    if (_showDebugWindow) {
        int x = 20;
        const mbgl::Size size = { uint32_t(w / 8.), uint32_t(h / 8.) };
        
        {
            Viewport::Set({x, 20, size});
            
            mbgl::mat4 matrix;
            parameters.state.getSunlightProjMatrix(matrix);
            shadow::frustum::ortho::sunlight().renderGroundProjection(matrix);
            
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
    GLConfigAutoRestore config;
    
    const int w = renderbuffer::width();
    const int h = renderbuffer::height();
    const auto logo = nav::runtime::texture::logo();
    const auto& size = std::get<0>(logo);
    Viewport::Set({ int(w - size.width), 0, size });
    quad::renderStandard(std::get<1>(logo));
}

} // renderer

} // nav
