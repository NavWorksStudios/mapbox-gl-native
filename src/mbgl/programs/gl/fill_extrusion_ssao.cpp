// NOTE: DO NOT CHANGE THIS FILE. IT IS AUTOMATICALLY GENERATED.
// clang-format off
#include <mbgl/programs/fill_extrusion_ssao_program.hpp>
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
struct ShaderSource<FillExtrusionSSAOProgram> {
    static constexpr const char* name = "fill_extrusion_ssao";
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
        
        uniform highp mat4 u_matrix;
        uniform highp mat4 u_model_view_matrix;
        uniform highp mat4 u_normal_matrix;
        uniform highp mat4 u_light_matrix;
    
        varying vec3 v_fragPos;
        varying vec3 v_normal;
        varying vec3 v_ao_normal;
        varying vec4 v_lightSpacePos;
        
        uniform lowp float u_base_t;
        attribute highp vec2 a_base;

        uniform lowp float u_height_t;
        attribute highp vec2 a_height;

        void main() {
            float base = unpack_mix_vec2(a_base,u_base_t);
            float height = unpack_mix_vec2(a_height,u_height_t);

            base = max(0.0, base);
            height = max(base, height);
    
            float lowp t = mod(a_normal_ed.x, 2.0);
            float highp z = t > 0. ? height : base;
            highp vec4 pos = vec4(a_pos, z, 1.0);

            // ssao
            v_fragPos = vec3(u_model_view_matrix * pos) / 32.;
            v_normal = vec3(-a_normal_ed.x, -a_normal_ed.y, a_normal_ed.z);
            v_ao_normal = vec3(u_normal_matrix * vec4(v_normal, a_normal_ed.w));
    
            // shadow
            v_lightSpacePos = u_light_matrix * pos;
    
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

        uniform sampler2D u_shadow_map;
        uniform vec2 u_shadow_uv_scale;
        uniform vec3 u_light_dir;

        varying vec3 v_fragPos;
        varying vec3 v_normal;
        varying vec3 v_ao_normal;
        varying vec4 v_lightSpacePos;
    
        float ShadowCalculation(vec4 fragPosLightSpace) {
            // perform perspective divide
            vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    
            // Keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
            if(projCoords.z > 1.0) return 0.0; // 超出光照相机视野
    
            // Transform to [0,1] range
            projCoords = projCoords * 0.5 + 0.5;
    
            // Get depth of current fragment from light's perspective
            float currentDepth = projCoords.z;

            // Calculate bias (based on depth map resolution and slope)    
            // 遮挡关系的计算精度存在误差，bias是容错过滤器。
            // bias小，容忍度高，阴影多。bias大，容忍度低，阴影少。
            // bias尽可能小，以产生更完整的阴影。 但是太小会产生不该有的阴影。

            // 所有面都可能被遮挡，都需要计算阴影
            // dot计算结果：90度为0 | 0到90度为0到1 | 90到180度为0到-1
            vec3 normal = normalize(v_normal);
            vec3 lightDir = normalize(u_light_dir);
            float diff = 1. - dot(normal, lightDir); // (平行光面 1)(背光面 1到0)(受光面 1到2)
            if (v_normal.z < .0001 && diff > .9) return 0.0; // 放弃接近于光照方向的立面，以避免闪烁

            // 系数调整方法：
            // 先将threshold置0，调整transform到最大值，使阴影刚好完全(越小越全)。再调整threshold收边
            const float transform = 0.00048; // for cullface back
            const float threshold = 0.;
            float bias = max(diff * transform, threshold);

    #if 1
            float depth = texture2D(u_shadow_map, projCoords.xy).r;
            return (currentDepth - depth > bias) ? 1.0 : 0.0;
    #else
            // Check whether current frag pos is in shadow
            // PCF (percentage-closer filtering)
            float shadow = 0.0;
            for(int x = -1; x <= 1; ++x) {
                for(int y = -1; y <= 1; ++y) {
                    float pcfDepth = texture2D(u_shadow_map, projCoords.xy + vec2(x, y) * u_shadow_uv_scale).r;
                    shadow += (currentDepth - pcfDepth > bias) ? 1.0 : 0.0;
                }
            }
            return shadow / 9.0;
    #endif
        }

        void main() {
            // store the fragment position vector in the first gbuffer texture
            gl_FragData[0].xyz = v_fragPos;

            // also store the per-fragment normals into the gbuffer
            gl_FragData[1].xyz = normalize(v_ao_normal);

            // and the diffuse per-fragment color
            gl_FragData[2].rgb = vec3(0.95);
    
            // shadow
            float shadow = ShadowCalculation(v_lightSpacePos);
            gl_FragData[3].r = shadow * .5;

//            gl_FragData[0].rgb = vec3(gl_FragData[3].r);
        }
            
    )"; }
    
};

constexpr const char* ShaderSource<FillExtrusionSSAOProgram>::name;
constexpr const uint8_t ShaderSource<FillExtrusionSSAOProgram>::hash[8];

} // namespace gl
} // namespace programs

namespace gfx {

template <>
std::unique_ptr<gfx::Program<FillExtrusionSSAOProgram>>
Backend::Create<gfx::Backend::Type::OpenGL>(const ProgramParameters& programParameters) {
    return std::make_unique<gl::Program<FillExtrusionSSAOProgram>>(programParameters);
}

} // namespace gfx


const char* floorVertexShader() { return R"(

attribute vec2 a_pos;
attribute vec4 a_normal_ed;

uniform mat4 u_matrix;
uniform mat4 u_model_view_matrix;
uniform mat4 u_normal_matrix;
uniform mat4 u_light_matrix;

varying vec3 v_fragPos;
varying vec3 v_normal;
varying vec3 v_ao_normal;
varying vec4 v_lightSpacePos;

void main()
{
    vec4 pos = vec4(a_pos, 0., 1.);

    // ssao
    v_fragPos = vec3(u_model_view_matrix * pos) / 32.;

    v_normal = vec3(-a_normal_ed.x, -a_normal_ed.y, a_normal_ed.z);
    v_ao_normal = vec3(u_normal_matrix * vec4(v_normal, a_normal_ed.w));

    // shadow
    v_lightSpacePos = u_light_matrix * pos;

    gl_Position = u_matrix * pos;
}

)"; }

const char* floorFragmentShader() {
    return programs::gl::ShaderSource<FillExtrusionSSAOProgram>::navFragment(0);
}


} // namespace mbgl

// clang-format on
