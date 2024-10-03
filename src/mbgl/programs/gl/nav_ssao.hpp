
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
uniform float u_zoom_scale;

uniform sampler2D u_position;
uniform sampler2D u_normal;
uniform sampler2D u_noise;

#define SAMPLE_SIZE 12
uniform vec3 u_samples[SAMPLE_SIZE];

// const float QUADRATIC = 1.;
// const float CONTRAST = 1.5;


void main()
{
    // get input for SSAO algorithm
    vec3 kernelPos = texture2D(u_position, TexCoords).xyz;
    vec3 kernelNormal = texture2D(u_normal, TexCoords).xyz;
    vec3 random = texture2D(u_noise, TexCoords * u_text_scale).xyz;

#if 1
    // 动态采样数 - 降低画面采样数，近多远少
    // 计算z_scale
    const float NEAR_Z = 0.;
    const float FAR_Z = -200.;
    float z_scale = (FAR_Z - kernelPos.z) / (FAR_Z - NEAR_Z);
    int sample_count = int(max(float(SAMPLE_SIZE) * z_scale, 4.));
#else
    int sample_count = SAMPLE_SIZE;
#endif

    float SAMPLE_RADIUS = 0.2 * u_zoom_scale;
    float Z_BIAS = 0.2 * u_zoom_scale;

    // create TBN change-of-basis matrix: from tangent-space to view-space
    // 使用Gramm-Schmidt方法我们可以创建正交的TBN矩，同时使用random进行偏移。
    // 注意因为我们使用的是随机向量来构造切线，所以没有必要百分百让矩阵完美地和平面贴合。
    vec3 tangent = normalize(random - kernelNormal * dot(random, kernelNormal));
    vec3 bitangent = cross(kernelNormal, tangent);
    mat3 TBN = mat3(tangent, bitangent, kernelNormal);

    // iterate over the sample kernel and calculate occlusion factor
    // 遍历每个核心采样，将采样从切线空间转化到视图空间，接着进行深度对比
    float occlusion = 0.0;

    for(int i=0; i<sample_count; i++)
    {
        // get sample position
        vec3 samplePos = TBN * u_samples[i]; // from tangent to view-space 从切线空间转化到视图空间
        samplePos = kernelPos + samplePos * SAMPLE_RADIUS;

        // project sample position (to sample texture) (to get position on screen/texture)
        // 投影smple点到深度纹理坐标，获取在纹理的位置
        vec4 depth_uv = u_projection * vec4(samplePos, 1.0); // from view to clip-space 使用projection将其转化到裁剪空间
        depth_uv.xy /= depth_uv.w; // perspective divide
        depth_uv.xy = depth_uv.xy * 0.5 + 0.5; // transform to range 0.0 - 1.0
        
        // get sample depth 使用纹理坐标来采样G缓冲中的位置的z值来作为采样的深度
        float z = texture2D(u_position, depth_uv.xy).z; // get depth value of kernel sample

        // range check & accumulate
        // 将当前的采样深度值和存储的深度值进行比较，如果大一些的话，添加遮蔽因数的影响。
        // 用范围检查，来确保某一片段的深度值在采样半径内，这样才会对遮蔽因数做影响。
        // 添加bias可以帮助调整环境光遮蔽的效果，也可以解决痤疮问题。
        float dz = z - samplePos.z;
        if (dz > Z_BIAS) {
            occlusion += SAMPLE_RADIUS / dz;
        }

        // dynamic sample radius
        SAMPLE_RADIUS *= 1.4;
        Z_BIAS *= 1.4;
    }

    // occlusion = pow(occlusion, QUADRATIC);

    occlusion /= float(sample_count);

    // occlusion = CONTRAST * (occlusion - 0.5) + 0.5;

    float result = 1.0 - occlusion * 2.;

    gl_FragColor.r = result;
    
    // gl_FragColor = vec4(result * .65, result * .85, result * 1.5, 1.);

    // gl_FragColor = vec4( vec3(result), 1.);

}

)"; }

static const char* blurFragmentShader() { return R"(

#if 1

varying vec2 TexCoords;

uniform sampler2D u_ssao;
uniform vec2 u_text_size;

void main() 
{
    float result = 0.0;

    for (int x = -2; x < 2; ++x) 
    {
        for (int y = -2; y < 2; ++y) 
        {
            vec2 offset = vec2(float(x), float(y)) / u_text_size;
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

#endif


#if 0

uniform sampler2D ssao;
varying vec2 TexCoords;

const vec2 texelSize = 1.0 / vec2(2048., 1080.) / 2.;

#define SEG 2

void main()
{
    float color = 0.0;
    float total = 0.0;

    for(int i = -SEG; i <= SEG; ++i)
    {
        for(int j = -SEG; j <= SEG; ++j)
        {
            float w = (1.1 - sqrt(float(i*i + j*j)) / 8.0);
            w *= w;

            vec2 offset = vec2(float(i), float(j)) * texelSize;
            color += texture2D(ssao, TexCoords + offset).r * w;

            total += w;
        }
    }

    color /= total;

    gl_FragColor.r = color;
}

#endif

)"; }

} // namespace ssao
} // namespace programs
} // nav
