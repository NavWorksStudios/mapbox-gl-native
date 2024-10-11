//
//  nav.ssao.cpp
//
//  Created by BernieZhao on 2024/9/1.
//

// intro
// https://learnopengl.com/Advanced-Lighting/SSAO
// source code
// https://learnopengl.com/code_viewer_gh.php?code=src/5.advanced_lighting/9.ssao/ssao.cpp


#include "mbgl/nav/render/ssao/nav.ssao.hpp"

#include <random>

#include "mbgl/nav/render/nav.render.hpp"
#include "mbgl/nav/render/vec3.h"
#include "mbgl/nav/render/shaders.h"

#include <mbgl/programs/gl/nav.ssao.shader.hpp>


namespace nav {
namespace ssao {


namespace sample {

// generates random floats between 0.0 and 1.0
std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0);
std::default_random_engine generator;

namespace kernel {

enum { SIZE = 16, };
Vec3 data[SIZE];

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
        data[i] = sample;
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

void generate() {
    kernel::generate();
    noise::generate();
}

}


namespace gbuffer {

GLuint fbo = 0;
GLuint position = 0;
GLuint normal = 0;
GLuint albedo = 0;
GLuint rboDepth = 0;

void generate(int width, int height) {
    if (!fbo) glGenFramebuffers(1, &fbo);

    // position color buffer
    glDeleteTextures(1, &position);
    position = nav::render::util::genTexture(GL_RGB16F, width, height, GL_RGB, GL_FLOAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // normal color buffer
    glDeleteTextures(1, &normal);
    normal = nav::render::util::genTexture(GL_RGB16F, width, height, GL_RGB, GL_FLOAT);

    // color + specular color buffer
    glDeleteTextures(1, &albedo);
    albedo = nav::render::util::genTexture(GL_RGB16F, width, height, GL_RGB, GL_FLOAT);

    // create and attach depth buffer (renderbuffer)
    glDeleteRenderbuffers(1, &rboDepth);
    glGenRenderbuffers(1, &rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
}

void bindFbo(GLint shadow) {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, position, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, normal, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, albedo, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, shadow, 0);
    
    GLenum attachments[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
    glDrawBuffers(4, attachments);
    
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
}

}


namespace ao {

GLuint fbo = 0;
GLuint buffer = 0;

void generate(int width, int height) {
    if (!fbo) glGenFramebuffers(1, &fbo);
    
    glDeleteTextures(1, &buffer);
    buffer = nav::render::util::genTexture(GL_RED, width, height, GL_RED, GL_FLOAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void bindFbo() {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, buffer, 0);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);
}

GLuint program() {
    static GLuint pass = 0;
    if (!pass) {
        pass =
        createProgram(compileShader(GL_VERTEX_SHADER, nav::programs::ssao::vertexShader()),
                      compileShader(GL_FRAGMENT_SHADER, nav::programs::ssao::genSSAOFragmentShader()));
    }

    return pass;
}

}


void renderGeoAndShadowBuffer(const Mat4& lightMatrix, GLint shadowDepth, std::function<void()> renderCallback, std::function<void()> bindScreen) {
    if (bindScreen) bindScreen();
    else gbuffer::bindFbo(ao::buffer);
    
    glDisable(GL_BLEND);
    
    GLfloat color[4];
    glGetFloatv(GL_COLOR_CLEAR_VALUE, color);
    
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    static GLint program = 0;
    if (program) {
        static programs::UniformLocation u0(program, "u_lightMatrix");
        glUniformMatrix4fv(u0, 1, GL_FALSE, reinterpret_cast<const float*>(&lightMatrix));

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, shadowDepth);
        static programs::UniformLocation u2(program, "u_shadowMap");
        glUniform1i(u2, 0);
    }
    
    renderCallback();
    
    glGetIntegerv(GL_CURRENT_PROGRAM, &program);
    glClearColor(color[0], color[1], color[2], color[3]);
}


GLint renderAOBuffer(int width, int height, float zoom, const Mat4& projMatrix, std::function<void()> bindScreen) {
    if (bindScreen) bindScreen();
    else ao::bindFbo();

    glClear(GL_COLOR_BUFFER_BIT);

    const GLint program = ao::program();

    glUseProgram(program);
    
    {
        for (unsigned int i = 0; i < sample::kernel::SIZE; ++i) {
            const float z = zoom - 15.; // zoom (15, 20)
            const float z_scale = pow(2., z);
            const float z_factor = fmin(fmax(z / 5., 0.), 1.);
            const float scalar = 0.03 * z_scale * pow(1.25 - z_factor * 0.2, i);
            
            programs::UniformLocation u0(program, ("u_sample_radius[" + std::to_string(i) + "]").c_str());
            glUniform1fv(u0, 1, &scalar);

            programs::UniformLocation u1(program, ("u_z_bias[" + std::to_string(i) + "]").c_str());
            glUniform1fv(u1, 1, &scalar);
        
            // Send kernel + rotation
            Vec3 v = sample::kernel::data[i].scale(scalar);
            programs::UniformLocation u2(program, ("u_samples[" + std::to_string(i) + "]").c_str());
            glUniform3fv(u2, 1, &v.x);
        }
    }

    {
        static programs::UniformLocation u0(program, "u_projection");
        glUniformMatrix4fv(u0, 1, GL_FALSE, reinterpret_cast<const float*>(&projMatrix));
        
        static programs::UniformLocation u1(program, "u_texscale");
        glUniform2f(u1, width / sample::noise::SIZE, height / sample::noise::SIZE);
    }

    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gbuffer::position);
        static programs::UniformLocation u0(program, "u_position");
        glUniform1i(u0, 0);
        
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, gbuffer::normal);
        static programs::UniformLocation u1(program, "u_normal");
        glUniform1i(u1, 1);
        
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, gbuffer::albedo);
        static programs::UniformLocation u2(program, "u_albedo");
        glUniform1i(u2, 2);
        
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, sample::noise::texture);
        static programs::UniformLocation u3(program, "u_noise");
        glUniform1i(u3, 3);
    }
    
    nav::render::util::renderQuad(program);

    return ao::buffer;
}


void initResource(int width, int height) {
    static std::once_flag flag;
    std::call_once(flag, [] () {
        sample::generate();
    });
    
    static int w = 0, h = 0;
    if (w != width || h != height) {
        w = width;
        h = height;
        
        gbuffer::generate(w, h);
        ao::generate(w, h);
    }
}


}

}
