// NOTE: DO NOT CHANGE THIS FILE. IT IS AUTOMATICALLY GENERATED.
// clang-format off
#include <mbgl/programs/line_program.hpp>
#include <mbgl/programs/gl/preludes.hpp>
#include <mbgl/programs/gl/shader_source.hpp>
#include <mbgl/gl/program.hpp>

namespace mbgl {
namespace programs {
namespace gl {

template <typename>
struct ShaderSource;

template <>
struct ShaderSource<LineProgram> {
    static constexpr const char* name = "line";
    static constexpr const uint8_t hash[8] = {0x7f, 0x8e, 0xaa, 0x53, 0x75, 0x78, 0xac, 0x2c};
    static constexpr const auto vertexOffset = 30578;
    static constexpr const auto fragmentOffset = 33575;
    
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

        #define scale 0.015873016
        
        attribute vec2 a_pos_normal;
        attribute vec4 a_data;
    
        uniform mat4 u_matrix;
        uniform vec3 u_camera_pos;
        uniform mediump float u_ratio;
        uniform vec2 u_units_to_pixels;
        uniform lowp float u_device_pixel_ratio;
    
        varying vec2 v_normal;
        varying vec2 v_width2;
        varying float v_gamma_scale;
        varying highp float v_linesofar;
        varying vec3 v_pos;
        varying vec3 v_camera_pos;
        
        #ifndef HAS_UNIFORM_u_color
            uniform lowp float u_color_t;
            attribute highp vec4 a_color;
            varying highp vec4 color;
        #else
            uniform highp vec4 u_color;
        #endif
    
        #ifndef HAS_UNIFORM_u_blur
            uniform lowp float u_blur_t;
            attribute lowp vec2 a_blur;
            varying lowp float blur;
        #else
            uniform lowp float u_blur;
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

        #ifndef HAS_UNIFORM_u_blur
            blur=unpack_mix_vec2(a_blur,u_blur_t);
        #else
            lowp float blur=u_blur;
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
    
            float ANTIALIASING=1.0/u_device_pixel_ratio/2.0;
            vec2 a_extrude=a_data.xy-128.0;
            float a_direction=mod(a_data.z,4.0)-1.0;
            v_linesofar=(floor(a_data.z/4.0)+a_data.w*64.0)*2.0;
    
            vec2 pos=floor(a_pos_normal*0.5);
            mediump vec2 normal=a_pos_normal-2.0*pos;
            normal.y=normal.y*2.0-1.0;
            v_normal=normal;

            gapwidth=gapwidth/2.0;
            float halfwidth=width/2.0;
            offset=-1.0*offset;

            float inset=gapwidth+(gapwidth > 0.0 ? ANTIALIASING : 0.0);
            float outset=gapwidth+halfwidth*(gapwidth > 0.0 ? 2.0 : 1.0)+(halfwidth==0.0 ? 0.0 : ANTIALIASING);
    
            mediump vec2 dist=outset*a_extrude*scale;
            mediump float u=0.5*a_direction;
            mediump float t=1.0-abs(u);
            mediump vec2 offset2=offset*a_extrude*scale*normal.y*mat2(t,-u,u,t);
    
            vec4 projected_extrude=u_matrix*vec4(dist/u_ratio,0.0,0.0);
            gl_Position=u_matrix*vec4(pos+offset2/u_ratio,0.0,1.0)+projected_extrude;
    
            float extrude_length_without_perspective=length(dist);
            float extrude_length_with_perspective=length(projected_extrude.xy/gl_Position.w*u_units_to_pixels);
            v_gamma_scale=extrude_length_without_perspective/extrude_length_with_perspective;
            v_width2=vec2(outset,inset);
    
            v_pos = gl_Position.xyz;
            v_camera_pos = u_camera_pos;
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

    
    // OpenGL基础23：平行光与点光源
    // https://blog.csdn.net/Jaihk662/article/details/106722949
    
    // 其他参考
    // https://www.freesion.com/article/98131083178/

    static const char* navFragment(const char* ) { return R"(

        uniform lowp float u_device_pixel_ratio;

        varying vec2 v_width2;
        varying vec2 v_normal;
        varying float v_gamma_scale;
        varying vec3 v_pos;
        varying vec3 v_camera_pos;
    
    #ifndef HAS_UNIFORM_u_color
        varying highp vec4 color;
    #else
        uniform highp vec4 u_color;
    #endif

    #ifndef HAS_UNIFORM_u_blur
        varying lowp float blur;
    #else
        uniform lowp float u_blur;
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

    #ifdef HAS_UNIFORM_u_blur
        lowp float blur=u_blur;
    #endif

    #ifdef HAS_UNIFORM_u_opacity
        lowp float opacity=u_opacity;
    #endif
    
        vec3 lightColor = vec3(1.,1.,1.);
        vec3 cameraPo = v_camera_pos * 0.00001;
        vec3 lightPo = cameraPo;
    
        float ambientStrength = .7;             //环境因子
    
        float specularStrength = 1.5;           //镜面强度
        float reflectance = 2.0;                //反射率

        float constantPara = 1.0;               //距离衰减常量
        float linearPara = 0.000000009;         //线性衰减常量
    
        //环境光
        vec3 norm = normalize(vec3(0.,0.,1.));
        vec3 lightDir = normalize(lightPo - v_pos);                             //当前顶点 至 光源的的单位向量
        vec3 ambient = ambientStrength * vec3(color);                           //环境光 = 环境因子 * 物体的材质颜色

        //漫反射
        float diff = max(dot(norm,lightDir),0.0);                               //DiffuseFactor=光源与法线夹角 max(0,dot(N,L))
        vec3 diffuse = diff * lightColor * vec3(color);                         //漫反射光颜色计算 = 光源的漫反射颜色 * 物体的漫发射材质颜色 * DiffuseFactor

        //镜面反射
        vec3 viewDir = normalize(cameraPo - v_pos);
        vec3 reflectDir = reflect(-lightDir,norm);                              // reflect (genType I, genType N),返回反射向量
        float spec = pow(max(dot(viewDir, reflectDir),0.0),reflectance);        //SpecularFactor = power(max(0,dot(N,H)),shininess)
        vec3 specular = specularStrength * spec * vec3(color);                  //镜面反射颜色 = 光源的镜面光的颜色 * 物体的镜面材质颜色 * SpecularFactor

        //衰减因子计算
        float LFDistance = length(lightPo - v_pos);
        //衰减因子 =  1.0/(距离衰减常量 + 线性衰减常量 * 距离 + 二次衰减常量 * 距离的平方)
        float lightWeakPara = 1.0 / (constantPara + linearPara * LFDistance);

        //最终的颜色
        vec3 res = (ambient + diffuse + specular) * lightWeakPara;              //光照颜色 =(环境颜色 + 漫反射颜色 + 镜面反射颜色)* 衰减因子
        color = vec4(res,1.0);

        float dist=length(v_normal)*v_width2.s;
        float blur2=(blur+1.0/u_device_pixel_ratio)*v_gamma_scale;
        float alpha=clamp(min(dist-(v_width2.t-blur2),v_width2.s-dist)/blur2,0.0,1.0);
        gl_FragColor=color*(alpha*opacity);
        
    #ifdef OVERDRAW_INSPECTOR
        gl_FragColor=vec4(1.0);
    #endif
        }

    )"; }

};

constexpr const char* ShaderSource<LineProgram>::name;
constexpr const uint8_t ShaderSource<LineProgram>::hash[8];

} // namespace gl
} // namespace programs

namespace gfx {

template <>
std::unique_ptr<gfx::Program<LineProgram>>
Backend::Create<gfx::Backend::Type::OpenGL>(const ProgramParameters& programParameters) {
    return std::make_unique<gl::Program<LineProgram>>(programParameters);
}

} // namespace gfx
} // namespace mbgl

// Uncompressed source of line.vertex.glsl:
/*
// floor(127 / 2) == 63.0
// the maximum allowed miter limit is 2.0 at the moment. the extrude normal is
// stored in a byte (-128..127). we scale regular normals up to length 63, but
// there are also "special" normals that have a bigger length (of up to 126 in
// this case).
// #define scale 63.0
#define scale 0.015873016

attribute vec2 a_pos_normal;
attribute vec4 a_data;

uniform mat4 u_matrix;
uniform mediump float u_ratio;
uniform vec2 u_units_to_pixels;
uniform lowp float u_device_pixel_ratio;

varying vec2 v_normal;
varying vec2 v_width2;
varying float v_gamma_scale;
varying highp float v_linesofar;


#ifndef HAS_UNIFORM_u_color
uniform lowp float u_color_t;
attribute highp vec4 a_color;
varying highp vec4 color;
#else
uniform highp vec4 u_color;
#endif


#ifndef HAS_UNIFORM_u_blur
uniform lowp float u_blur_t;
attribute lowp vec2 a_blur;
varying lowp float blur;
#else
uniform lowp float u_blur;
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
    color = unpack_mix_color(a_color, u_color_t);
#else
    highp vec4 color = u_color;
#endif

    
#ifndef HAS_UNIFORM_u_blur
    blur = unpack_mix_vec2(a_blur, u_blur_t);
#else
    lowp float blur = u_blur;
#endif

    
#ifndef HAS_UNIFORM_u_opacity
    opacity = unpack_mix_vec2(a_opacity, u_opacity_t);
#else
    lowp float opacity = u_opacity;
#endif

    
#ifndef HAS_UNIFORM_u_gapwidth
    mediump float gapwidth = unpack_mix_vec2(a_gapwidth, u_gapwidth_t);
#else
    mediump float gapwidth = u_gapwidth;
#endif

    
#ifndef HAS_UNIFORM_u_offset
    lowp float offset = unpack_mix_vec2(a_offset, u_offset_t);
#else
    lowp float offset = u_offset;
#endif

    
#ifndef HAS_UNIFORM_u_width
    mediump float width = unpack_mix_vec2(a_width, u_width_t);
#else
    mediump float width = u_width;
#endif


    // the distance over which the line edge fades out.
    // Retina devices need a smaller distance to avoid aliasing.
    float ANTIALIASING = 1.0 / u_device_pixel_ratio / 2.0;

    vec2 a_extrude = a_data.xy - 128.0;
    float a_direction = mod(a_data.z, 4.0) - 1.0;

    v_linesofar = (floor(a_data.z / 4.0) + a_data.w * 64.0) * 2.0;

    vec2 pos = floor(a_pos_normal * 0.5);

    // x is 1 if it's a round cap, 0 otherwise
    // y is 1 if the normal points up, and -1 if it points down
    // We store these in the least significant bit of a_pos_normal
    mediump vec2 normal = a_pos_normal - 2.0 * pos;
    normal.y = normal.y * 2.0 - 1.0;
    v_normal = normal;

    // these transformations used to be applied in the JS and native code bases.
    // moved them into the shader for clarity and simplicity.
    gapwidth = gapwidth / 2.0;
    float halfwidth = width / 2.0;
    offset = -1.0 * offset;

    float inset = gapwidth + (gapwidth > 0.0 ? ANTIALIASING : 0.0);
    float outset = gapwidth + halfwidth * (gapwidth > 0.0 ? 2.0 : 1.0) + (halfwidth == 0.0 ? 0.0 : ANTIALIASING);

    // Scale the extrusion vector down to a normal and then up by the line width
    // of this vertex.
    mediump vec2 dist = outset * a_extrude * scale;

    // Calculate the offset when drawing a line that is to the side of the actual line.
    // We do this by creating a vector that points towards the extrude, but rotate
    // it when we're drawing round end points (a_direction = -1 or 1) since their
    // extrude vector points in another direction.
    mediump float u = 0.5 * a_direction;
    mediump float t = 1.0 - abs(u);
    mediump vec2 offset2 = offset * a_extrude * scale * normal.y * mat2(t, -u, u, t);

    vec4 projected_extrude = u_matrix * vec4(dist / u_ratio, 0.0, 0.0);
    gl_Position = u_matrix * vec4(pos + offset2 / u_ratio, 0.0, 1.0) + projected_extrude;

    // calculate how much the perspective view squishes or stretches the extrude
    float extrude_length_without_perspective = length(dist);
    float extrude_length_with_perspective = length(projected_extrude.xy / gl_Position.w * u_units_to_pixels);
    v_gamma_scale = extrude_length_without_perspective / extrude_length_with_perspective;

    v_width2 = vec2(outset, inset);
}

*/

// Uncompressed source of line.fragment.glsl:
/*
uniform lowp float u_device_pixel_ratio;

varying vec2 v_width2;
varying vec2 v_normal;
varying float v_gamma_scale;


#ifndef HAS_UNIFORM_u_color
varying highp vec4 color;
#else
uniform highp vec4 u_color;
#endif


#ifndef HAS_UNIFORM_u_blur
varying lowp float blur;
#else
uniform lowp float u_blur;
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

    
#ifdef HAS_UNIFORM_u_blur
    lowp float blur = u_blur;
#endif

    
#ifdef HAS_UNIFORM_u_opacity
    lowp float opacity = u_opacity;
#endif


    // Calculate the distance of the pixel from the line in pixels.
    float dist = length(v_normal) * v_width2.s;

    // Calculate the antialiasing fade factor. This is either when fading in
    // the line in case of an offset line (v_width2.t) or when fading out
    // (v_width2.s)
    float blur2 = (blur + 1.0 / u_device_pixel_ratio) * v_gamma_scale;
    float alpha = clamp(min(dist - (v_width2.t - blur2), v_width2.s - dist) / blur2, 0.0, 1.0);

    gl_FragColor = color * (alpha * opacity);

#ifdef OVERDRAW_INSPECTOR
    gl_FragColor = vec4(1.0);
#endif
}

*/
// clang-format on
