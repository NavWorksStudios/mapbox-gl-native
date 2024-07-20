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
uniform lowp float u_base;
uniform lowp vec4 u_palette_color;
uniform lowp float u_palette_lightness;

attribute vec2 a_pos;

varying lowp vec3 v_pos;
varying lowp vec2 v_texture_pos;

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
       
    gl_Position=u_matrix*vec4(a_pos,u_base,1.);
    v_pos=gl_Position.xyz;
    v_texture_pos=a_pos;

#ifndef HAS_UNIFORM_u_color
    // 灰阶色变换主题色
    if (u_palette_lightness>0.) {
        lowp float lightness=color.r/u_palette_lightness;
        color=vec4(u_palette_color.rgb*lightness,color.a);
    }
#endif

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

uniform lowp float u_spotlight;
uniform lowp float u_render_time;
uniform lowp float u_water_wave;
uniform lowp float u_water_data_z_scale;
uniform lowp float u_clip_region;
uniform lowp float u_focus_region;

varying lowp vec3 v_pos;
varying lowp vec2 v_texture_pos;

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

// -------------- color flow ---------------
        
float spot_light(vec2 uv, vec2 C, float r, float b) {
    return clamp(.2 / clamp(length(uv-C)-r, 0., 1.), 0., b) / 5.;
}

vec3 color_flow(lowp vec2 fragCoord, lowp vec2 resolution) {
    lowp float time = u_render_time * .01;
    lowp vec2 uv = (fragCoord*2. - resolution.xy) / resolution.y;
    lowp vec3 rgb = cos(time * 31. + uv.xyx + vec3(1.0,2.0,4.0)) * .2;

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

// -------------- grid color ---------------

//created by TRASHTRASH aka Joshua deLorimier

#define iter 40.0
#define scaleSpeed 3.0
#define satSpeed 4.2

// Dave Hoskins - https://www.shadertoy.com/view/4djSRW
//noise
float N2(vec2 p) {
    vec3 p3  = fract(vec3(p.xyx) * vec3(443.897, 441.423, 437.195));
    p3 += dot(p3, p3.yzx + 19.19);
    return fract((p3.x + p3.y) * p3.z);
}

float grid_color( vec2 fragCoord, lowp vec2 resolution ) {
    float iTime = u_render_time * .6;

    //create coordinates
    vec2 uv = (fragCoord - 0.5*resolution.xy)/resolution.y;
    
    //iterate to make grid
    uv*=iter;

    //give ID's for each square
    vec2 gv=fract(uv);//-0.5; 去掉后间距变大
    vec2 id=floor(uv);

    //random values
    float ran = N2(id);

    //offset for each grid
    vec2 d = abs(gv) - (abs(sin((iTime*scaleSpeed)*ran)*0.5)-0.05);

    //draw the square
    float rect = min(max(d.x, d.y),0.0) + length(max(d, 0.));
    float r = step(0., rect);

    //combine square and offset to the color var
    return abs((1.-r) * sin((iTime*satSpeed)*ran));
}

// -------------- main ---------------

void main() {

#ifdef HAS_UNIFORM_u_color
    highp vec4 color=u_color;
#endif

#ifdef HAS_UNIFORM_u_opacity
    lowp float opacity=u_opacity;
#endif

    if (u_water_wave > 0.) { // 水面波光

        // point light
        const lowp vec3 cameraPos=vec3(0.,500.,0.);
        const lowp vec3 lightPos=vec3(0.,1000.,2000.);
        const lowp float specular=1.5; // 镜面反射强度
        const lowp float shininess=4.; // 反光度
        lowp vec3 lightDir=normalize(v_pos.xyz-lightPos);
        lowp vec3 viewDir=normalize(cameraPos-v_pos.xyz);
        lowp vec3 reflectDir=reflect(lightDir,vec3(0.,1.,0.)); // reflect (genType I, genType N),返回反射向量
        lowp float brighten=max(specular*pow(max(dot(viewDir,reflectDir),0.0),shininess), 0.); // power(max(0,dot(N,H)),shininess)

        lowp float distance=pow(v_pos.x,2.)+pow(v_pos.z,2.);
        lowp float fadeout=clamp(1.-distance/u_clip_region,0.,1.);
        fadeout=pow(fadeout,3.) * u_water_wave;

        const lowp vec2 texture_size = vec2(8000.);
        lowp vec2 coord=vec2(
        mod(v_texture_pos.x*u_water_data_z_scale,texture_size.x),
        mod(v_texture_pos.y*u_water_data_z_scale,texture_size.y));
        lowp float gridcolor=grid_color(coord,texture_size);
        gridcolor = pow(gridcolor,3.);

        gl_FragColor=color;
        gl_FragColor.rgb += (gridcolor + brighten) * fadeout * .2;
        gl_FragColor*=opacity;

    } else {

        if (u_spotlight > 0.) { // 五彩地面

            lowp float distance=pow(v_pos.x,2.)+pow(v_pos.y,2.);
            lowp float fadeout=clamp(1.-distance/(u_focus_region*.3),0.,u_spotlight);
            fadeout=pow(fadeout,3.);

            vec3 colorflow=color_flow(gl_FragCoord.xy,vec2(2000.));
            gl_FragColor.rgb=mix(color.rgb,colorflow,fadeout)*opacity; // 距离屏幕中心点越近，越亮
            gl_FragColor.a=color.a*opacity;

        } else {

            gl_FragColor=color*opacity;

        }

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
