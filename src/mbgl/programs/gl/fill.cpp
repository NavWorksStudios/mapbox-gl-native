// NOTE: DO NOT CHANGE THIS FILE. IT IS AUTOMATICALLY GENERATED.
// clang-format off
#include <mbgl/programs/fill_program.hpp>
#include <mbgl/programs/gl/preludes.hpp>
#include <mbgl/programs/gl/shader_source.hpp>
#include <mbgl/gl/program.hpp>

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

    static const char* navVertex(const char* , size_t ) { return
R"(

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
    
    static const char* navVertex(const char* ) { return 
R"(

attribute vec2 a_pos;
uniform mat4 u_matrix;

uniform highp float u_base;

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
    color=unpack_mix_color(a_color,u_color_t);
#else
    highp vec4 color=u_color;
#endif

#ifndef HAS_UNIFORM_u_opacity
    opacity=unpack_mix_vec2(a_opacity,u_opacity_t);
#else
    lowp float opacity=u_opacity;
#endif

    gl_Position=u_matrix*vec4(a_pos,u_base,1);
}

)"; }
    
    static const char* navFragment(const char* , size_t ) { return 
R"(

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
    
    static const char* navFragment(const char* ) { return
R"(

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
highp vec4 color=u_color;
#endif
#ifdef HAS_UNIFORM_u_opacity
lowp float opacity=u_opacity;
#endif
gl_FragColor=color*opacity;
#ifdef OVERDRAW_INSPECTOR
gl_FragColor=vec4(1.0);
#endif
}

)"; }
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
