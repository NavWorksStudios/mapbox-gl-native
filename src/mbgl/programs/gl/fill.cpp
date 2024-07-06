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
    
//    static const char* navVertex(const char* shaderSource, size_t preludeOffset) { return shaderSource + preludeOffset; }
//    static const char* navVertex(const char* shaderSource) { return shaderSource + vertexOffset; }
//    static const char* navFragment(const char* shaderSource, size_t preludeOffset) { return shaderSource + preludeOffset; }
//    static const char* navFragment(const char* shaderSource) { return shaderSource + fragmentOffset; }

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

uniform mat4 u_matrix;
uniform lowp vec3 u_camera_pos;
uniform lowp float u_base;
uniform mat4 u_normal_matrix;
uniform lowp float u_render_time;

attribute vec2 a_pos;
attribute vec3 a_normal;

varying vec3 v_camera_pos;
varying vec3 v_pos;

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
       
    v_camera_pos = u_camera_pos;
    gl_Position=u_matrix*vec4(a_pos, u_base, 1);
        
//    v_normal = normalize((u_normal_matrix * vec4(0., 0., 1., 0.0)).xyz);
    v_pos = gl_Position.xyz;
//    v_camera_pos = normalize(u_camera_pos - vec3(a_pos,u_base));
//    v_uv = matcap(v_camera_pos, v_normal).xy;
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

uniform mat4 u_normal_matrix;
uniform bool u_enable_matcap;
uniform lowp float u_spotlight;
uniform lowp float u_render_time;
        
varying vec3 v_pos;
varying vec3 v_camera_pos;
varying vec3 v_normal;
varying vec2 v_uv;

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

        
float spot_light(vec2 uv, vec2 C, float r, float b) {
    return clamp(.2 / clamp(length(uv-C)-r, 0., 1.), 0., b) / 5.;
}
        
vec3 color_flow(vec2 fragCoord) {
    const lowp vec2 resolution = vec2(1800, 720);
    lowp float time = u_render_time * .01;

    lowp vec2 uv = (2. * fragCoord - resolution.xy) / resolution.y;
    lowp vec3 rgb = cos(time * 31. + uv.xyx + vec3(1.0,2.0,4.0)) *.15;

    lowp vec3 spots;
    for(int i = 0; i < 10; i++){
        lowp float n = float(i);
        lowp float s = sin(n * time);

        lowp float c = 2. * (n/14.-.5) * resolution.x / resolution.y;
        lowp vec2 p = vec2(c + cos(n + time * 5.), sin(time * n / 11.));

        lowp float r = abs(0.01 * s);
        lowp float b = (s * 11. + 13.) / 2.;
        
        spots += spot_light(uv, p, r, b);
    }
    
    return rgb + spots;
}
        
void main() {

#ifdef HAS_UNIFORM_u_color
    highp vec4 color=u_color;
#endif

#ifdef HAS_UNIFORM_u_opacity
    lowp float opacity=u_opacity;
#endif

    if (u_spotlight > 0.) {
        const lowp float radius = 1500000.;
        lowp float distance = pow(v_pos.x,2.) + pow(v_pos.y,2.);
        lowp float centerFactor = clamp(distance/radius, 1.-u_spotlight, 1.);
        centerFactor = pow(1. - centerFactor, 3.) * .8;
        gl_FragColor.xyz = mix(color.rgb, color_flow(gl_FragCoord.xy), centerFactor) * opacity; // 距离屏幕中心点越近，越亮
        gl_FragColor.a = color.a * opacity;
    } else {
        gl_FragColor = color * opacity;
    }
        
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
