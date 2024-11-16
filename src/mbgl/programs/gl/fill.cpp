// NOTE: DO NOT CHANGE THIS FILE. IT IS AUTOMATICALLY GENERATED.
// clang-format off
#include <mbgl/programs/fill_program.hpp>
#include <mbgl/programs/gl/preludes.hpp>
#include <mbgl/programs/gl/shader_source.hpp>
#include <mbgl/gl/program.hpp>

#include "mbgl/nav/nav.theme.hpp"
#include <mbgl/programs/nav/p1/fill.hpp>
#include <mbgl/programs/nav/p2/fill.hpp>

namespace mbgl {
namespace programs {
namespace gl {

template <typename>
struct ShaderSource;

template <>
struct ShaderSource<FillProgram> {
    static constexpr const char* name = "fill";
    static constexpr const uint8_t hash[8] = {0x87, 0xea, 0x65, 0x7f, 0x0c, 0x9b, 0x97, 0x5d};
    static constexpr const auto vertexOffset = 12862;
    static constexpr const auto fragmentOffset = 13506;
    
//    static const char* navVertex(const char* shaderSource, size_t preludeOffset) { return shaderSource + preludeOffset; }
//    static const char* navVertex(const char* shaderSource) { return shaderSource + vertexOffset; }
//    static const char* navFragment(const char* shaderSource, size_t preludeOffset) { return shaderSource + preludeOffset; }
//    static const char* navFragment(const char* shaderSource) { return shaderSource + fragmentOffset; }

static const char* navVertex(const char* , size_t ) { return R"(

#ifdef GL_ES
precision highp float;
#else
#if !defined(lowp)
#define lowp
#endif
#if !defined(mediump)
#define mediump
#endif
#if !defined(highp)
#define highp
#endif
#endif
vec2 unpack_float(const float packedValue) {
    int packedIntValue=int(packedValue);
    int v0=packedIntValue/256;
    return vec2(v0,packedIntValue-v0*256);
}

vec2 unpack_opacity(const float packedOpacity) {
    int intOpacity=int(packedOpacity)/2;
    return vec2(float(intOpacity)/127.0,mod(packedOpacity,2.0));
}

vec4 decode_color(const vec2 encodedColor) {
    return vec4(unpack_float(encodedColor[0])/255.0,unpack_float(encodedColor[1])/255.0);
}

float unpack_mix_vec2(const vec2 packedValue,const float t) {
    return mix(packedValue[0],packedValue[1],t);
}

vec4 unpack_mix_color(const vec4 packedColors,const float t) {
    vec4 minColor=decode_color(vec2(packedColors[0],packedColors[1]));
    vec4 maxColor=decode_color(vec2(packedColors[2],packedColors[3]));
    return mix(minColor,maxColor,t);
}

vec2 get_pattern_pos(const vec2 pixel_coord_upper,const vec2 pixel_coord_lower,const vec2 pattern_size,const float tile_units_to_pixels,const vec2 pos) {
    vec2 offset=mod(mod(mod(pixel_coord_upper,pattern_size)*256.0,pattern_size)*256.0+pixel_coord_lower,pattern_size);
    return (tile_units_to_pixels*pos+offset)/pattern_size;
}

)"; }

static const char* navVertex(const char* ) { return R"(

uniform mat4 u_matrix;
uniform mat4 u_model_matrix;
uniform mat4 u_model_matrix_p20;
uniform lowp float u_base;

uniform lowp vec4 u_palette_color;
uniform lowp float u_palette_lightness;

uniform vec2 u_texsize;
uniform float u_textype;

uniform sampler2D u_image;
uniform sampler2D u_image0;
uniform sampler2D u_image1;
        
attribute vec2 a_pos;

varying vec3 v_world_pos;
varying vec2 v_uv;

#ifndef HAS_UNIFORM_u_color
    uniform lowp float u_color_t;
    attribute vec4 a_color;
    varying vec4 color;
#else
    uniform vec4 u_color;
#endif

#ifndef HAS_UNIFORM_u_opacity
    uniform lowp float u_opacity_t;
    attribute lowp vec2 a_opacity;
    varying lowp float opacity;
#else
    uniform lowp float u_opacity;
#endif

#ifndef HAS_UNIFORM_u_gapwidth
    uniform lowp float u_gapwidth_t;
    attribute mediump vec2 a_gapwidth;
#else
    uniform mediump float u_gapwidth;
#endif

#ifndef HAS_UNIFORM_u_offset
    uniform lowp float u_offset_t;
    attribute lowp vec2 a_offset;
#else
    uniform lowp float u_offset;
#endif

#ifndef HAS_UNIFORM_u_width
    uniform lowp float u_width_t;
    attribute mediump vec2 a_width;
#else
    uniform mediump float u_width;
#endif
        
void main() {
#ifndef HAS_UNIFORM_u_color
    color=unpack_mix_color(a_color,u_color_t);
#else
    highp vec4 color=u_color;
#endif

#ifndef HAS_UNIFORM_u_opacity
    opacity=unpack_mix_vec2(a_opacity,u_opacity_t);
#else
    lowp float opacity=u_opacity;
#endif

#ifndef HAS_UNIFORM_u_gapwidth
    mediump float gapwidth=unpack_mix_vec2(a_gapwidth,u_gapwidth_t);
#else
    mediump float gapwidth=u_gapwidth;
#endif

#ifndef HAS_UNIFORM_u_offset
    lowp float offset=unpack_mix_vec2(a_offset,u_offset_t);
#else
    lowp float offset=u_offset;
#endif

#ifndef HAS_UNIFORM_u_width
    mediump float width=unpack_mix_vec2(a_width,u_width_t);
#else
    mediump float width=u_width;
#endif
    
    vec4 pos = vec4(a_pos, u_base, 1.);
    gl_Position = u_matrix * pos;

    v_world_pos = vec3(u_model_matrix * pos);

    const float scaleFactor = 1. / 1.;
    v_uv = vec2(u_model_matrix_p20 * pos);
    v_uv = fract(v_uv / 102400.) * 102400.;
    v_uv /= u_texsize * scaleFactor;

#ifndef HAS_UNIFORM_u_color
    // 灰阶色变换主题色
    if (u_palette_lightness > 0.) {
        lowp float lightness = color.r / u_palette_lightness;
        color = vec4(u_palette_color.rgb * lightness, color.a);
    }
#endif

}

)"; }
    
static const char* navFragment(const char* , size_t ) { return R"(

#ifdef GL_ES
precision mediump float;
#else
#if !defined(lowp)
#define lowp
#endif
#if !defined(mediump)
#define mediump
#endif
#if !defined(highp)
#define highp
#endif
#endif

)"; }
    
static const char* navFragment(const char* ) {
    switch (nav::theme::getShaderIndex()) {
        case 1:
            return nav::p1::navFragment(nullptr);
        case 2:
            return nav::p2::navFragment(nullptr);
        default:
            return "";
    }
}

};

constexpr const char* ShaderSource<FillProgram>::name;
constexpr const uint8_t ShaderSource<FillProgram>::hash[8];

} // namespace gl
} // namespace programs

namespace gfx {

template <>
std::unique_ptr<gfx::Program<FillProgram>>
Backend::Create<gfx::Backend::Type::OpenGL>(const ProgramParameters& programParameters) {
    return std::make_unique<gl::Program<FillProgram>>(programParameters);
}

} // namespace gfx
} // namespace mbgl

// Uncompressed source of fill.vertex.glsl:
/*
attribute vec2 a_pos;

uniform mat4 u_matrix;


#ifndef HAS_UNIFORM_u_color
uniform lowp float u_color_t;
attribute highp vec4 a_color;
varying highp vec4 color;
#else
uniform highp vec4 u_color;
#endif


#ifndef HAS_UNIFORM_u_opacity
uniform lowp float u_opacity_t;
attribute lowp vec2 a_opacity;
varying lowp float opacity;
#else
uniform lowp float u_opacity;
#endif


void main() {
    
#ifndef HAS_UNIFORM_u_color
    color = unpack_mix_color(a_color, u_color_t);
#else
    highp vec4 color = u_color;
#endif

    
#ifndef HAS_UNIFORM_u_opacity
    opacity = unpack_mix_vec2(a_opacity, u_opacity_t);
#else
    lowp float opacity = u_opacity;
#endif


    gl_Position = u_matrix * vec4(a_pos, 0, 1);
}

*/

// Uncompressed source of fill.fragment.glsl:
/*

#ifndef HAS_UNIFORM_u_color
varying highp vec4 color;
#else
uniform highp vec4 u_color;
#endif


#ifndef HAS_UNIFORM_u_opacity
varying lowp float opacity;
#else
uniform lowp float u_opacity;
#endif


void main() {
    
#ifdef HAS_UNIFORM_u_color
    highp vec4 color = u_color;
#endif

    
#ifdef HAS_UNIFORM_u_opacity
    lowp float opacity = u_opacity;
#endif


    gl_FragColor = color * opacity;

#ifdef OVERDRAW_INSPECTOR
    gl_FragColor = vec4(1.0);
#endif
}

*/
// clang-format on
