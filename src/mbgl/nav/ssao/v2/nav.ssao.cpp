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

#include <vector>
#include <random>
#include <iostream>


#define SCR_WIDTH nav::display::width() * 2
#define SCR_HEIGHT nav::display::height() * 2


namespace nav {
namespace ssao {


// Shader programs and attrib/uniform locations

GLuint shaderGeometryPass;
GLuint shaderSSAO;
GLint shaderSSAOBlur;
GLint shaderLightingPass;

void loadShaders() {
    shaderGeometryPass = 
    createProgram(loadShader(GL_VERTEX_SHADER, "/shaders/9.ssao_geometry.vs"),
                  loadShader(GL_FRAGMENT_SHADER, "/shaders/9.ssao_geometry.fs"));

    
    shaderSSAO = 
    createProgram(loadShader(GL_VERTEX_SHADER, "/shaders/9.ssao.vs"),
                  loadShader(GL_FRAGMENT_SHADER, "/shaders/9.ssao.fs"));

    
    shaderSSAOBlur = 
    createProgram(loadShader(GL_VERTEX_SHADER, "/shaders/9.ssao.vs"),
                  loadShader(GL_FRAGMENT_SHADER, "/shaders/9.ssao_blur.fs"));
//                  loadShader(GL_FRAGMENT_SHADER, "/shaders/9.median_filter.fs"));
//                  loadShader(GL_FRAGMENT_SHADER, "/shaders/9.kuwahara_filter.fs"));
    
    shaderLightingPass =
    createProgram(loadShader(GL_VERTEX_SHADER, "/shaders/9.ssao.vs"),
                  loadShader(GL_FRAGMENT_SHADER, "/shaders/9.ssao_lighting.fs"));
    
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
GLuint gPosition;
GLuint gNormal;
GLuint gAlbedo;
GLuint rboDepth;

GLuint ssaoFBO;
GLuint ssaoBuffer;

GLuint ssaoBlurFBO;
GLuint ssaoBlurBuffer;

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
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // create and attach depth buffer (renderbuffer)
    glGenRenderbuffers(1, &rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT);


    // also create framebuffer to hold SSAO processing stage
    // -----------------------------------------------------
    
    // SSAO color buffer
    glGenFramebuffers(1, &ssaoFBO);
    glGenTextures(1, &ssaoBuffer);
    glBindTexture(GL_TEXTURE_2D, ssaoBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, SCR_WIDTH, SCR_HEIGHT, 0, GL_RED, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // and blur stage
    glGenFramebuffers(1, &ssaoBlurFBO);
    glGenTextures(1, &ssaoBlurBuffer);
    glBindTexture(GL_TEXTURE_2D, ssaoBlurBuffer);
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


enum { SAMPLE_SIZE = 16 };
std::vector<Vec3> ssaoSample;
std::vector<float> ssaoSampleWeight;
float ssaoSampleWeightTotal;
std::vector<Vec3> ssaoNoise;
GLuint noiseTexture = 0;

GLfloat lerp(GLfloat a, GLfloat b, GLfloat f) {
    return a + f * (b - a);
}

void genSampleKernelAndNoiseTexture() {
    
    ssaoSampleWeightTotal = 0;

    // 生成一个沿法线方向的半球形采样核心，将在切线空间生成这个半球（法线都指向+z轴），以免为每个平面都单独生成一个沿各自法线方向的半球。
    // - generate sample kernel
    std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0); // generates random floats between 0.0 and 1.0
    std::default_random_engine generator;
    for (unsigned int i = 0; i < SAMPLE_SIZE; ++i)
    {
        Vec3 sample(
            randomFloats(generator) * 2.0 - 1.0, // x, -1.0到1.0
            randomFloats(generator) * 2.0 - 1.0, // y, -1.0到1.0
            randomFloats(generator) );           // z, 0到1
        
        sample.normalize();
        sample.scale(randomFloats(generator));
        float scale = float(i) / SAMPLE_SIZE;
        
        // scale samples s.t. they're more aligned to center of kernel
        scale = lerp(0.1f, 1.0f, scale * scale);
        sample.scale(scale);

        ssaoSample.push_back(sample);
        
        float w = 1.;//pow(1. / sample.norm(), 1.);
        ssaoSampleWeight.push_back(w);
        ssaoSampleWeightTotal += w;
    }

    // 随机核心旋转
    // 将每个核心的随机旋转存储在纹理中。创建一个4*4阵列的随即旋转向量，绕着切线空间的法线：
    // - Noise texture
    for (GLuint i = 0; i < 16; i++) {
        //基于屏幕空间，z为0
        Vec3 noise(
            randomFloats(generator) * 2.0 - 1.0,
            randomFloats(generator) * 2.0 - 1.0,
            0.0f );
        
        noise.normalize();
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

// the camera info
Vec3 eye = Vec3(0., 1.5, 1.5);
Vec3 lookat = Vec3(0, 0, 0);



//#define RABBIT


// 1. geometry pass: render scene's geometry/color data into gbuffer
// -----------------------------------------------------------------

void renderSceneToGBuffer(std::function<void()> renderCallback) {
    
    {
        glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedo, 0);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
        
        // tell OpenGL which color attachments we'll use (of this framebuffer) for rendering
        unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
        glDrawBuffers(3, attachments);
    }

    glDisable(GL_BLEND);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
#ifdef RABBIT

    glUseProgram(shaderGeometryPass);
    
    {
        const static double pi = acos(0.0) * 2;
        Mat4 ident = Mat4::identityMatrix();
        
        Mat4 mv, normal;
        Mat4::lookAtMatrix(eye, lookat, Vec3(0, 1, 0), mv, normal);
        
        Mat4 p = Mat4::perspectiveMatrix(pi * 0.5, 1.333333f, 0.1f, 1000.0f);
        Mat4 mvp = p * mv;
        
        static UniformLocation u0(shaderGeometryPass, "modelViewMat");
        glUniformMatrix4fv(u0, 1, GL_FALSE, reinterpret_cast<float*>(&mv));
        static UniformLocation u1(shaderGeometryPass, "modelViewProjMat");
        glUniformMatrix4fv(u1, 1, GL_FALSE, reinterpret_cast<float*>(&mvp));
        static UniformLocation u2(shaderGeometryPass, "normalMat");
        glUniformMatrix4fv(u2, 1, GL_FALSE, reinterpret_cast<float*>(&normal));
    }
    
    static AttribLocation a0(shaderGeometryPass, "aPos");
    static AttribLocation a1(shaderGeometryPass, "aNormal");
    
    // 绘制 floor
    {
        glBindBuffer(GL_ARRAY_BUFFER, floorBuf);
        
        glEnableVertexAttribArray(a0);
        glVertexAttribPointer(a0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void*>(0));
        
        glEnableVertexAttribArray(a1);
        glVertexAttribPointer(a1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void*>(3*sizeof(GLfloat)));
        
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
    
    // 绘制 ply obj
    {
        // Set up vertex attributes
        glBindBuffer(GL_ARRAY_BUFFER, vertexDataBuf);
        
        glEnableVertexAttribArray(a0);
        glVertexAttribPointer(a0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void*>(0));
        
        glEnableVertexAttribArray(a1);
        glVertexAttribPointer(a1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void*>(3*sizeof(GLfloat)));
        
        glDrawArrays(GL_TRIANGLES, 0, faceIndexCount);
    }
    
#else
    
    renderCallback();
    
#endif
}

// 2. generate SSAO texture
// ------------------------
void generateSSAOTexture(float zoom, const Mat4& projMatrix) {

    {
        glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoBuffer, 0);
        glDrawBuffer(GL_COLOR_ATTACHMENT0);
    }

    glClear(GL_COLOR_BUFFER_BIT);
    
    glUseProgram(shaderSSAO);
    
    {
        // Send kernel + rotation
        for (unsigned int i = 0; i < SAMPLE_SIZE; ++i) {
            UniformLocation u0(shaderSSAO, ("u_samples[" + std::to_string(i) + "]").c_str());
            glUniform3fv(u0, 1, reinterpret_cast<float*>(&(ssaoSample[i])));
            
            UniformLocation u1(shaderSSAO, ("u_samples_weight[" + std::to_string(i) + "]").c_str());
            glUniform1fv(u1, 1, reinterpret_cast<float*>(&(ssaoSampleWeight[i])));
        }
        
        UniformLocation u2(shaderSSAO, "u_samples_weight_total");
        glUniform1f(u2, ssaoSampleWeightTotal);
    }
    
#ifdef RABBIT
    {
        const static double pi = acos(0.0) * 2;
        Mat4 projection = Mat4::perspectiveMatrix(pi * 0.5, 1.333333f, 0.1f, 1000.0f);
        static UniformLocation u0(shaderSSAO, "u_projection");
        glUniformMatrix4fv(u0, 1, GL_FALSE, reinterpret_cast<float*>(&projection));
    }
#else
    {
        static UniformLocation u0(shaderSSAO, "u_projection");
        glUniformMatrix4fv(u0, 1, GL_FALSE, reinterpret_cast<const float*>(&projMatrix));
    }
#endif

    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gPosition);
        static UniformLocation u0(shaderSSAO, "u_position");
        glUniform1i(u0, 0);
        
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, gNormal);
        static UniformLocation u1(shaderSSAO, "u_normal");
        glUniform1i(u1, 1);
        
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, noiseTexture);
        static UniformLocation u2(shaderSSAO, "u_noise");
        glUniform1i(u2, 2);
        
        static UniformLocation u3(shaderSSAO, "u_zoom");
        glUniform1f(u3, zoom);
    }
    
    renderQuad(shaderSSAO);

}

// 3. blur SSAO texture to remove noise
// ------------------------------------
void blurSSAOTexture() {

    {
        glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoBlurBuffer, 0);
        glDrawBuffer(GL_COLOR_ATTACHMENT0);
    }

    glClear(GL_COLOR_BUFFER_BIT);
    
    glUseProgram(shaderSSAOBlur);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, ssaoBuffer);
    static UniformLocation u0(shaderSSAOBlur, "ssao");
    glUniform1i(u0, 0);
    
    renderQuad(shaderSSAOBlur);
    
}

// 4. lighting pass: traditional deferred Blinn-Phong lighting with added screen-space ambient occlusion
// -----------------------------------------------------------------------------------------------------
void lightingPass() {

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    glClear(GL_COLOR_BUFFER_BIT);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glUseProgram(shaderLightingPass);
    
    {
        // send light relevant uniforms
        Vec3 lightPos(0.0f, 0.0f, 0.5f);
        Vec3 lightColor(1., 1., 1.);
        Mat4 view, notused;
        Mat4::lookAtMatrix(eye, lookat, Vec3(0, 1, 0), view, notused);
        Vec3 lightPosView(view * lightPos);
        static UniformLocation u0(shaderLightingPass, "light.Position");
        glUniform3fv(u0, 1, reinterpret_cast<float*>(&lightPosView));
        static UniformLocation u1(shaderLightingPass, "light.Color");
        glUniform3fv(u1, 1, reinterpret_cast<float*>(&lightColor));
        
        // Update attenuation parameters
        const float linear    = 0.09f;
        const float quadratic = 0.032f;
        static UniformLocation u2(shaderLightingPass, "light.Linear");
        glUniform1f(u2, linear);
        static UniformLocation u3(shaderLightingPass, "light.Quadratic");
        glUniform1f(u3, quadratic);
    }
    
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gPosition);
        static UniformLocation u0(shaderLightingPass, "gPosition");
        glUniform1i(u0, 0);
        
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, gNormal);
        static UniformLocation u1(shaderLightingPass, "gNormal");
        glUniform1i(u1, 1);
        
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, gAlbedo);
        static UniformLocation u2(shaderLightingPass, "gAlbedo");
        glUniform1i(u2, 2);
        
        glActiveTexture(GL_TEXTURE3); // add extra SSAO texture to lighting pass
        glBindTexture(GL_TEXTURE_2D, ssaoBlurBuffer);
        static UniformLocation u3(shaderLightingPass, "ssaoBlur");
        glUniform1i(u3, 3);
    }
    
    renderQuad(shaderLightingPass);
    
    glDisable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

}

namespace v2 {

auto convert = [] (mbgl::mat4 matrix) {
    Mat4 m;
    for (int i=0; i<16; i++) ((float*)&m)[i] = float(matrix[i]);
    return m;
};

void draw(float zoom, mbgl::mat4 projMatrix, std::function<void()> renderCallback) {
    
    static bool initized = false;
    if (!initized) {
        initized = true;
        
        initializeResources();
        genSampleKernelAndNoiseTexture();
        loadShaders();
        loadModel();
    }

    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);

    renderSceneToGBuffer(renderCallback);
    generateSSAOTexture(zoom, convert(projMatrix));
    blurSSAOTexture();
    lightingPass();

}

}


}

}
