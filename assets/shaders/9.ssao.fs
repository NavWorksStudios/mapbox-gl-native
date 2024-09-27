
varying vec2 TexCoords;

uniform mat4 u_projection;
uniform vec2 u_text_scale;
uniform float u_zoom_scale;

uniform sampler2D u_position;
uniform sampler2D u_normal;
uniform sampler2D u_noise;

#define SAMPLE_SIZE 32
uniform vec3 u_samples[SAMPLE_SIZE];

const float QUADRATIC = 1.5;
const float CONTRAST = 1.2;

void main()
{
    vec3 kernelPos = texture2D(u_position, TexCoords).xyz;



    // 动态采样数 - 降低画面采样数，近多远少
    // 计算z_scale
    const float NEAR_Z = 0.;
    const float FAR_Z = -200.;
    float z_scale = (FAR_Z - kernelPos.z) / (FAR_Z - NEAR_Z);
    // 计算sample count
    float sample_count_countinous = float(SAMPLE_SIZE) * z_scale;
    int sample_count = int(ceil(sample_count_countinous));
    if (sample_count < 1 || sample_count > SAMPLE_SIZE) {
        gl_FragColor.r = 1.;
        return;
    }



    // 动态半径偏差 - 有效降低波纹，提高画面对比度
    // 近处半径大，远处半径小
    float scale = u_zoom_scale * z_scale;
    float SAMPLE_RADIUS = 0.2 * scale;
    float Z_BIAS = 0.2 * scale;
    // 近处差距大，更强对比度；远处差距归零，避免波纹
    float SAMPLE_RADIUS_PROGRESS = 1.3;
    float Z_BIAS_PROGRESS = 1.3 - 0.3 * max(z_scale - .2, 0.);


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
            // 动态采样数 - 规避近处采样数阶跃，以保持采样数连续，防止波纹
            float z_compensation = min(1., sample_count_countinous - float(i));

            occlusion += smoothstep(0.0, 1.0, SAMPLE_RADIUS / dz) * z_compensation;
        }

        // dynamic sample radius
        SAMPLE_RADIUS *= SAMPLE_RADIUS_PROGRESS;
        Z_BIAS *= Z_BIAS_PROGRESS;
    }

    occlusion = pow(occlusion, QUADRATIC);

    occlusion /= float(sample_count);

    occlusion = CONTRAST * (occlusion - 0.5) + 0.5;

    float result = 1.0 - occlusion;

    gl_FragColor.r = result;
    
    // gl_FragColor = vec4(result * .65, result * .85, result * 1.5, 1.);

    // gl_FragColor = vec4( vec3(result), 1.);

}
