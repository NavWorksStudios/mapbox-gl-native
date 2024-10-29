
#include <cassert>


namespace nav {
namespace programs {

typedef GLint(*GetLocation)(GLuint, const GLchar*);

template <GetLocation f> struct Location {
    Location(GLint program, const char* name) {
        value = f(program, name);
        assert(value >= 0);
    }
    
    operator GLint () { return value; }
    GLint value;
};

using UniformLocation = Location<glGetUniformLocation>;
using AttribLocation = Location<glGetAttribLocation>;


namespace ssao {
    
static const char* vertexShader() { return R"(

attribute vec3 aPos;
attribute vec2 aTexCoords;

varying vec2 TexCoords;

void main()
{
    gl_Position = vec4(aPos, 1.0);
    TexCoords = aTexCoords;
}

)"; }

static const char* genSSAOFragmentShader() { return R"(

varying vec2 TexCoords;

uniform mat4 u_projection;
uniform vec2 u_noise_uv_scale;

uniform sampler2D u_position;
uniform sampler2D u_normal;
uniform sampler2D u_albedo;
uniform sampler2D u_noise;

#define SAMPLE_SIZE 16
uniform vec3 u_smaple_kernels[SAMPLE_SIZE];
uniform float u_sample_radius[SAMPLE_SIZE];
uniform float u_depth_bias[SAMPLE_SIZE];

const float QUADRATIC = 1.1; // 强度
const float CONTRAST = 1.1; // 对比度

const float NEAR_DEPTH = 0.;
const float FAR_DEPTH = -350.;

void main() {

    float occlusion = 0.0;

    vec3 kernelPos = texture2D(u_position, TexCoords).xyz;
    vec3 albedo = texture2D(u_albedo, TexCoords).xyz;
    if (kernelPos.z > FAR_DEPTH && albedo.r > 0.) {

        // 动态采样数，近密远疏，可以大幅降低开销
        float depth_factor = clamp((FAR_DEPTH - kernelPos.z) / FAR_DEPTH, .2, 1.);
        int sample_count = int(float(SAMPLE_SIZE) * depth_factor);

        // get input for SSAO algorithm
        vec3 kernelNormal = texture2D(u_normal, TexCoords).xyz;
        vec3 random = texture2D(u_noise, TexCoords * u_noise_uv_scale).xyz;

        // create TBN change-of-basis matrix: from tangent-space to view-space
        // 使用Gramm-Schmidt方法我们可以创建正交的TBN矩，同时使用random进行偏移。
        // 注意因为我们使用的是随机向量来构造切线，所以没有必要百分百让矩阵完美地和平面贴合。
        vec3 tangent = normalize(random - kernelNormal * dot(random, kernelNormal));
        vec3 bitangent = cross(kernelNormal, tangent);
        mat3 TBN = mat3(tangent, bitangent, kernelNormal);

        // iterate over the sample kernel and calculate occlusion factor
        // 遍历每个核心采样，将采样从切线空间转化到视图空间，接着进行深度对比
        for(int i=0; i<sample_count; i++) {
            vec3 samplePos = kernelPos + TBN * u_smaple_kernels[i]; // from tangent to view-space 从切线空间转化到视图空间

            // project sample position (to sample texture) (to get position on screen/texture) 投影smple点到深度纹理坐标，获取在纹理的位置
            vec4 depth_uv = u_projection * vec4(samplePos, 1.0); // from view to clip-space 使用projection将其转化到裁剪空间
            depth_uv.xy /= depth_uv.w; // perspective divide
            depth_uv.xy = depth_uv.xy * 0.5 + 0.5; // transform to range 0.0 - 1.0
            
            // get sample depth 使用纹理坐标来采样G缓冲中的位置的z值来作为采样的深度
            float z = texture2D(u_position, depth_uv.xy).z; // get depth value of kernel sample

            // range check & accumulate 将当前的采样深度值和存储的深度值进行比较，如果大一些的话，添加遮蔽因数的影响。
            // 用范围检查，来确保某一片段的深度值在采样半径内，这样才会对遮蔽因数做影响。添加bias可以帮助调整环境光遮蔽的效果，也可以解决波纹问题。
            float dz = z - samplePos.z;
            if (dz > u_depth_bias[i]) {
                occlusion += smoothstep(0.0, 1.0, u_sample_radius[i] / dz);
            }
        }

//        occlusion = pow(occlusion, QUADRATIC);
        occlusion = occlusion / float(sample_count);
//        occlusion = CONTRAST * (occlusion - 0.5) + 0.5;

    }

#if 1
    gl_FragColor.r = occlusion;
#else
    float result = 1.0 - occlusion;
    gl_FragColor = vec4(result * .65, result * .85, result * 1.5, 1.);
#endif

}

)"; }


// kawase blur

static const char* blurFragmentShader() { return R"(

varying vec2 TexCoords;

uniform sampler2D u_ssao;
uniform float u_enable_blur;
uniform vec2 u_offset[3];

float kawaseBlurSample5(vec2 uv) {    
    float color = texture2D(u_ssao, uv).r;

    color += texture2D(u_ssao, uv + vec2(+u_offset[0].x, +u_offset[0].y)).r;
    color += texture2D(u_ssao, uv + vec2(+u_offset[0].x, -u_offset[0].y)).r;
    color += texture2D(u_ssao, uv + vec2(-u_offset[0].x, +u_offset[0].y)).r;
    color += texture2D(u_ssao, uv + vec2(-u_offset[0].x, -u_offset[0].y)).r;

    return color / 5.;
}

void main() {
    if (u_enable_blur > 0.) {
        float result = kawaseBlurSample5(TexCoords);
        gl_FragColor = vec4(0., 0., 0., result);
    } else {
        float result = texture2D(u_ssao, TexCoords).r;
        gl_FragColor = vec4(vec3(1.) * result, .8);
    }
}

)"; }


} // namespace ssao
} // namespace programs
} // nav
