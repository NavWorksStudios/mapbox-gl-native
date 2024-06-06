// NOTE: DO NOT CHANGE THIS FILE. IT IS AUTOMATICALLY GENERATED.
// clang-format off
#include <mbgl/programs/fill_extrusion_program.hpp>
#include <mbgl/programs/gl/preludes.hpp>
#include <mbgl/programs/gl/shader_source.hpp>
#include <mbgl/gl/program.hpp>

namespace mbgl {
namespace programs {
namespace gl {

template <typename>
struct ShaderSource;

template <>
struct ShaderSource<FillExtrusionProgram> {
    static constexpr const char* name = "fill_extrusion";
    static constexpr const uint8_t hash[8] = {0x9d, 0x76, 0x7f, 0xaa, 0x86, 0x57, 0x56, 0x96};
    static constexpr const auto vertexOffset = 21491;
    static constexpr const auto fragmentOffset = 23422;
    
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
        uniform vec3 u_lightcolor;
        uniform lowp vec3 u_lightpos;
        uniform lowp float u_lightintensity;
        uniform float u_vertical_gradient;
        uniform lowp float u_opacity;
    
        attribute vec2 a_pos;
        attribute vec4 a_normal_ed;
    
        varying vec3 v_pos;
        varying vec4 v_color;
                
        #ifndef HAS_UNIFORM_u_base
            uniform lowp float u_base_t;
            attribute highp vec2 a_base;
        #else
            uniform highp float u_base;
        #endif
        
        #ifndef HAS_UNIFORM_u_height
            uniform lowp float u_height_t;
            attribute highp vec2 a_height;
        #else
            uniform highp float u_height;
        #endif
        
        #ifndef HAS_UNIFORM_u_color
            uniform lowp float u_color_t;
            attribute highp vec4 a_color;
        #else
            uniform highp vec4 u_color;
        #endif
        
        void main() {
        #ifndef HAS_UNIFORM_u_base
            highp float base=unpack_mix_vec2(a_base,u_base_t);
        #else
            highp float base=u_base;
        #endif
        
        #ifndef HAS_UNIFORM_u_height
            highp float height=unpack_mix_vec2(a_height,u_height_t);
        #else
            highp float height=u_height;
        #endif
        
        #ifndef HAS_UNIFORM_u_color
            highp vec4 color=unpack_mix_color(a_color,u_color_t);
        #else
            highp vec4 color=u_color;
        #endif
        
        // normal
        vec3 normal=a_normal_ed.xyz;
    
        // position
        //base=max(0.0,base);
        height=max(0.0,height);
        bool withheight=mod(normal.x,2.0)>0.0;
        gl_Position=u_matrix*vec4(a_pos,withheight?height:base,1);
    
        v_pos=gl_Position.xyz;
        
        // ambient light
        float colorvalue=color.r*0.2126+color.g*0.7152+color.b*0.0722;
        vec4 ambientlight=vec4(0.2,0.2,0.2,1.0);
        color+=ambientlight;

        // color
        float directional=clamp(dot(normal/16384.0,u_lightpos),0.0,1.0);
        directional=mix((1.0-u_lightintensity), max((1.0-colorvalue+u_lightintensity),1.0), directional)*0.4;
        if (normal.y!=0.0) {
            directional*=((1.0-u_vertical_gradient)+(u_vertical_gradient*clamp((height+base)*pow(height/150.0,0.5),mix(0.7,0.98,1.0-u_lightintensity),1.0)));
        }
        v_color.r=clamp(color.r*directional*u_lightcolor.r,0.3*(1.0-u_lightcolor.r),1.0);
        v_color.g=clamp(color.g*directional*u_lightcolor.g,0.3*(1.0-u_lightcolor.g),1.0);
        v_color.b=clamp(color.b*directional*u_lightcolor.b,0.3*(1.0-u_lightcolor.b),1.0);
        v_color.a=1.0;
        v_color*=u_opacity;

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
    
    static const char* navFragment(const char* ) { return R"(

        varying vec3 v_pos;
        varying vec4 v_color;

        void main() {
        gl_FragColor=v_color;
        gl_FragColor.a=gl_FragColor.a*0.2+pow(v_pos.z,.7)/600.;
        
        #ifdef OVERDRAW_INSPECTOR
            gl_FragColor=vec4(1.0);
        #endif
        }
        
    )"; }
    
};

constexpr const char* ShaderSource<FillExtrusionProgram>::name;
constexpr const uint8_t ShaderSource<FillExtrusionProgram>::hash[8];

} // namespace gl
} // namespace programs

namespace gfx {

template <>
std::unique_ptr<gfx::Program<FillExtrusionProgram>>
Backend::Create<gfx::Backend::Type::OpenGL>(const ProgramParameters& programParameters) {
    return std::make_unique<gl::Program<FillExtrusionProgram>>(programParameters);
}

} // namespace gfx
} // namespace mbgl

// Uncompressed source of fill_extrusion.vertex.glsl:
/*
uniform mat4 u_matrix;
uniform vec3 u_lightcolor;
uniform lowp vec3 u_lightpos;
uniform lowp float u_lightintensity;
uniform float u_vertical_gradient;
uniform lowp float u_opacity;

attribute vec2 a_pos;
attribute vec4 a_normal_ed;

varying vec4 v_color;


#ifndef HAS_UNIFORM_u_base
uniform lowp float u_base_t;
attribute highp vec2 a_base;
#else
uniform highp float u_base;
#endif


#ifndef HAS_UNIFORM_u_height
uniform lowp float u_height_t;
attribute highp vec2 a_height;
#else
uniform highp float u_height;
#endif



#ifndef HAS_UNIFORM_u_color
uniform lowp float u_color_t;
attribute highp vec4 a_color;
#else
uniform highp vec4 u_color;
#endif


void main() {
    
#ifndef HAS_UNIFORM_u_base
    highp float base = unpack_mix_vec2(a_base, u_base_t);
#else
    highp float base = u_base;
#endif

    
#ifndef HAS_UNIFORM_u_height
    highp float height = unpack_mix_vec2(a_height, u_height_t);
#else
    highp float height = u_height;
#endif

    
#ifndef HAS_UNIFORM_u_color
    highp vec4 color = unpack_mix_color(a_color, u_color_t);
#else
    highp vec4 color = u_color;
#endif


    vec3 normal = a_normal_ed.xyz;

    base = max(0.0, base);
    height = max(0.0, height);

    float t = mod(normal.x, 2.0);

    gl_Position = u_matrix * vec4(a_pos, t > 0.0 ? height : base, 1);

    // Relative luminance (how dark/bright is the surface color?)
    float colorvalue = color.r * 0.2126 + color.g * 0.7152 + color.b * 0.0722;

    v_color = vec4(0.0, 0.0, 0.0, 1.0);

    // Add slight ambient lighting so no extrusions are totally black
    vec4 ambientlight = vec4(0.03, 0.03, 0.03, 1.0);
    color += ambientlight;

    // Calculate cos(theta), where theta is the angle between surface normal and diffuse light ray
    float directional = clamp(dot(normal / 16384.0, u_lightpos), 0.0, 1.0);

    // Adjust directional so that
    // the range of values for highlight/shading is narrower
    // with lower light intensity
    // and with lighter/brighter surface colors
    directional = mix((1.0 - u_lightintensity), max((1.0 - colorvalue + u_lightintensity), 1.0), directional);

    // Add gradient along z axis of side surfaces
    if (normal.y != 0.0) {
        // This avoids another branching statement, but multiplies by a constant of 0.84 if no vertical gradient,
        // and otherwise calculates the gradient based on base + height
        directional *= (
            (1.0 - u_vertical_gradient) +
            (u_vertical_gradient * clamp((t + base) * pow(height / 150.0, 0.5), mix(0.7, 0.98, 1.0 - u_lightintensity), 1.0)));
    }

    // Assign final color based on surface + ambient light color, diffuse light directional, and light color
    // with lower bounds adjusted to hue of light
    // so that shading is tinted with the complementary (opposite) color to the light color
    v_color.r += clamp(color.r * directional * u_lightcolor.r, mix(0.0, 0.3, 1.0 - u_lightcolor.r), 1.0);
    v_color.g += clamp(color.g * directional * u_lightcolor.g, mix(0.0, 0.3, 1.0 - u_lightcolor.g), 1.0);
    v_color.b += clamp(color.b * directional * u_lightcolor.b, mix(0.0, 0.3, 1.0 - u_lightcolor.b), 1.0);
    v_color *= u_opacity;
}

*/

// Uncompressed source of fill_extrusion.fragment.glsl:
/*
varying vec4 v_color;

void main() {
    gl_FragColor = v_color;

#ifdef OVERDRAW_INSPECTOR
    gl_FragColor = vec4(1.0);
#endif
}

*/
// clang-format on
