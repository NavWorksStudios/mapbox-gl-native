
varying vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D texNoise;

uniform vec3 samples[64];

// parameters (you'd probably want to use them as uniforms to more easily tweak the effect)
const int SAMPLE_SIZE = 64;
const float SAMPLE_RADIUS = 0.15; // 采样球半径
const float Z_MIN_DIFFERENCE = 0.05;
const float QUADRATIC = 1.5;

// tile noise texture over screen based on screen dimensions divided by noise size
// 设置随机纹理坐标的缩放noiseScale
const vec2 noiseScale = vec2(2048.0/4.0, 1080.0/4.0); 

uniform mat4 projection;

void main()
{
    // get input for SSAO algorithm
    vec3 kernelPos = texture2D(gPosition, TexCoords).xyz;
    vec3 kernalNormal = texture2D(gNormal, TexCoords).xyz;
    vec3 random = texture2D(texNoise, TexCoords * noiseScale).xyz;

    // create TBN change-of-basis matrix: from tangent-space to view-space
    // 使用Gramm-Schmidt方法我们可以创建正交的TBN矩，同时使用random进行偏移。
    // 注意因为我们使用的是随机向量来构造切线，所以没有必要百分百让矩阵完美地和平面贴合。
    vec3 tangent = normalize(random - kernalNormal * dot(random, kernalNormal));
    vec3 bitangent = cross(kernalNormal, tangent);
    mat3 TBN = mat3(tangent, bitangent, kernalNormal);

    // iterate over the sample kernel and calculate occlusion factor
    // 遍历每个核心采样，将采样从切线空间转化到视图空间，接着进行深度对比
    float occlusion = 0.0;

    for(int i = 0; i < SAMPLE_SIZE; i++)
    {
        // ============ 采样点 视图空间坐标 ==============

        // get sample position
        vec3 tangentPos = TBN * samples[i]; // from tangent to view-space 从切线空间转化到视图空间
        vec3 samplePos = kernelPos + tangentPos * SAMPLE_RADIUS;

        // ============ 采样点 深度纹理坐标 ==============
        
        // project sample position (to sample texture) (to get position on screen/texture)
        // 投影smple到深度纹理坐标，获取在纹理的位置
        vec4 texCoors = projection * vec4(samplePos, 1.0); // from view to clip-space 使用projection将其转化到切割空间
        texCoors.xyz /= texCoors.w; // perspective divide
        texCoors.xyz = texCoors.xyz * 0.5 + 0.5; // transform to range 0.0 - 1.0
        
        // ============ 采样点 z ==============

        // get sample depth
        // 使用纹理坐标来采样G缓冲中的位置的z值来作为采样的深度
        float z = texture2D(gPosition, texCoors.xy).z; // get depth value of kernel sample

        // ============ 比较 采样核心z 和 该采样点z ==============

        // 用范围检查，来确保某一片段的深度值在采样半径内，这样才会对遮蔽因数做影响。
        // 添加bias可以帮助调整环境光遮蔽的效果，也可以解决痤疮问题。
        if (z - kernelPos.z > Z_MIN_DIFFERENCE) {
            // range check & accumulate
            // 将当前的采样深度值和存储的深度值进行比较，如果大一些的话，添加遮蔽因数的影响。
            occlusion += smoothstep(0.0, 1.0, SAMPLE_RADIUS / abs(z - kernelPos.z));
        }

    }

    occlusion = 1.0 - occlusion / float(SAMPLE_SIZE);
    
    gl_FragColor = vec4(vec3(pow(occlusion, QUADRATIC)), 1.);
}
