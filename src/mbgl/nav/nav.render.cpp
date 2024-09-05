//
//  nav.render.cpp
//
//  Created by BernieZhao on 2024/9/1.
//

#include "mbgl/nav/nav.render.hpp"

#include <vector>
#include <random>
#include <iostream>

#include <mbgl/util/mat3.hpp>
#include "mbgl/nav/nav.style.hpp"

enum {
    GL_RGB16F = GL_RGB16F_ARB,
    GL_RGBA16F = GL_RGBA16F_ARB,
};

#define SCR_WIDTH nav::display::width()
#define SCR_HEIGHT nav::display::height()


namespace nav {

namespace render {


namespace gbuffer {
    
GLuint gBuffer = 0;
GLuint gPositionDepth = 0, gNormal = 0, gAlbedo = 0;
GLuint rboDepth = 0;

void genFramebuffer() {
    if (gBuffer) return;
    
    // Set up G-Buffer
    // 3 textures:
    // 1. Positions + depth (RGBA)
    // 2. Color (RGB)
    // 3. Normals (RGB)
    glGenFramebuffers(1, &gBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
    
    // - Position + linear depth color buffer
    glGenTextures(1, &gPositionDepth);
    glBindTexture(GL_TEXTURE_2D, gPositionDepth);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPositionDepth, 0);
    
    // - Normal color buffer
    glGenTextures(1, &gNormal);
    glBindTexture(GL_TEXTURE_2D, gNormal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);
    
    // - Albedo color buffer
    glGenTextures(1, &gAlbedo);
    glBindTexture(GL_TEXTURE_2D, gAlbedo);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedo, 0);
    
    // - Tell OpenGL which color attachments we'll use (of this framebuffer) for rendering
    GLuint attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    glDrawBuffers(3, attachments);
    
    // - Create and attach depth buffer (renderbuffer)
    glGenRenderbuffers(1, &rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
    
    // - Finally check if framebuffer is complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "GBuffer Framebuffer not complete!" << std::endl;
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
    
}



namespace aobuffer {

GLuint ssaoFBO = 0, ssaoBlurFBO = 0;
GLuint ssaoColorBuffer = 0, ssaoColorBufferBlur = 0;

void genFramebuffer() {
    if (ssaoFBO) return;
    
    // - Also create framebuffer to hold SSAO processing stage
    glGenFramebuffers(1, &ssaoFBO);
    glGenFramebuffers(1, &ssaoBlurFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
    
    // - SSAO color buffer
    glGenTextures(1, &ssaoColorBuffer);
    glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColorBuffer, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "SSAO Framebuffer not complete!" << std::endl;

    // - and blur stage
    glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
    glGenTextures(1, &ssaoColorBufferBlur);
    glBindTexture(GL_TEXTURE_2D, ssaoColorBufferBlur);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColorBufferBlur, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "SSAO Blur Framebuffer not complete!" << std::endl;
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

}



namespace noisebuffer {

GLuint noiseTexture = 0;

GLfloat lerp(GLfloat a, GLfloat b, GLfloat f) {
    return a + f * (b - a);
}

void genTexture() {
    if (noiseTexture) return;
    
    // - Sample kernel
    std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0); // 随机浮点数，范围0.0 - 1.0
    std::default_random_engine generator;
    std::vector<mbgl::vec3> ssaoKernel;
    for (GLuint i = 0; i < 64; ++i) {
        mbgl::vec3 sample = {
            randomFloats(generator) * 2.0 - 1.0, // x, -1.0到1.0
            randomFloats(generator) * 2.0 - 1.0, // y, -1.0到1.0
            randomFloats(generator) };           // z, 0到1

        sample = mbgl::vec3Normalize(sample);
        sample = mbgl::vec3Scale(sample, randomFloats(generator));
        float scale = float(i) / 64.0; // 每一份有64个
        scale = lerp(0.1f, 1.0f, scale * scale);
        sample = mbgl::vec3Scale(sample, scale);

        ssaoKernel.push_back(sample);
    }

    // - Noise texture
    std::vector<mbgl::vec3> ssaoNoise;
    for (GLuint i = 0; i < 16; i++) {
        //基于屏幕空间，z为0
        mbgl::vec3 noise = {
            randomFloats(generator) * 2.0 - 1.0,
            randomFloats(generator) * 2.0 - 1.0,
            0.0f };

        ssaoNoise.push_back(noise);
    }

    glGenTextures(1, &noiseTexture);
    glBindTexture(GL_TEXTURE_2D, noiseTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

}


void createRenderbuffer() {
    gbuffer::genFramebuffer();
    aobuffer::genFramebuffer();
    noisebuffer::genTexture();
}

GLuint gBuffer() {
    return gbuffer::gBuffer;
}

void renderGBuffer() {
    glBindFramebuffer(GL_FRAMEBUFFER, gbuffer::gBuffer);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
//    glm::mat4 projection = glm::perspective(glm::radians(90.0F), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
//    glm::mat4 view = camera.GetViewMatrix();
//    glm::mat4 model = glm::mat4(1.0f);
//    shaderGeometryPass.use();
//    shaderGeometryPass.setMat4("projection", projection);
//    shaderGeometryPass.setMat4("view", view);

//    glUniformMatrix4fv(glGetUniformLocation(shaderGeometryPass.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
//    glUniformMatrix4fv(glGetUniformLocation(shaderGeometryPass.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
    
    // Floor cube
//    model = glm::translate(model, glm::vec3(0.0, -1.0f, 0.0f));
//    model = glm::scale(model, glm::vec3(20.0f, 1.0f, 20.0f));
//    glUniformMatrix4fv(glGetUniformLocation(shaderGeometryPass.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
//    renderCube();
    
    // Nanosuit model on the floor
//    model = glm::mat4();
//    model = glm::translate(model, glm::vec3(0.0f, 1.0f, 0.0));
//    model = glm::rotate(model, 80.0f, glm::vec3(1.0, 0.0, 0.0));
//    model = glm::scale(model, glm::vec3(0.5f));
//    glUniformMatrix4fv(glGetUniformLocation(shaderGeometryPass.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
    
//    nanosuit.Draw(shaderGeometryPass);
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void renderAOBuffer() {
    glBindFramebuffer(GL_FRAMEBUFFER, aobuffer::ssaoFBO);
    glClear(GL_COLOR_BUFFER_BIT);
    
//    shaderSSAO.use();
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gbuffer::gPositionDepth);
    
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, gbuffer::gNormal);
    
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, noisebuffer::noiseTexture);
    
    // Send kernel + rotation
//    for (GLuint i = 0; i < 64; ++i)
//        glUniform3fv(glGetUniformLocation(shaderSSAO.ID, ("samples[" + std::to_string(i) + "]").c_str()), 1, &ssaoKernel[i][0]);
//    glUniformMatrix4fv(glGetUniformLocation(shaderSSAO.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
//    renderQuad();
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void renderAO() {
    glBindFramebuffer(GL_FRAMEBUFFER, aobuffer::ssaoBlurFBO);
    glClear(GL_COLOR_BUFFER_BIT);
    
//    shaderSSAOBlur.use();
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, aobuffer::ssaoColorBuffer);
    
//    renderQuad();
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//    shaderLightingPass.use();
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gbuffer::gPositionDepth);
    
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, gbuffer::gNormal);
    
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, gbuffer::gAlbedo);
    
    glActiveTexture(GL_TEXTURE3); // 模糊后的AO
    glBindTexture(GL_TEXTURE_2D, aobuffer::ssaoColorBufferBlur);
    
    // Also send light relevant uniforms
//    glm::vec3 lightPosView = glm::vec3(camera.GetViewMatrix() * glm::vec4(lightPos, 1.0));
//    glUniform3fv(glGetUniformLocation(shaderLightingPass.ID, "light.Position"), 1, &lightPosView[0]);
//    glUniform3fv(glGetUniformLocation(shaderLightingPass.ID, "light.Color"), 1, &lightColor[0]);
    
    // Update attenuation parameters
//    const float constant = 1.0; // Note that we don't send this to the shader, we assume it is always 1.0 (in our case)
//    const float linear = 0.09;
//    const float quadratic = 0.032;

//    glUniform1f(glGetUniformLocation(shaderLightingPass.ID, "light.Linear"), linear);
//    glUniform1f(glGetUniformLocation(shaderLightingPass.ID, "light.Quadratic"), quadratic);
//    glUniform1i(glGetUniformLocation(shaderLightingPass.ID, "draw_mode"), draw_mode);

//    renderQuad();
}


}

}
