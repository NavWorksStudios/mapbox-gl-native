//
//  nav.shader.cpp
//
//  Created by BernieZhao on 2024/9/1.
//

#include "mbgl/nav/nav.shader.hpp"


// 1. Geometry Pass: render scene's geometry/color data into gbuffer
// Shader shaderGeometryPass(
// "C:/Users/Administrator/Desktop/OpenGL/OpenGL/vertexSource.vert",    "C:/Users/Administrator/Desktop/OpenGL/OpenGL/fragmentSource.frag");

// 2. 使用G缓冲渲染SSAO纹理
// Shader shaderSSAO(
// "C:/Users/Administrator/Desktop/OpenGL/OpenGL/vs.vert",              "C:/Users/Administrator/Desktop/OpenGL/OpenGL/ssao.frag");

// 3. Blur SSAO texture to remove noise
// Shader shaderSSAOBlur(
// "C:/Users/Administrator/Desktop/OpenGL/OpenGL/vs.vert",              "C:/Users/Administrator/Desktop/OpenGL/OpenGL/ssao_blur.frag");

// 4.Lighting Pass：传统的延迟Blinn-Phong照明现在增加了屏幕空间环境遮蔽
// Shader shaderLightingPass(
// "C:/Users/Administrator/Desktop/OpenGL/OpenGL/vs.vert",              "C:/Users/Administrator/Desktop/OpenGL/OpenGL/fs.frag");


namespace nav {

namespace shader {


/****************************** G-buffer vs（顶点传入观察空间）******************************/

const char* vs_vert() { return R"(

#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;

out vec3 FragPos;
out vec2 TexCoords;
out vec3 Normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    vec4 viewPos = view * model * vec4(position, 1.0f);
    FragPos = viewPos.xyz;
    gl_Position = projection * viewPos;
    TexCoords = texCoords;
    
    mat3 normalMatrix = transpose(inverse(mat3(view * model)));
    Normal = normalMatrix * normal;
}

)"; }


/****************************** G-buffer fs ******************************/

const char* geometry_frag() { return R"(

#version 330 core
layout (location = 0) out vec4 gPositionDepth;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

const float NEAR = 0.1; // 投影矩阵的近平面
const float FAR = 50.0f; // 投影矩阵的远平面

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // Back to NDC
    return (2.0 * NEAR * FAR) / (FAR + NEAR - z * (FAR - NEAR));
}

void main()
{
     // 储存片段的位置矢量到第一个G缓冲纹理
    gPositionDepth.xyz = FragPos;
    // 储存线性深度到gPositionDepth的alpha分量
    gPositionDepth.a = LinearizeDepth(gl_FragCoord.z);
     // 储存法线信息到G缓冲
    gNormal = normalize(Normal);
   // 和漫反射颜色
    gAlbedoSpec.rgb = vec3(0.95);
}

)"; }

/****************************** ssao fs ******************************/

const char* SSAO_frag() { return R"(

#version 330 core
out float FragColor;
in vec2 TexCoords;

uniform sampler2D gPositionDepth;
uniform sampler2D gNormal;
uniform sampler2D texNoise;

uniform vec3 samples[64];

int kernelSize = 64;
float radius = 1.0;

// 屏幕的平铺噪声纹理会根据屏幕分辨率除以噪声大小的值来决定
const vec2 noiseScale = vec2(800.0f/4.0f, 600.0f/4.0f);

uniform mat4 projection;

void main()
{
    // 采样G-buffer
    vec3 fragPos = texture(gPositionDepth, TexCoords).xyz;
    vec3 normal = texture(gNormal, TexCoords).rgb;
    vec3 randomVec = texture(texNoise, TexCoords * noiseScale).xyz; //随机向量

    // 创建 TBN 矩阵：从切线空间到观察空间
    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal)); //用随机向量来构造切线
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);

    //迭代样本核并计算遮挡因子
    float occlusion = 0.0;
    for(int i = 0; i < kernelSize; ++i)
    {
        // 采样存储的顶点位置（观察空间）
        vec3 sample = TBN * samples[i]; // 从切线空间到观察空间
        sample = fragPos + sample * radius; //样本=位置 + 样本*半径
        
        // 生成屏幕UV
        vec4 offset = vec4(sample, 1.0);
        offset = projection * offset; // 观察->裁剪空间
        offset.xyz /= offset.w; // 齐次除法
        offset.xyz = offset.xyz * 0.5 + 0.5; // NDC(0~1)
        // 采样深度值
        float sampleDepth = -texture(gPositionDepth, offset.xy).w; //获取基于屏幕UV的深度值，offset.xy表示屏幕
        
        // 范围检查和生成AO
        float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPos.z - sampleDepth )); //范围检测
        occlusion += (sampleDepth >= sample.z ? 1.0 : 0.0) * rangeCheck; //当前深度值（sampleDepth）与存储的深度值(sample.z) 对比
    }
    occlusion = 1-(occlusion / kernelSize); //对AO根据核心的大小标准化

    //结果
    FragColor = occlusion; //FragColor以float传出
}

)"; }

/****************************** ssao_blur.fs ******************************/

const char* SSAOBlur_frag() { return R"(

#version 330 core
in vec2 TexCoords;

out float fragColor;

uniform sampler2D ssaoInput;//ssaoColorBuffer
const int blurSize = 4;

void main()
{
   vec2 texelSize = 1.0 / vec2(textureSize(ssaoInput, 0)); // 纹素大小[-2~2]
   float result = 0.0;
   for (int x = 0; x < blurSize; ++x) // X方向偏移4次
   {
      for (int y = 0; y < blurSize; ++y) // Y方向偏移4次
      {
         vec2 offset = (vec2(-2.0) + vec2(float(x), float(y))) * texelSize;
         result += texture(ssaoInput, TexCoords + offset).r;
      }
   }
 
   fragColor = result / float(blurSize * blurSize); //ssaoBlurFBO
}

)"; }


/****************************** light.fs ******************************/

const char* lighting_frag() { return R"(

#version 330 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D gPositionDepth;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;
uniform sampler2D ssao;

struct Light {
    vec3 Position;
    vec3 Color;

    float Linear;
    float Quadratic;
};

uniform Light light;

void main()
{
    // 从G缓冲中提取数据
    vec3 FragPos = texture(gPositionDepth, TexCoords).rgb;
    vec3 Normal = texture(gNormal, TexCoords).rgb;
    vec3 Diffuse = texture(gAlbedo, TexCoords).rgb;
    float AmbientOcclusion = texture(ssao, TexCoords).r;
    
    // Blinn-Phong (观察空间中)
    vec3 ambient = vec3(0.3 * AmbientOcclusion); // 这里我们加上遮蔽因子
    vec3 lighting = ambient;
    vec3 viewDir = normalize(-FragPos); // Viewpos 为 (0.0.0)，在观察空间中
    // Diffuse
    vec3 lightDir = normalize(light.Position - FragPos);
    vec3 diffuse = max(dot(Normal, lightDir), 0.0) * Diffuse * light.Color;
    // Specular
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(Normal, halfwayDir), 0.0), 8.0);
    vec3 specular = light.Color * spec;
    // Attenuation
    float distance = length(light.Position - FragPos);
    float attenuation = 1.0 / (1.0 + light.Linear * distance + light.Quadratic * distance * distance);
    diffuse *= attenuation;
    specular *= attenuation;
    lighting += diffuse + specular;

    FragColor = vec4(lighting, 1.0);
}


)"; }


}

}
