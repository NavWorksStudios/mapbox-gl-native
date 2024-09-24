
varying vec2 TexCoords;

uniform mat4 u_projection;
uniform vec2 u_text_size;
uniform float u_zoom_scale;

uniform sampler2D u_position;
uniform sampler2D u_normal;
uniform sampler2D u_noise;

// parameters (you'd probably want to use them as uniforms to more easily tweak the effect)
#define SAMPLE_SIZE 8
uniform vec3 u_samples[SAMPLE_SIZE];

// const float QUADRATIC = 1.;
// const float CONTRAST = 1.;

void main()
{
    // get input for SSAO algorithm
    vec3 kernelPos = texture2D(u_position, TexCoords).xyz;
    vec3 kernelNormal = normalize(texture2D(u_normal, TexCoords).xyz);

    // tile noise texture over screen based on screen dimensions divided by noise size
    vec3 random = normalize(texture2D(u_noise, TexCoords * u_text_size / 4.0).xyz);

    // radius
    float scale = u_zoom_scale;// * (100. / kernelPos.z);
    float SAMPLE_RADIUS = 0.4 * scale; // 采样球半径
    float Z_MIN_DIFFERENCE = 0.02 * scale;

    // create TBN change-of-basis matrix: from tangent-space to view-space
    // 使用Gramm-Schmidt方法我们可以创建正交的TBN矩，同时使用random进行偏移。
    // 注意因为我们使用的是随机向量来构造切线，所以没有必要百分百让矩阵完美地和平面贴合。
    vec3 tangent = normalize(random - kernelNormal * dot(random, kernelNormal));
    vec3 bitangent = cross(kernelNormal, tangent);
    mat3 TBN = mat3(tangent, bitangent, kernelNormal);

    // iterate over the sample kernel and calculate occlusion factor
    // 遍历每个核心采样，将采样从切线空间转化到视图空间，接着进行深度对比
    float occlusion = 0.0;

    float weight = 0.0;

    for(int i = 0; i < SAMPLE_SIZE; i += 1)
    {
        // ============ 采样点 视图空间坐标 ==============

        // get sample position
        vec3 samplePos = TBN * u_samples[i]; // from tangent to view-space 从切线空间转化到视图空间
        samplePos = kernelPos + samplePos * SAMPLE_RADIUS;

        // ============ 采样点 深度纹理坐标 ==============
        
        // project sample position (to sample texture) (to get position on screen/texture)
        // 投影smple点到深度纹理坐标，获取在纹理的位置
        vec4 depth_uv = u_projection * vec4(samplePos, 1.0); // from view to clip-space 使用projection将其转化到裁剪空间
        depth_uv.xyz /= depth_uv.w; // perspective divide
        depth_uv.xyz = depth_uv.xyz * 0.5 + 0.5; // transform to range 0.0 - 1.0
        
        // ============ 采样点 z ==============

        // get sample depth
        // 使用纹理坐标来采样G缓冲中的位置的z值来作为采样的深度
        float z = texture2D(u_position, depth_uv.xy).z; // get depth value of kernel sample

        // ============ 比较 采样核心z 和 该采样点z ==============

        // 用范围检查，来确保某一片段的深度值在采样半径内，这样才会对遮蔽因数做影响。
        // 添加bias可以帮助调整环境光遮蔽的效果，也可以解决痤疮问题。
        if (samplePos.z + Z_MIN_DIFFERENCE <= z) {
            // range check & accumulate
            // 将当前的采样深度值和存储的深度值进行比较，如果大一些的话，添加遮蔽因数的影响。
            occlusion += smoothstep(0.0, 1.0, SAMPLE_RADIUS / abs(kernelPos.z - z));
        }

        SAMPLE_RADIUS *= 1.4;

    }

    // occlusion = pow(occlusion, QUADRATIC);

    occlusion /= float(SAMPLE_SIZE);

    // const vec2 FADE_OUT  = vec2(15., 25.);
    // float fadeout = clamp(0., 1., (-kernelPos.z - FADE_OUT[0]) / FADE_OUT[1]);
    // float alpha = .5 + .5 * fadeout;
    // occlusion *= alpha;

    // occlusion  = CONTRAST * (occlusion - 0.5) + 0.5;

    gl_FragColor.r = 1.0 - occlusion;
    
    // gl_FragColor = vec4(occlusion, occlusion, occlusion, 1.);

}
