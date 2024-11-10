// NOTE: DO NOT CHANGE THIS FILE. IT IS AUTOMATICALLY GENERATED.
// clang-format off
#include <mbgl/programs/line_program.hpp>
#include <mbgl/programs/gl/preludes.hpp>
#include <mbgl/programs/gl/shader_source.hpp>
#include <mbgl/gl/program.hpp>

namespace mbgl {

const char* nav_programs_geo_fragmentShader();

namespace programs {
namespace gl {

template <typename>
struct ShaderSource;

template <>
struct ShaderSource<LineHaloProgram> {
    static constexpr const char* name = "line_halo";
    static constexpr const uint8_t hash[8] = {0x7f, 0x8e, 0xaa, 0x53, 0x75, 0x78, 0xac, 0x2e};
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
    
        uniform mat4 u_matrix;
        uniform lowp float u_ratio;
        uniform lowp float u_device_pixel_ratio;
    
        attribute vec2 a_pos_normal;
        attribute vec4 a_data;
        attribute float a_height;
    
        varying vec3 v_color;
        
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
    
            lowp float ANTIALIASING=1.0/u_device_pixel_ratio/2.0;
            lowp vec2 a_extrude=a_data.xy-128.0;
            lowp float a_direction=mod(a_data.z,4.0)-1.0;
    
            lowp vec2 pos=floor(a_pos_normal*0.5);
            lowp vec2 normal=a_pos_normal-2.0*pos;
            normal.y=normal.y*2.0-1.0;

            gapwidth=gapwidth/2.0;
            float halfwidth=width/2.0;
            offset=-1.0*offset;

            lowp float outset=gapwidth+halfwidth*(gapwidth > 0.0 ? 2.0 : 1.0)+(halfwidth==0.0 ? 0.0 : ANTIALIASING);
    
            lowp vec2 dist=outset*a_extrude*scale;
            lowp float u=0.5*a_direction;
            lowp float t=1.0-abs(u);
            lowp vec2 offset2=offset*a_extrude*scale*normal.y*mat2(t,-u,u,t);
    
            vec4 position = vec4(pos+offset2/u_ratio,a_height + 100.,1.0);
            vec4 projected_extrude=u_matrix*vec4(dist/u_ratio,0.0,0.0);

            gl_Position=u_matrix*position+projected_extrude;

            v_color = vec3(1., 0., 0.);
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

        varying vec3 v_color;

        void main() {
            gl_FragColor = vec4(v_color, 1.);
        }
            
    )"; }

};

constexpr const char* ShaderSource<LineHaloProgram>::name;
constexpr const uint8_t ShaderSource<LineHaloProgram>::hash[8];

} // namespace gl
} // namespace programs

namespace gfx {

template <>
std::unique_ptr<gfx::Program<LineHaloProgram>>
Backend::Create<gfx::Backend::Type::OpenGL>(const ProgramParameters& programParameters) {
    return std::make_unique<gl::Program<LineHaloProgram>>(programParameters);
}

} // namespace gfx
} // namespace mbgl

// clang-format on
