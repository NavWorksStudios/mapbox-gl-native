//
//  nav.ssao.cpp
//
//  Created by BernieZhao on 2024/9/1.
//

#include "mbgl/nav/ssao/v2/nav.ssao.hpp"
#include <mbgl/programs/fill_extrusion_ssao_program.hpp>

#include <random>

#include "mbgl/nav/ssao/v1/shaders.h"
#include "mbgl/nav/ssao/v1/vec3.h"
#include "mbgl/nav/ssao/v1/mat4.h"

#include "mbgl/nav/nav.style.hpp"
#include "mbgl/nav/nav.palette.hpp"


namespace nav {
namespace ssao {


namespace sample {

// generates random floats between 0.0 and 1.0
std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0);
std::default_random_engine generator;

namespace kernel {

enum { SIZE = 12, };
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
//    scale::generate();
    noise::generate();
}

}


namespace fbo {

GLuint genTexture(GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type,
                  std::function<void()> configTexture=nullptr) {
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    if (configTexture) configTexture();

    return texture;
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
    position = genTexture(GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, [] () {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    });

    // normal color buffer
    glDeleteTextures(1, &normal);
    normal = genTexture(GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT);

    // color + specular color buffer
    glDeleteTextures(1, &albedo);
    albedo = genTexture(GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT);

    // create and attach depth buffer (renderbuffer)
    glDeleteRenderbuffers(1, &rboDepth);
    glGenRenderbuffers(1, &rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
}

void bind() {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, position, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, normal, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, albedo, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
    
    // tell OpenGL which color attachments we'll use (of this framebuffer) for rendering
    unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    glDrawBuffers(3, attachments);
}

}


namespace ssao {
GLuint fbo = 0;
GLuint buffer = 0;

void generate(int width, int height) {
    if (!fbo) glGenFramebuffers(1, &fbo);
    
    glDeleteTextures(1, &buffer);
    buffer = genTexture(GL_RED, width, height, 0, GL_RED, GL_FLOAT);
}

void bind() {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, buffer, 0);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);
}

}

namespace blur {
GLuint fbo = 0;
GLuint buffer = 0;

void generate(int width, int height) {
    if (!fbo) glGenFramebuffers(1, &fbo);
    
    glDeleteTextures(1, &buffer);
    buffer = genTexture(GL_RED, width, height, 0, GL_RED, GL_FLOAT);
}

void bind() {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, buffer, 0);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);
}

}


void generate(int width, int height) {
    static int w = 0, h = 0;
    if (w != width || h != height) {
        w = width;
        h = height;
        
        // Enable and configure textures on applicable texture units
        glActiveTexture(GL_TEXTURE0);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        
        gbuffer::generate(w, h);
        ssao::generate(w, h);
        blur::generate(w, h);
    }
}

}


namespace shader {

GLint floorPass;
GLuint aoPass;
GLint blurPass;
GLint renderNDCPass;

void load() {
    // #*# 加载floor shader

    floorPass =
    createProgram(compileShader(GL_VERTEX_SHADER, mbgl::vertexShader()),
                  compileShader(GL_FRAGMENT_SHADER, mbgl::fragmentShader()));
    
    aoPass =
    createProgram(loadShader(GL_VERTEX_SHADER, "/shaders/9.ssao.vs"),
                  loadShader(GL_FRAGMENT_SHADER, "/shaders/9.ssao.fs"));
    
    blurPass =
    createProgram(loadShader(GL_VERTEX_SHADER, "/shaders/9.ssao.vs"),
                  loadShader(GL_FRAGMENT_SHADER, "/shaders/9.ssao_blur.fs"));
}

}


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


// renderQuad() renders a 1x1 XY quad in NDC
// -----------------------------------------

void renderQuad(GLint program) {
    static GLuint quadVAO = 0;
    static GLuint quadVBO;
    
    if (quadVAO == 0)
    {
        float quadVertices[] = {
            // positions            // texture Coords
            -1.0f,  1.0f, 0.0f,     0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f,     0.0f, 0.0f,
             1.0f,  1.0f, 0.0f,     1.0f, 1.0f,
             1.0f, -1.0f, 0.0f,     1.0f, 0.0f,
        };
        
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);

        static AttribLocation a0(program, "aPos");
        glEnableVertexAttribArray(a0);
        glVertexAttribPointer(a0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        
        static AttribLocation a1(program, "aTexCoords");
        glEnableVertexAttribArray(a1);
        glVertexAttribPointer(a1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }

    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}



// 1. geometry pass: render scene's geometry/color data into gbuffer
// -----------------------------------------------------------------

void renderSceneToGBuffer(std::function<void()> renderCallback,
                          std::function<void()> bindScreenFbo=nullptr) {
    
    if (bindScreenFbo) bindScreenFbo();
    else fbo::gbuffer::bind();
    
    glDisable(GL_BLEND);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    renderCallback();
    
}

// 2. generate SSAO texture
// ------------------------
void generateSSAOTexture(float width, float height, float zoom,
                         const Mat4& projMatrix,
                         std::function<void()> bindScreenFbo=nullptr) {
    
    if (bindScreenFbo) bindScreenFbo();
    else fbo::ssao::bind();

    glClear(GL_COLOR_BUFFER_BIT);
    
    const GLint program = shader::aoPass;
    glUseProgram(program);
    
    {
        // Send kernel + rotation
        for (unsigned int i = 0; i < sample::kernel::SIZE; ++i) {
            UniformLocation u0(program, ("u_samples[" + std::to_string(i) + "]").c_str());
            glUniform3fv(u0, 1, reinterpret_cast<float*>(&(sample::kernel::data[i])));
        }
    }

    {
        static UniformLocation u0(program, "u_projection");
        glUniformMatrix4fv(u0, 1, GL_FALSE, reinterpret_cast<const float*>(&projMatrix));
        
        static UniformLocation u1(program, "u_text_scale");
        glUniform2f(u1, width / sample::noise::SIZE, height / sample::noise::SIZE);
        
        static UniformLocation u2(program, "u_zoom_scale");
        glUniform1f(u2, pow(2., zoom - 18.));
    }

    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, fbo::gbuffer::position);
        static UniformLocation u0(program, "u_position");
        glUniform1i(u0, 0);
        
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, fbo::gbuffer::normal);
        static UniformLocation u1(program, "u_normal");
        glUniform1i(u1, 1);
        
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, sample::noise::texture);
        static UniformLocation u2(program, "u_noise");
        glUniform1i(u2, 2);
    }
    
    renderQuad(program);

}

// 3. blur SSAO texture to screen
// ------------------------------------
void blurSSAOTexture(int width, int height,
                     std::function<void()> bindScreenFbo=nullptr) {
    
    if (bindScreenFbo) bindScreenFbo();
    else {
        fbo::blur::bind();
        glClear(GL_COLOR_BUFFER_BIT);
    }
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    const GLint program = shader::blurPass;
    glUseProgram(program);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, fbo::ssao::buffer);
    static UniformLocation u0(program, "u_ssao");
    glUniform1i(u0, 0);
    
    static UniformLocation u1(program, "u_text_size");
    glUniform2f(u1, width, height);
    
    renderQuad(program);
    
    glDisable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    
}

namespace v2 {

auto convertMatrix = [] (mbgl::mat4 matrix) {
    Mat4 m;
    for (int i=0; i<16; i++) ((float*)&m)[i] = float(matrix[i]);
    return m;
};


namespace floor {

GLuint tileFloorBuf = 0;

void init() {
    // Set up buffer for floor data, 6 triangles
    const static GLfloat tileFloorData[36] = {
        0.0f, 8192.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        8192.0f, 8192.0f,
        0.0f, 0.0f, 1.0f, 0.0f,

        8192.0f, 8192.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        8192.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f };
    
    glGenBuffers(1, &tileFloorBuf);
    glBindBuffer(GL_ARRAY_BUFFER, tileFloorBuf);
    glBufferData(GL_ARRAY_BUFFER, 36 * sizeof(GLfloat), tileFloorData, GL_STATIC_DRAW);
}

}


void renderTileFloor(const mbgl::mat4& mvp, const mbgl::mat4& mv, const mbgl::mat4& normal) {
    const GLint program = shader::floorPass;
    glUseProgram(program);
    
    // convert 3*matrix mbgl::mat4 to Mat4
    Mat4 MVP = convertMatrix(mvp);
    Mat4 MV = convertMatrix(mv);
    Mat4 NORMAL = convertMatrix(normal);
    
    // floor uniforms
    static UniformLocation u0(program, "u_matrix");
    glUniformMatrix4fv(u0, 1, GL_FALSE, reinterpret_cast<float*>(&MVP));
    
    static UniformLocation u1(program, "u_model_view_matrix");
    glUniformMatrix4fv(u1, 1, GL_FALSE, reinterpret_cast<float*>(&MV));
    
    static UniformLocation u2(program, "u_normal_matrix");
    glUniformMatrix4fv(u2, 1, GL_FALSE, reinterpret_cast<float*>(&NORMAL));
    
    // floor attributes
    glBindBuffer(GL_ARRAY_BUFFER, floor::tileFloorBuf);
    
    static AttribLocation a0(program, "a_pos");
    glEnableVertexAttribArray(a0);
    glVertexAttribPointer(a0, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void*>(0));
    
    static AttribLocation a1(program, "a_normal_ed");
    glEnableVertexAttribArray(a1);
    glVertexAttribPointer(a1, 4, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void*>(2*sizeof(GLfloat)));
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void draw(float zoom, mbgl::mat4 projMatrix, std::function<void()> renderCallback) {

    static std::once_flag flag;
    std::call_once(flag, [] () {
        sample::generate();
        shader::load();
        floor::init();
    });

    const float BUFFER_SCALE = .8;
    const int width = nav::display::pixels::width() * BUFFER_SCALE;
    const int height = nav::display::pixels::height() * BUFFER_SCALE;
    fbo::generate(width, height);
    
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    auto bindScreenFbo = [viewport] () {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
    };
    
    glViewport(0, 0, width, height);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);

    renderSceneToGBuffer(renderCallback);
    generateSSAOTexture(width, height, zoom, convertMatrix(projMatrix));
    blurSSAOTexture(width, height, bindScreenFbo);

    bindScreenFbo();
}

}


}

}
