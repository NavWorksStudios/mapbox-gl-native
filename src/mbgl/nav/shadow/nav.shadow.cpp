//
//  nav.ssao.cpp
//
//  Created by BernieZhao on 2024/9/1.
//

#include "mbgl/nav/ssao/v2/nav.ssao.hpp"

#include <random>

#include "mbgl/nav/ssao/v1/shaders.h"
#include "mbgl/nav/ssao/v1/vec3.h"
#include "mbgl/nav/ssao/v1/mat4.h"
#include "mbgl/nav/ssao/v1/rply.h"

#include "mbgl/nav/nav.style.hpp"
#include "mbgl/nav/nav.palette.hpp"


#define SCR_WIDTH nav::display::pixels::width()
#define SCR_HEIGHT nav::display::pixels::height()


namespace nav {

namespace shadow {

namespace sample {

// Framebuffers, textures to render to, renderbuffers, other textures
GLuint modelbuffer;
GLuint modelDepthTexture;
GLuint shadowbuffer;
GLuint shadowDepthTexture;
GLuint floorBuf;

void initializeResources()
{
    // Enable and configure textures on applicable texture units
    glActiveTexture(GL_TEXTURE0);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    
    // Create our framebuffer
    glGenFramebuffers(1, &modelbuffer);
    
    // Setup a texture to render depth to
    glDeleteTextures(1, &modelDepthTexture);
    glGenTextures(1, &modelDepthTexture);
    glBindTexture(GL_TEXTURE_2D, modelDepthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    // Create our framebuffer
    glGenFramebuffers(1, &shadowbuffer);
    
    // Setup a texture to render depth to
    glDeleteTextures(1, &shadowDepthTexture);
    glGenTextures(1, &shadowDepthTexture);
    glBindTexture(GL_TEXTURE_2D, shadowDepthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    // Set up buffer for floor data, 12 pieses
    const static GLfloat floorData[36] = {
        -1.0f, -0.4f, -1.0f,
        0.0f, 1.0f, 0.0f,
        -1.0f, -0.4f, 1.0f,
        0.0f, 1.0f, 0.0f,
        1.0f, -0.4f, 1.0f,
        0.0f, 1.0f, 0.0f,
        1.0f, -0.4f, 1.0f,
        0.0f, 1.0f, 0.0f,
        1.0f, -0.4f, -1.0f,
        0.0f, 1.0f, 0.0f,
        -1.0f, -0.4f, -1.0f,
        0.0f, 1.0f, 0.0f };
    glGenBuffers(1, &floorBuf);
    glBindBuffer(GL_ARRAY_BUFFER, floorBuf);
    glBufferData(GL_ARRAY_BUFFER, 36 * sizeof(GLfloat), floorData, GL_STATIC_DRAW);
}

// Contain data for the model
std::vector<GLfloat> modelVertices;
std::vector<GLuint> faceIndices;
std::vector<GLfloat> allModelData;

GLsizei faceIndexCount;
GLuint vertexDataBuf;

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

GLuint modelProg;
GLint modelProgPosAttrib;
GLint modelProgNormAttrib;
GLint modelProgModelViewMat;
GLint modelProgMvpMat;
GLint modelProgNormalMat;

GLint modelProgLAmb;
GLint modelProgLPos;
GLint modelProgLDif;
GLint modelProgLSpc;

GLint modelProgKAmb;
GLint modelProgKDif;
GLint modelProgKSpc;
GLint modelProgKShn;

GLuint shadowProg;
GLint shadowProgPosAttrib;
GLint shadowProgNormAttrib;
GLint shadowProgModelViewMat;
GLint shadowProgMvpMat;
GLint shadowProgNormalMat;

GLint shadowProgLAmb;
GLint shadowProgLPos;
GLint shadowProgLDif;
GLint shadowProgLSpc;

GLint shadowProgKAmb;
GLint shadowProgKDif;
GLint shadowProgKSpc;
GLint shadowProgKShn;

void loadShaders() {
    
    // shadow shaders
    modelProg = createProgram(loadShader(GL_VERTEX_SHADER, "/shaders/modelProg.vert"),
                              loadShader(GL_FRAGMENT_SHADER, "/shaders/modelProg.frag"));
    
    // uniform & attribute 待定
    modelProgPosAttrib = glGetAttribLocation(modelProg, "positionIn");
    modelProgNormAttrib = glGetAttribLocation(modelProg, "normalIn");

    modelProgModelViewMat = glGetUniformLocation(modelProg, "modelViewMat");
    modelProgMvpMat = glGetUniformLocation(modelProg, "modelViewProjMat");
    modelProgNormalMat = glGetUniformLocation(modelProg, "normalMat");

    modelProgLAmb = glGetUniformLocation(modelProg, "lAmbient");
    modelProgLPos = glGetUniformLocation(modelProg, "lPosition");
    modelProgLDif = glGetUniformLocation(modelProg, "lDiffuse");
    modelProgLSpc = glGetUniformLocation(modelProg, "lSpecular");

    modelProgKAmb = glGetUniformLocation(modelProg, "kAmbient");
    modelProgKDif = glGetUniformLocation(modelProg, "kDiffuse");
    modelProgKSpc = glGetUniformLocation(modelProg, "kSpecular");
    modelProgKShn = glGetUniformLocation(modelProg, "kShininess");
    
    shadowProg = createProgram(loadShader(GL_VERTEX_SHADER, "/shaders/shadowProg.vert"),
                              loadShader(GL_FRAGMENT_SHADER, "/shaders/shadowProg.frag"));

    shadowProgPosAttrib = glGetAttribLocation(shadowProg, "positionIn");
    shadowProgNormAttrib = glGetAttribLocation(shadowProg, "normalIn");

    shadowProgModelViewMat = glGetUniformLocation(modelProg, "modelViewMat");
    shadowProgMvpMat = glGetUniformLocation(shadowProg, "modelViewProjMat");
    shadowProgNormalMat = glGetUniformLocation(modelProg, "normalMat");
    
}

// the camera info
Vec3 eye = Vec3(0.5, 1.5, 0.5);
Vec3 lookat = Vec3(0, 0, 0);
Vec3 light_eye = Vec3(-0.75, 1.0, 0.75);

void drawModel(bool shadow)
{
//    glBindFramebuffer(GL_FRAMEBUFFER, modelbuffer); // 往 framebuffer 上画
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, modelDepthTexture, 0);
    GLenum bufs[1] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, bufs);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(modelProg);

    // set uniform
    {
        // 设置mat
        const static double pi = acos(0.0) * 2;
        Mat4 ident = Mat4::identityMatrix();
        Mat4 view, viewNorm;
        Mat4::lookAtMatrix(eye, lookat, Vec3(0, 1, 0), view, viewNorm);
        Mat4 proj = Mat4::perspectiveMatrix(pi * 0.5, 1.333333f, 0.1f, 1000.0f);

        Mat4 mvp = proj * view;
        glUniformMatrix4fv(modelProgModelViewMat, 1, GL_FALSE, reinterpret_cast<float*>(&view));
        glUniformMatrix4fv(modelProgMvpMat, 1, GL_FALSE, reinterpret_cast<float*>(&mvp));
        glUniformMatrix4fv(modelProgNormalMat, 1, GL_FALSE, reinterpret_cast<float*>(&ident));

        // 设置 Lighting uniforms
        static GLfloat lPos[3] = {0.5f, 1.5f, 0.5f};

        static GLfloat lAmb[3] = {1.0f, 1.0f, 1.0f}; // 环境光颜色
        static GLfloat kAmb[3] = {0.9f, 0.9f, 0.9f};
        static GLfloat lDif[3] = {1.0f, 1.0f, 1.0f}; // 漫反光颜色
        static GLfloat kDif[3] = {0.4f, 0.4f, 0.4f};
        static GLfloat lSpc[3] = {1.0f, 1.0f, 1.0f}; // 镜面光颜色
        static GLfloat kSpc[3] = {0.01f, 0.01f, 0.01f};
        static GLfloat kShn = 2.0f;

        glUniform3fv(modelProgKAmb, 1, lAmb);
        glUniform3fv(modelProgKAmb, 1, kAmb);
        glUniform3fv(modelProgLPos, 1, lPos);
        glUniform3fv(modelProgLDif, 1, lDif);
        glUniform3fv(modelProgLSpc, 1, lSpc);
        glUniform3fv(modelProgKDif, 1, kDif);
        glUniform3fv(modelProgKSpc, 1, kSpc);
        glUniform1f(modelProgKShn, kShn);
    }

    // 绘制 ply obj
    {
        // Set up vertex attributes
        glEnableVertexAttribArray(modelProgPosAttrib);
        glEnableVertexAttribArray(modelProgNormAttrib);

        glBindBuffer(GL_ARRAY_BUFFER, vertexDataBuf);
        glVertexAttribPointer(modelProgPosAttrib, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void*>(0));
        glVertexAttribPointer(modelProgNormAttrib, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void*>(3*sizeof(GLfloat)));

        glDrawArrays(GL_TRIANGLES, 0, faceIndexCount);
    }

    // 绘制 floor
    {
        static GLfloat floorKAmb[3] = {1.0f, 1.0f, 1.0f};
        static GLfloat floorKDif[3] = {1.0f, 1.0f, 1.0f};
        static GLfloat floorKSpc[3] = {1.0f, 1.0f, 1.0f};
        static GLfloat floorKShn = 2.0f;

        glUniform3fv(modelProgKAmb, 1, floorKAmb);
        glUniform3fv(modelProgKDif, 1, floorKDif);
        glUniform3fv(modelProgKSpc, 1, floorKSpc);
        glUniform1f(modelProgKShn, floorKShn);

        // 绑定一个顶点缓冲对象（VBO）。floorBuf是特定的缓冲区对象标识符。这使得后续的操作都针对这个特定的缓冲区进行。
        glBindBuffer(GL_ARRAY_BUFFER, floorBuf);
        // 定义顶点属性指针。phongProgPosAttrib是顶点位置属性的索引。3表示该顶点属性有三个分量，可能对应三维空间中的坐标（x、y、z）。
        // GL_FLOAT是顶点属性的数据类型为浮点数。GL_FALSE表示不进行归一化。
        // 6 * sizeof(GLfloat)是步长，即两个连续顶点属性之间的字节数。这里表示每六个浮点数（每个浮点数大小为sizeof(GLfloat)）为一个完整的顶点数据。
        // reinterpret_cast<void*>(0)表示位置属性在缓冲区中的偏移量为 0，即从缓冲区起始位置开始读取位置数据。
        glVertexAttribPointer(modelProgPosAttrib, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void*>(0));
        glVertexAttribPointer(modelProgNormAttrib, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void*>(3*sizeof(GLfloat)));
        // 使用顶点数组数据进行绘制。GL_TRIANGLES表示使用三角形的方式进行绘制。0是起始顶点的索引。6表示要绘制的顶点数量。这意味着将绘制两个三角形，因为每个三角形需要三个顶点。
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
    
    if (shadow) {
        // bind buffer
//        glBindFramebuffer(GL_FRAMEBUFFER, shadowbuffer); // 往 framebuffer 上画
        glBindFramebuffer(GL_FRAMEBUFFER, 0); // 往 framebuffer 上画
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, shadowDepthTexture, 0);
        GLenum bufs[1] = { GL_COLOR_ATTACHMENT0 };
        glDrawBuffers(1, bufs);
        glUseProgram(shadowProg);
        
        // set lighting uniform
        const static double pi = acos(0.0) * 2;
        Mat4 ident = Mat4::identityMatrix();
        Mat4 view, viewNorm;
        Mat4::lookAtMatrix(light_eye, lookat, Vec3(0, 1, 0), view, viewNorm);
        Mat4 proj = Mat4::perspectiveMatrix(pi * 0.5, 1.333333f, 0.1f, 1000.0f);
        
        Mat4 mvp = proj * view;
        glUniformMatrix4fv(shadowProgModelViewMat, 1, GL_FALSE, reinterpret_cast<float*>(&view));
        glUniformMatrix4fv(shadowProgMvpMat, 1, GL_FALSE, reinterpret_cast<float*>(&mvp));
        glUniformMatrix4fv(shadowProgNormalMat, 1, GL_FALSE, reinterpret_cast<float*>(&ident));
        
        // 使用lighting matrix 绘制 ply obj shadow
        glEnableVertexAttribArray(shadowProgPosAttrib);
        glEnableVertexAttribArray(shadowProgNormAttrib);
        
        glBindBuffer(GL_ARRAY_BUFFER, vertexDataBuf);
        glVertexAttribPointer(shadowProgPosAttrib, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void*>(0));
        glVertexAttribPointer(shadowProgNormAttrib, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void*>(3*sizeof(GLfloat)));
        
        glDrawArrays(GL_TRIANGLES, 0, faceIndexCount);
        
        // 使用lighting matrix 绘制 floor
        glBindBuffer(GL_ARRAY_BUFFER, floorBuf);
        glVertexAttribPointer(modelProgPosAttrib, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void*>(0));
        glVertexAttribPointer(modelProgNormAttrib, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void*>(3*sizeof(GLfloat)));
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void draw(std::function<void()> renderCallback) {
    static std::once_flag flag;
    std::call_once(flag, [] () {
        initializeResources();
        loadShaders();
        loadModel();
    });
    
    glClearColor(0, 0, 0, 0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);
    
    // If we're rendering with ambient occlusion
    drawModel(true);
}

}   // end sample

}   // end shadow

}   // end nav



