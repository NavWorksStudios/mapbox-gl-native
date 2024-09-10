//
//  nav.ssao.cpp
//
//  Created by BernieZhao on 2024/9/1.
//

#include "mbgl/nav/ssao/v2/nav.ssao.hpp"

#include "mbgl/nav/ssao/v1/shaders.h"
#include "mbgl/nav/ssao/v1/vec3.h"
#include "mbgl/nav/ssao/v1/mat4.h"
#include "mbgl/nav/ssao/v1/shaders.h"
#include "mbgl/nav/ssao/v1/rply.h"

#include "mbgl/nav/nav.style.hpp"
#include "mbgl/util/mat3.hpp"

#include <vector>
#include <random>
#include <iostream>


#define SCR_WIDTH nav::display::width() * 2
#define SCR_HEIGHT nav::display::height() * 2


namespace nav {
namespace ssao {


// Shader programs and attrib/uniform locations

GLuint shaderGeometryPass;
GLint shaderLightingPass;
GLuint shaderSSAO;
GLint shaderSSAOBlur;


void loadShaders() {
    shaderGeometryPass = 
    createProgram(loadShader(GL_VERTEX_SHADER, "/shaders/9.ssao_geometry.vs"),
                  loadShader(GL_FRAGMENT_SHADER, "/shaders/9.ssao_geometry.fs"));


    shaderLightingPass = 
    createProgram(loadShader(GL_VERTEX_SHADER, "/shaders/9.ssao.vs"),
                  loadShader(GL_FRAGMENT_SHADER, "/shaders/9.ssao_lighting.fs"));

    
    shaderSSAO = 
    createProgram(loadShader(GL_VERTEX_SHADER, "/shaders/9.ssao.vs"),
                  loadShader(GL_FRAGMENT_SHADER, "/shaders/9.ssao.fs"));

    
    shaderSSAOBlur = 
    createProgram(loadShader(GL_VERTEX_SHADER, "/shaders/9.ssao.vs"),
                  loadShader(GL_FRAGMENT_SHADER, "/shaders/9.ssao_blur.fs"));
    
}

// Contain data for the model
std::vector<GLfloat> modelVertices;
std::vector<GLuint> faceIndices;
std::vector<GLfloat> allModelData;

GLuint vertexDataBuf;
GLuint floorBuf;

GLsizei faceIndexCount;

static GLfloat maxValue = 0.0;

void load_error_cb(p_ply ply, const char* message)
{
    fprintf(stderr, "Error loading model: %s\n", message);
}

int load_read_vertex_cb(p_ply_argument argument)
{
    // Get an offset into the modelVertices vector
    long offset;
    ply_get_argument_element(argument, NULL, &offset);
    offset *= 3;
    
    p_ply_property prop;
    const char* propName;
    ply_get_argument_property(argument, &prop, NULL, NULL);
    ply_get_property_info(prop, &propName, NULL, NULL, NULL);
    switch (propName[0]) {
        case 'x':
            break;
        case 'y':
            offset += 1;
            break;
        case 'z':
            offset += 2;
            break;
    }
    modelVertices[offset] = ply_get_argument_value(argument);
    if (fabs(modelVertices[offset]) > fabs(maxValue)) {
        maxValue = fabs(modelVertices[offset]);
        //printf("%f\n", maxValue);
    }
    return 1;
}

int load_read_face_cb(p_ply_argument argument)
{
    // Get an offset into the faceIndices vector
    long offset;
    ply_get_argument_element(argument, NULL, &offset);
    offset *= 3;
    
    long valueIndex;
    ply_get_argument_property(argument, NULL, NULL, &valueIndex);
    if (valueIndex >= 0) {
        offset += valueIndex;
    }
    faceIndices[offset] = ply_get_argument_value(argument);
    return 1;
}

void loadModel()
{
    // ////////////hmm...>>>>>>>>>>>>
    int result;
    p_ply plyModel = ply_open(nav::path("/resources/bun_zipper.ply").c_str(), load_error_cb, 0, NULL);
    if (!plyModel)
        return;
    
    result = ply_read_header(plyModel);
    if (!result)
        return;
    
    long vertexCount = ply_set_read_cb(plyModel, "vertex", "x", load_read_vertex_cb, NULL, 0);
    ply_set_read_cb(plyModel, "vertex", "y", load_read_vertex_cb, NULL, 1);
    ply_set_read_cb(plyModel, "vertex", "z", load_read_vertex_cb, NULL, 2);
    
    modelVertices.resize(vertexCount * 3);
    
    long triCount = ply_set_read_cb(plyModel, "face", "vertex_indices", load_read_face_cb, NULL, 0);
    faceIndices.resize(triCount * 3);
    allModelData.resize(triCount * 3 * 6);
    
    result = ply_read(plyModel);
    if (!result) {
        modelVertices.clear();
        faceIndices.clear();
        allModelData.clear();
    }
    
    ply_close(plyModel);
    
    // Put ALL the data into |allModelData|.
    // Scale vertices to unit cube
    float scaleFactor = 1.0f / maxValue;
    Vec3 halfUnit(0.0f, 0.5f, 0.0f);
    for (std::vector<GLuint>::size_type faceIndex = 0; faceIndex < faceIndices.size() / 3; faceIndex++) {
        GLuint vi1, vi2, vi3;
        vi1 = faceIndices[faceIndex*3];
        vi2 = faceIndices[faceIndex*3+1];
        vi3 = faceIndices[faceIndex*3+2];
        
        Vec3 v1(modelVertices[vi1*3], modelVertices[vi1*3+1], modelVertices[vi1*3+2]);
        Vec3 v2(modelVertices[vi2*3], modelVertices[vi2*3+1], modelVertices[vi2*3+2]);
        Vec3 v3(modelVertices[vi3*3], modelVertices[vi3*3+1], modelVertices[vi3*3+2]);
        
        // Scale vertices to unit cube
        v1 = v1.scale(scaleFactor);
        v2 = v2.scale(scaleFactor);
        v3 = v3.scale(scaleFactor);
        
        v1 = v1.subtract(halfUnit);
        v2 = v2.subtract(halfUnit);
        v3 = v3.subtract(halfUnit);
        
        Vec3 normal = (v2 - v1).cross(v3 - v1);
        normal.normalize();
        
        allModelData[faceIndex*18] = v1.x;
        allModelData[faceIndex*18+1] = v1.y;
        allModelData[faceIndex*18+2] = v1.z;
        allModelData[faceIndex*18+3] = normal.x;
        allModelData[faceIndex*18+4] = normal.y;
        allModelData[faceIndex*18+5] = normal.z;
        allModelData[faceIndex*18+6] = v2.x;
        allModelData[faceIndex*18+7] = v2.y;
        allModelData[faceIndex*18+8] = v2.z;
        allModelData[faceIndex*18+9] = normal.x;
        allModelData[faceIndex*18+10] = normal.y;
        allModelData[faceIndex*18+11] = normal.z;
        allModelData[faceIndex*18+12] = v3.x;
        allModelData[faceIndex*18+13] = v3.y;
        allModelData[faceIndex*18+14] = v3.z;
        allModelData[faceIndex*18+15] = normal.x;
        allModelData[faceIndex*18+16] = normal.y;
        allModelData[faceIndex*18+17] = normal.z;
    }
    
    // Set up VBO for vertex data
    glGenBuffers(1, &vertexDataBuf);
    glBindBuffer(GL_ARRAY_BUFFER, vertexDataBuf);
    glBufferData(GL_ARRAY_BUFFER, allModelData.size() * sizeof(GLfloat), allModelData.data(), GL_STATIC_DRAW);
    
    faceIndexCount = faceIndices.size();
    
    // Clear the vectors now, we're done with them
    modelVertices.clear();
    faceIndices.clear();
    allModelData.clear();
}



GLuint gBuffer;
GLuint gPosition, gNormal, gAlbedo;
GLuint rboDepth;

GLuint ssaoFBO, ssaoBlurFBO;
GLuint ssaoColorBuffer, ssaoColorBufferBlur;

void initializeResources() {
    // Enable and configure textures on applicable texture units
    glActiveTexture(GL_TEXTURE0);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    
    glGenFramebuffers(1, &gBuffer);

    // position color buffer
    glGenTextures(1, &gPosition);
    glBindTexture(GL_TEXTURE_2D, gPosition);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    // normal color buffer
    glGenTextures(1, &gNormal);
    glBindTexture(GL_TEXTURE_2D, gNormal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // color + specular color buffer
    glGenTextures(1, &gAlbedo);
    glBindTexture(GL_TEXTURE_2D, gAlbedo);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // create and attach depth buffer (renderbuffer)
    glGenRenderbuffers(1, &rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT);


    // also create framebuffer to hold SSAO processing stage
    // -----------------------------------------------------
    glGenFramebuffers(1, &ssaoFBO);
    glGenFramebuffers(1, &ssaoBlurFBO);

    // SSAO color buffer
    glGenTextures(1, &ssaoColorBuffer);
    glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, SCR_WIDTH, SCR_HEIGHT, 0, GL_RED, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // and blur stage
    glGenTextures(1, &ssaoColorBufferBlur);
    glBindTexture(GL_TEXTURE_2D, ssaoColorBufferBlur);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, SCR_WIDTH, SCR_HEIGHT, 0, GL_RED, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    // Set up buffer for floor data, 12 pieses
    const static GLfloat floorData[36] = {
        -10.0f, -0.4f, -10.0f,
        0.0f, 1.0f, 0.0f,
        -10.0f, -0.4f, 10.0f,
        0.0f, 1.0f, 0.0f,
        10.0f, -0.4f, 10.0f,
        0.0f, 1.0f, 0.0f,
        10.0f, -0.4f, 10.0f,
        0.0f, 1.0f, 0.0f,
        10.0f, -0.4f, -10.0f,
        0.0f, 1.0f, 0.0f,
        -10.0f, -0.4f, -10.0f,
        0.0f, 1.0f, 0.0f };
    glGenBuffers(1, &floorBuf);
    glBindBuffer(GL_ARRAY_BUFFER, floorBuf);
    glBufferData(GL_ARRAY_BUFFER, 36 * sizeof(GLfloat), floorData, GL_STATIC_DRAW);
}



std::vector<Vec3> ssaoKernel;
std::vector<Vec3> ssaoNoise;
GLuint noiseTexture = 0;

GLfloat lerp(GLfloat a, GLfloat b, GLfloat f) {
    return a + f * (b - a);
}

void genSampleKernelAndNoiseTexture() {

    // - generate sample kernel
    std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0); // generates random floats between 0.0 and 1.0
    std::default_random_engine generator;
    for (unsigned int i = 0; i < 64; ++i)
    {
        Vec3 sample(
            randomFloats(generator) * 2.0 - 1.0, // x, -1.0到1.0
            randomFloats(generator) * 2.0 - 1.0, // y, -1.0到1.0
            randomFloats(generator) );           // z, 0到1
        
        sample.normalize();
        sample.scale(randomFloats(generator));
        float scale = float(i) / 64.0f;
        // scale samples s.t. they're more aligned to center of kernel
        scale = lerp(0.1f, 1.0f, scale * scale);
        sample.scale(scale);
        ssaoKernel.push_back(sample);
    }

    // - Noise texture
    for (GLuint i = 0; i < 16; i++) {
        //基于屏幕空间，z为0
        Vec3 noise(
            randomFloats(generator) * 2.0 - 1.0,
            randomFloats(generator) * 2.0 - 1.0,
            0.0f );

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






GLint uniformLocation(GLuint program, const GLchar *name) {
    GLint location = glGetUniformLocation(program, name);
    assert(location >= 0);
    return location;
}

GLint attribLocation(GLuint program, const GLchar *name) {
    GLint location = glGetAttribLocation(program, name);
    assert(location >= 0);
    return location;
}

// renderQuad() renders a 1x1 XY quad in NDC
// -----------------------------------------

void renderQuad(GLint program)
{
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
        
        GLint location;
        
        location = attribLocation(program, "aPos");
        glEnableVertexAttribArray(location);
        glVertexAttribPointer(location, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        
        location = attribLocation(program, "aTexCoords");
        glEnableVertexAttribArray(location);
        glVertexAttribPointer(location, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }

    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

// the camera info
Vec3 eye = Vec3(0, 1.5, 1.5);
Vec3 lookat = Vec3(0, 0, 0);


// 1. geometry pass: render scene's geometry/color data into gbuffer
// -----------------------------------------------------------------

void renderSceneToGBuffer() {

    {
        glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
        
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedo, 0);
        
        // tell OpenGL which color attachments we'll use (of this framebuffer) for rendering
        unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
        glDrawBuffers(3, attachments);
        
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
    }
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glUseProgram(shaderGeometryPass);
    
    const static double pi = acos(0.0) * 2;
    Mat4 ident = Mat4::identityMatrix();
    Mat4 view, viewNorm;
    Mat4::lookAtMatrix(eye, lookat, Vec3(0, 1, 0), view, viewNorm);
    Mat4 proj = Mat4::perspectiveMatrix(pi * 0.5, 1.333333f, 0.1f, 1000.0f);
    
    Mat4 mvp = proj * view;
    glUniformMatrix4fv(uniformLocation(shaderGeometryPass, "modelViewMat"), 1, GL_FALSE, reinterpret_cast<float*>(&view));
    glUniformMatrix4fv(uniformLocation(shaderGeometryPass, "modelViewProjMat"), 1, GL_FALSE, reinterpret_cast<float*>(&mvp));
    glUniformMatrix4fv(uniformLocation(shaderGeometryPass, "normalMat"), 1, GL_FALSE, reinterpret_cast<float*>(&ident));

    // 绘制 floor
    {
        glBindBuffer(GL_ARRAY_BUFFER, floorBuf);
        
        GLint location;
        
        location = attribLocation(shaderGeometryPass, "aPos");
        glEnableVertexAttribArray(location);
        glVertexAttribPointer(location, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void*>(0));
        
        location = attribLocation(shaderGeometryPass, "aNormal");
        glEnableVertexAttribArray(location);
        glVertexAttribPointer(location, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void*>(3*sizeof(GLfloat)));
        
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
    
    // 绘制 ply obj
    {
        // Set up vertex attributes
        glBindBuffer(GL_ARRAY_BUFFER, vertexDataBuf);
        
        GLint location;
        
        location = attribLocation(shaderGeometryPass, "aPos");
        glEnableVertexAttribArray(location);
        glVertexAttribPointer(location, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void*>(0));
        
        location = attribLocation(shaderGeometryPass, "aNormal");
        glEnableVertexAttribArray(location);
        glVertexAttribPointer(location, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void*>(3*sizeof(GLfloat)));
        
        glDrawArrays(GL_TRIANGLES, 0, faceIndexCount);
    }

}

// 2. generate SSAO texture
// ------------------------
void generateSSAOTexture() {
    {
        glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColorBuffer, 0);
        glDrawBuffer(GL_COLOR_ATTACHMENT0);
    }
    
    glClear(GL_COLOR_BUFFER_BIT);
    
    glUseProgram(shaderSSAO);
    
    // Send kernel + rotation
    for (unsigned int i = 0; i < 64; ++i) {
        GLint location = uniformLocation(shaderSSAO, ("samples[" + std::to_string(i) + "]").c_str());
        glUniform3fv(location, 1, reinterpret_cast<float*>(&(ssaoKernel[i])));
    }
    
    const static double pi = acos(0.0) * 2;
    Mat4 ident = Mat4::identityMatrix();
    Mat4 view, viewNorm;
    Mat4::lookAtMatrix(eye, lookat, Vec3(0, 1, 0), view, viewNorm);
    Mat4 projection = Mat4::perspectiveMatrix(pi * 0.5, 1.333333f, 0.1f, 1000.0f);
    
    glUniformMatrix4fv(uniformLocation(shaderSSAO, "projection"), 1, GL_FALSE, reinterpret_cast<float*>(&projection));
    
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gPosition);
        
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, gNormal);
        
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, noiseTexture);
    }
    
    renderQuad(shaderSSAO);
}

// 3. blur SSAO texture to remove noise
// ------------------------------------
void blurSSAOTexture() {
    {
        glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColorBufferBlur, 0);
        glDrawBuffer(GL_COLOR_ATTACHMENT0);
    }
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    glClear(GL_COLOR_BUFFER_BIT);
    
    glUseProgram(shaderSSAOBlur);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
    
    renderQuad(shaderSSAOBlur);
}

// 4. lighting pass: traditional deferred Blinn-Phong lighting with added screen-space ambient occlusion
// -----------------------------------------------------------------------------------------------------
void lightingPass() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glUseProgram(shaderLightingPass);
    
    // send light relevant uniforms
    Vec3 lightPos(2.0, 4.0, -2.0);
    Vec3 lightColor(0.2, 0.2, 0.7);
    
    const static double pi = acos(0.0) * 2;
    Mat4 ident = Mat4::identityMatrix();
    Mat4 view, viewNorm;
    Mat4::lookAtMatrix(eye, lookat, Vec3(0, 1, 0), view, viewNorm);
    Vec3 lightPosView(view * lightPos);
    
    glUniform3fv(uniformLocation(shaderLightingPass, "light.Position"), 1, reinterpret_cast<float*>(&lightPosView));
    glUniform3fv(uniformLocation(shaderLightingPass, "light.Color"), 1, reinterpret_cast<float*>(&lightColor));
    
    // Update attenuation parameters
    const float linear    = 0.09f;
    const float quadratic = 0.032f;
    glUniform1f(uniformLocation(shaderLightingPass, "light.Linear"), linear);
    glUniform1f(uniformLocation(shaderLightingPass, "light.Quadratic"), quadratic);
    
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gPosition);
        
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, gNormal);
        
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, gAlbedo);
        
        glActiveTexture(GL_TEXTURE3); // add extra SSAO texture to lighting pass
        glBindTexture(GL_TEXTURE_2D, ssaoColorBufferBlur);
    }
    
    renderQuad(shaderLightingPass);
}

namespace v2 {

void draw() {
    static bool initized = false;
    if (!initized) {
        initized = true;
        
        initializeResources();
        genSampleKernelAndNoiseTexture();
        loadShaders();
        loadModel();
    }
    
    glClearColor(1, 1, 1, 1);
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);
    
    renderSceneToGBuffer();
    generateSSAOTexture();
    blurSSAOTexture();
//    lightingPass();
}

}


}

}
