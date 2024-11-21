//
//  nav.ssao.cpp
//
//  Created by BernieZhao on 2024/9/1.
//

// intro
// https://learnopengl.com/Advanced-Lighting/SSAO
// source code
// https://learnopengl.com/code_viewer_gh.php?code=src/5.advanced_lighting/9.ssao/ssao.cpp


#include "mbgl/nav/render/nav.ssao.hpp"

#include "mbgl/nav/render/vec3.h"
#include "mbgl/nav/render/shaders.h"
#include "mbgl/nav/render/nav.quad.hpp"
#include "mbgl/nav/render/programs/nav.program.ssao.hpp"
#include "mbgl/nav/render/nav.glvalue.hpp"

#include <random>
#include <array>
#include <mutex>

namespace nav {
namespace render {

namespace sample {

// generates random floats between 0.0 and 1.0
std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0);
std::default_random_engine generator;

namespace kernel {

enum { SIZE = 8, };
Vec3 random[SIZE];

GLfloat lerp(GLfloat a, GLfloat b, GLfloat f) {
    return a + f * (b - a);
}

// 生成一个沿法线方向的半球形采样核心，将在切线空间生成这个半球（法线都指向+z轴），以免为每个平面都单独生成一个沿各自法线方向的半球。
// - generate sample kernel
void generate() {
    for (int i=0; i<SIZE; ++i) {
        Vec3 sample(randomFloats(generator) * 2.0 - 1.0, // x, -1.0到1.0
                    randomFloats(generator) * 2.0 - 1.0, // y, -1.0到1.0
                    randomFloats(generator) );           // z, 0到1
        
        sample.normalize();
        sample.scale(randomFloats(generator));
        float scale = float(i) / SIZE;
        // scale samples s.t. they're more aligned to center of kernel
        scale = lerp(0.1f, 1.0f, scale * scale);
        sample.scale(scale);
        random[i] = sample;
    }
}

}

namespace noise {

enum { SIZE = 4, };
Vec3 data[SIZE * SIZE];
GLuint texture = 0;

// 随机核心旋转 - Noise texture
// 将每个核心的随机旋转存储在纹理中。创建一个4*4阵列的随即旋转向量，绕着切线空间的法线：
void generate() {
    for (int i=0; i<SIZE*SIZE; i++) {
        Vec3 noise(randomFloats(generator) * 2.0 - 1.0,
                   randomFloats(generator) * 2.0 - 1.0,
                   0.0f); //基于屏幕空间，z为0
        
        noise.normalize();
        data[i] = noise;
    }
    
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, SIZE, SIZE, 0, GL_RGB, GL_FLOAT, &data[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

}

}


namespace ssao {

GLuint fbo = 0;
GLuint buffer = 0;

GLuint program() {
    static GLuint pass = createProgram(compileShader(GL_VERTEX_SHADER, nav::programs::ssao::vertexShader()),
                                       compileShader(GL_FRAGMENT_SHADER, nav::programs::ssao::fragmentShader()));
    return pass;
}

void initResource(int width, int height) {
    static std::once_flag flag;
    std::call_once(flag, [] () {
        sample::kernel::generate();
        sample::noise::generate();
    });
    
    if (!fbo) glGenFramebuffers(1, &fbo);
}

void bindFbo(GLuint buffer) {
    glBindFramebuffer(GL_FRAMEBUFFER, ssao::fbo);
    if (ssao::buffer != buffer) {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssao::buffer = buffer, 0);
        glDrawBuffer(GL_COLOR_ATTACHMENT0);
    }
}

void render(uint32_t width, uint32_t height, float zoom, const Mat4& projMatrix,
            GLuint renderBuffer, GLuint haloBuffer, const std::array<GLuint, 3>& gbuffer) {
    initResource(width, height);
    
    gl::Value<BindFramebuffer> bindFramebuffer;
    gl::Value<Viewport> viewport;
    gl::Value<Program> program;
    gl::Value<Blend> blend;
    
    {
        bindFbo(renderBuffer);
        Viewport::Set({0, 0, { width, height }});
        Blend::Set(true);
        
        const GLint program = ssao::program();
        Program::Set(program);
        
        {
            static GLint u_smaple_kernels[sample::kernel::SIZE];
            static GLint u_sample_radius[sample::kernel::SIZE];
            static GLint u_depth_bias[sample::kernel::SIZE];
            
            static std::once_flag flag;
            std::call_once(flag, [program] () {
                for (unsigned int i = 0; i < sample::kernel::SIZE; ++i) {
                    std::string num = "[" + std::to_string(i) + "]";
                    
                    u_smaple_kernels[i] = programs::UniformLocation(program, ("u_smaple_kernels" + num).c_str());
                    u_sample_radius[i] = programs::UniformLocation(program, ("u_sample_radius" + num).c_str());
                    u_depth_bias[i] = programs::UniformLocation(program, ("u_depth_bias" + num).c_str());
                }
            });
            
            const float radius = .1;
            const float bias = .0001;
            for (int i=0; i<sample::kernel::SIZE; i++) {
                const float scale = pow(2., i);
                const Vec3 v = sample::kernel::random[i].scale(radius * scale);
                glUniform3f(u_smaple_kernels[i], v.x, v.y, v.z);
                glUniform1f(u_sample_radius[i], radius * scale);
                glUniform1f(u_depth_bias[i], bias * scale);
            }
            
            static programs::UniformLocation u0(program, "u_projection");
            glUniformMatrix4fv(u0, 1, GL_FALSE, reinterpret_cast<const float*>(&projMatrix));
            
            static programs::UniformLocation u1(program, "u_noise_uv_scale");
            glUniform2f(u1, (float) width / sample::noise::SIZE, (float) height / sample::noise::SIZE);
            
        }
        
        {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, gbuffer[0]);
            static programs::UniformLocation u0(program, "u_position");
            glUniform1i(u0, 0);
            
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, gbuffer[1]);
            static programs::UniformLocation u1(program, "u_normal");
            glUniform1i(u1, 1);
            
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, gbuffer[2]);
            static programs::UniformLocation u2(program, "u_albedo");
            glUniform1i(u2, 2);
            
            glActiveTexture(GL_TEXTURE3);
            glBindTexture(GL_TEXTURE_2D, sample::noise::texture);
            static programs::UniformLocation u3(program, "u_noise");
            glUniform1i(u3, 3);
        }

        quad::render(program);
    }

}

}

}
}
