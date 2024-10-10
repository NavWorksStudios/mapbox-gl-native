
namespace nav {
namespace programs {
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
uniform vec2 u_text_scale;

uniform sampler2D u_position;
uniform sampler2D u_normal;
uniform sampler2D u_albedo;
uniform sampler2D u_noise;

#define SAMPLE_SIZE 16
uniform float u_sample_radius[SAMPLE_SIZE];
uniform float u_z_bias[SAMPLE_SIZE];
uniform vec3 u_samples[SAMPLE_SIZE];
uniform float u_sample_factor;


const float QUADRATIC = 1.5;
//const float CONTRAST = 1.5;

const float NEAR_Z = 0.;
const float FAR_Z = -250.;
const float HIDE_Z = -200.;

void main() {

    float occlusion = 0.0;

    vec3 kernelPos = texture2D(u_position, TexCoords).xyz;
    vec3 albedo = texture2D(u_albedo, TexCoords).xyz;
    if (kernelPos.z > HIDE_Z || albedo.r > 0.) {

        // 动态采样数，近密远疏，可以大幅降低开销
        float z_scale = min(1., (FAR_Z - kernelPos.z) / (FAR_Z - NEAR_Z));
        int dynamic_sample_count = int(float(SAMPLE_SIZE) * z_scale);

        // get input for SSAO algorithm
        vec3 kernelNormal = texture2D(u_normal, TexCoords).xyz;
        vec3 random = texture2D(u_noise, TexCoords * u_text_scale).xyz;

        // create TBN change-of-basis matrix: from tangent-space to view-space
        // 使用Gramm-Schmidt方法我们可以创建正交的TBN矩，同时使用random进行偏移。
        // 注意因为我们使用的是随机向量来构造切线，所以没有必要百分百让矩阵完美地和平面贴合。
        vec3 tangent = normalize(random - kernelNormal * dot(random, kernelNormal));
        vec3 bitangent = cross(kernelNormal, tangent);
        mat3 TBN = mat3(tangent, bitangent, kernelNormal);

        // iterate over the sample kernel and calculate occlusion factor
        // 遍历每个核心采样，将采样从切线空间转化到视图空间，接着进行深度对比
        for(int i=0; i<dynamic_sample_count; i++) {
            vec3 samplePos = kernelPos + TBN * u_samples[i]; // from tangent to view-space 从切线空间转化到视图空间

            // project sample position (to sample texture) (to get position on screen/texture) 投影smple点到深度纹理坐标，获取在纹理的位置
            vec4 depth_uv = u_projection * vec4(samplePos, 1.0); // from view to clip-space 使用projection将其转化到裁剪空间
            depth_uv.xy /= depth_uv.w; // perspective divide
            depth_uv.xy = depth_uv.xy * 0.5 + 0.5; // transform to range 0.0 - 1.0
            
            // get sample depth 使用纹理坐标来采样G缓冲中的位置的z值来作为采样的深度
            float z = texture2D(u_position, depth_uv.xy).z; // get depth value of kernel sample

            // range check & accumulate 将当前的采样深度值和存储的深度值进行比较，如果大一些的话，添加遮蔽因数的影响。
            // 用范围检查，来确保某一片段的深度值在采样半径内，这样才会对遮蔽因数做影响。添加bias可以帮助调整环境光遮蔽的效果，也可以解决痤疮问题。
            float dz = z - samplePos.z;
            if (dz > u_z_bias[i]) {
                occlusion += u_sample_radius[i] / dz;
            }
        }

        occlusion = pow(occlusion * 1.5, QUADRATIC);
        occlusion = occlusion / float(dynamic_sample_count);
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



#if 0

// box blur

static const char* blurFragmentShader() { return R"(

varying vec2 TexCoords;

uniform sampler2D u_ssao;
uniform vec2 u_texsize;

void main() 
{
    float result = 0.0;

    for (int x = -2; x < 2; ++x) 
    {
        for (int y = -2; y < 2; ++y) 
        {
            vec2 offset = vec2(float(x), float(y)) / u_texsize;
            result += texture2D(u_ssao, TexCoords + offset).r;
        }
    }

    result /= (4.0 * 4.0);

    // gl_FragColor.r = result;
    gl_FragColor = vec4(.1 * 1.5, .1 * .85, .1 * .65, max(1. - result, 0.));

    // gl_FragColor = vec4( vec3(result), 1.); // 白色
    // gl_FragColor = vec4(result * 1.5, result * .9, result * 1.2, 1.); // 粉色
    // gl_FragColor = vec4(result * .65, result * .85, result * 1.5, 1.); // 蓝色
    // gl_FragColor = vec4(result * 1.5, result * .85, result * .65, 1.); // 黄色


    
}

)"; }

#else

// kawase blur

static const char* blurFragmentShader() { return R"(

varying vec2 TexCoords;

uniform sampler2D u_ssao;
uniform vec2 u_texsize;

float kawaseBlurSample(vec2 uv) {
    vec2 offset = vec2(1.1) / u_texsize;
    float color = texture2D(u_ssao, uv).r;
    color += texture2D(u_ssao, uv + vec2(+offset.x, +offset.y)).r;
    color += texture2D(u_ssao, uv + vec2(-offset.x, +offset.y)).r;
    color += texture2D(u_ssao, uv + vec2(-offset.x, -offset.y)).r;
    color += texture2D(u_ssao, uv + vec2(+offset.x, -offset.y)).r;
    return color * .2;
}

void main() {
    float result = kawaseBlurSample(TexCoords);
    gl_FragColor = vec4(.0, .0, .0, result);
}

)"; }

#endif



} // namespace ssao
} // namespace programs
} // nav
