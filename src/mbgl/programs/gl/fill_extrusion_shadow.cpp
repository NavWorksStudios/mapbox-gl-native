// NOTE: DO NOT CHANGE THIS FILE. IT IS AUTOMATICALLY GENERATED.
// clang-format off
#include <mbgl/programs/fill_extrusion_shadow_program.hpp>
#include <mbgl/programs/gl/preludes.hpp>
#include <mbgl/programs/gl/shader_source.hpp>
#include <mbgl/gl/program.hpp>

#include "mbgl/nav/nav.theme.hpp"
#include <mbgl/programs/gl/nav_fill_extrusion_P1.hpp>
#include <mbgl/programs/gl/nav_fill_extrusion_P2.hpp>

namespace mbgl {
namespace programs {
namespace gl {

template <typename>
struct ShaderSource;

template <>
struct ShaderSource<FillExtrusionShadowProgram> {
    static constexpr const char* name = "fill_extrusion_shadow";
    static constexpr const uint8_t hash[8] = {0x9d, 0x76, 0x7f, 0xaa, 0x86, 0x57, 0x56, 0x96};
    static constexpr const auto vertexOffset = 21491;
    static constexpr const auto fragmentOffset = 23422;
    
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
    
        float unpack_mix_vec2(const vec2 packedValue,const float t) {
            return mix(packedValue[0],packedValue[1],t);
        }
        
    )"; }
    
    static const char* navVertex(const char* ) { return R"(
        
        attribute highp vec2 a_pos;
        attribute highp vec4 a_normal_ed;
        
        uniform highp mat4 u_matrix; // u_lightMatrix
        
        uniform lowp float u_base_t;
        attribute lowp vec2 a_base;

        uniform mediump float u_height_t;
        attribute mediump vec2 a_height;

        void main() {
            highp float base=unpack_mix_vec2(a_base,u_base_t);
            highp float height=unpack_mix_vec2(a_height,u_height_t);

            base = max(0.0, base);
            height = max(base, height);
            float lowp t = mod(a_normal_ed.x, 2.0);
            float highp z = t > 0. ? height : base;
            highp vec4 pos = vec4(a_pos, z, 1.0);
    
            // for shadow depth
            gl_Position = u_matrix * pos;
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
        
        void main() {
//            gl_FragColor = vec4(vec3(pow((gl_FragCoord.z - 0.5) * 2., 2.)), 1.); // for shadow debug
        }

    )"; }
    
};

constexpr const char* ShaderSource<FillExtrusionShadowProgram>::name;
constexpr const uint8_t ShaderSource<FillExtrusionShadowProgram>::hash[8];

} // namespace gl
} // namespace programs

namespace gfx {

template <>
std::unique_ptr<gfx::Program<FillExtrusionShadowProgram>>
Backend::Create<gfx::Backend::Type::OpenGL>(const ProgramParameters& programParameters) {
    return std::make_unique<gl::Program<FillExtrusionShadowProgram>>(programParameters);
}

} // namespace gfx


const char* floorShadowVertexShader() { return R"(

attribute vec2 a_pos;

uniform mat4 u_matrix;

void main()
{
    vec4 pos = vec4(a_pos, 0., 1.);

    gl_Position = u_matrix * pos;
}

)"; }

const char* floorShadowFragmentShader() {
    return programs::gl::ShaderSource<FillExtrusionShadowProgram>::navFragment(0);
}


} // namespace mbgl

// clang-format on
