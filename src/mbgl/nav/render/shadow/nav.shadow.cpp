//
//  nav.ssao.cpp
//
//  Created by BernieZhao on 2024/9/1.
//

#include "mbgl/nav/render/shadow/nav.shadow.hpp"

#include <random>

#include "mbgl/nav/render/shaders.h"
#include "mbgl/nav/render/vec3.h"

#include <mbgl/util/mat4.hpp>

#include <mbgl/programs/gl/nav.ssao.shader.hpp>


#include "mbgl/nav/render/ssao/v1/rply.h"

#include "mbgl/nav/nav.style.hpp"
#include "mbgl/nav/nav.palette.hpp"


#define SCR_WIDTH nav::display::pixels::width()
#define SCR_HEIGHT nav::display::pixels::height()


namespace nav {

namespace shadow {


// Framebuffers, textures to render to, renderbuffers, other textures
GLuint modelbuffer;
GLuint modelDepthTexture;
GLuint shadowDepthFBO;
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
    glGenFramebuffers(1, &shadowDepthFBO);
    
    // Setup a texture to render depth to
    glGenTextures(1, &shadowDepthTexture);
    glBindTexture(GL_TEXTURE_2D, shadowDepthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    float pF/*planeFactor*/ = 1.0;
    // Set up buffer for floor data, 12 pieses
    const static GLfloat floorData[36] = {
        -1.0f * pF, -0.4f, -1.0f * pF,
        0.0f, 1.0f, 0.0f,
        -1.0f * pF, -0.4f, 1.0f * pF,
        0.0f, 1.0f, 0.0f,
        1.0f * pF, -0.4f, 1.0f * pF,
        0.0f, 1.0f, 0.0f,
        
        1.0f * pF, -0.4f, 1.0f * pF,
        0.0f, 1.0f, 0.0f,
        1.0f * pF, -0.4f, -1.0f * pF,
        0.0f, 1.0f, 0.0f,
        -1.0f * pF, -0.4f, -1.0f * pF,
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

GLint shadowDebugDepth;

GLint shadowProgMapping;


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

void loadShaders() {
    
    // shadow shaders
//    modelProg = createProgram(loadShader(GL_VERTEX_SHADER, "/deprecated_shaders/modelProg.vert"),
//                              loadShader(GL_FRAGMENT_SHADER, "/deprecated_shaders/modelProg.frag"));
    
    // modelProg attribute
//    modelProgPosAttrib = glGetAttribLocation(modelProg, "aPos");
//    modelProgNormAttrib = glGetAttribLocation(modelProg, "aNormal");
    
    // shadowProg & attribute & uniform
    shadowProg = createProgram(loadShader(GL_VERTEX_SHADER, "/deprecated_shaders/shadowProg.vert"),
                              loadShader(GL_FRAGMENT_SHADER, "/deprecated_shaders/shadowProg.frag"));

    shadowProgPosAttrib = glGetAttribLocation(shadowProg, "positionIn");
    shadowProgNormAttrib = glGetAttribLocation(shadowProg, "normalIn");

    shadowProgModelViewMat = glGetUniformLocation(shadowProg, "modelViewMat");
    shadowProgMvpMat = glGetUniformLocation(shadowProg, "modelViewProjMat");
    shadowProgNormalMat = glGetUniformLocation(shadowProg, "normalMat");
    
    // shadowDebugDepth & attribute
    shadowDebugDepth =
    createProgram(loadShader(GL_VERTEX_SHADER, "/deprecated_shaders/shadow.quad.vs"),
                  loadShader(GL_FRAGMENT_SHADER, "/deprecated_shaders/shadow.quad.fs"));
    
    // shadowProgMapping & attribute
    shadowProgMapping =
    createProgram(loadShader(GL_VERTEX_SHADER, "/deprecated_shaders/shadow.mapping.vs"),
                  loadShader(GL_FRAGMENT_SHADER, "/deprecated_shaders/shadow.mapping.fs"));
    
    modelProgPosAttrib = glGetAttribLocation(shadowProgMapping, "aPos");
    modelProgNormAttrib = glGetAttribLocation(shadowProgMapping, "aNormal");
    
}

// the camera info
Vec3 eye = Vec3(0.25, 1.5, 0.25);
Vec3 lookat = Vec3(0, 0, 0);
Vec3 light_eye = Vec3(-0.75, 2.5, 0.75);

auto convertMatrix = [] (mbgl::mat4 matrix) {
    Mat4 m;
    for (int i=0; i<16; i++) ((float*)&m)[i] = float(matrix[i]);
    return m;
};

// renderQuad() renders a 1x1 XY quad in NDC
// -----------------------------------------
void renderQuad()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    glUseProgram(shadowDebugDepth);
    
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

        static AttribLocation a0(shadowDebugDepth, "aPos");
        glEnableVertexAttribArray(a0);
        glVertexAttribPointer(a0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        
        static AttribLocation a1(shadowDebugDepth, "aTexCoords");
        glEnableVertexAttribArray(a1);
        glVertexAttribPointer(a1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, shadowDepthTexture);
    static UniformLocation u0(shadowDebugDepth, "depthMap");
    glUniform1i(u0, 0);
    
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

Mat4 l_mvp;
void renderDepthBuf() {
    // bind buffer
    glBindFramebuffer(GL_FRAMEBUFFER, shadowDepthFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowDepthTexture, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    
    glClear(GL_DEPTH_BUFFER_BIT);
    
    glUseProgram(shadowProg);
    // set lighting uniform
    const static double pi = acos(0.0) * 2;
    Mat4 ident = Mat4::identityMatrix();
    Mat4 model = Mat4(1.0);
    Mat4 view, viewNorm;
    Mat4::lookAtMatrix(light_eye, lookat, Vec3(0, 1, 0), view, viewNorm);
    Mat4 proj = Mat4::perspectiveMatrix(pi * 0.5, 1.333333f, 1.0f, 7.5f);
    Mat4 mvp = proj * view;
    l_mvp = mvp;
    
//    GLfloat near_plane = 1.0f, far_plane = 7.5f;
//    mbgl::mat4 lP_m;
//    mbgl::matrix::ortho(lP_m, -10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
//    Mat4 mvp = convertMatrix(lP_m) * view;
    
    glUniformMatrix4fv(shadowProgMvpMat, 1, GL_FALSE, reinterpret_cast<float*>(&mvp));
    
    // 使用lighting matrix 绘制 ply obj depth
    glEnableVertexAttribArray(shadowProgPosAttrib);
    glBindBuffer(GL_ARRAY_BUFFER, vertexDataBuf);
    glVertexAttribPointer(shadowProgPosAttrib, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void*>(0));
    glDrawArrays(GL_TRIANGLES, 0, faceIndexCount);
    // 使用lighting matrix 绘制 floor depth
    glBindBuffer(GL_ARRAY_BUFFER, floorBuf);
    glVertexAttribPointer(shadowProgPosAttrib, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void*>(0));
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void renderModelWithShadow() {
    
    // render scene as normal using the generated depth/shadow map
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(shadowProgMapping);
    
    // set uniform
    {
        // 设置mat
        const static double pi = acos(0.0) * 2;
        Mat4 ident = Mat4::identityMatrix();
        Mat4 model = Mat4(1.0);
        Mat4 view, viewNorm;
        Mat4::lookAtMatrix(eye, lookat, Vec3(0, 1, 0), view, viewNorm);
        Mat4 proj = Mat4::perspectiveMatrix(pi * 0.5, 1.333333f, 1.0f, 7.5f);
        Mat4 mvp = proj * view;
        
        static UniformLocation u0(shadowProgMapping, "projection");
        glUniformMatrix4fv(u0, 1, GL_FALSE, reinterpret_cast<float*>(&proj));
        
        static UniformLocation u1(shadowProgMapping, "view");
        glUniformMatrix4fv(u1, 1, GL_FALSE, reinterpret_cast<float*>(&view));
        
        static UniformLocation u2(shadowProgMapping, "model");
        glUniformMatrix4fv(u2, 1, GL_FALSE, reinterpret_cast<float*>(&ident));
        
        static UniformLocation u3(shadowProgMapping, "lightSpaceMatrix");
        glUniformMatrix4fv(u3, 1, GL_FALSE, reinterpret_cast<float*>(&l_mvp));
        
        static UniformLocation u4(shadowProgMapping, "lightPos");
        glUniform3fv(u4, 1, reinterpret_cast<float*>(&light_eye));
        static UniformLocation u5(shadowProgMapping, "viewPos");
        glUniform3fv(u5, 1, reinterpret_cast<float*>(&eye));
        
//        static GLfloat textureSizeD[2] = {(float)SCR_WIDTH, (float)SCR_HEIGHT};
//        static UniformLocation u6(shadowProgMapping, "textureSizeD");
//        glUniform2fv(u6, 1, reinterpret_cast<float*>(&textureSizeD));
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, shadowDepthTexture);
        static UniformLocation u7(shadowProgMapping, "shadowMap");
        glUniform1i(u7, 0);
        

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
}

void drawModel(bool shadow)
{
#if 1 // render Depth value to Depth map
    renderDepthBuf();
#endif
    
#if 0
    // render Depth map to quad for visual debugging
    renderQuad();
#endif
    
#if 1
    // render Models With Shadow Depth info
    renderModelWithShadow();
#endif
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void draw(std::function<void()> renderCallback) {
    static std::once_flag flag;
    std::call_once(flag, [] () {
        initializeResources();
        loadShaders();
        loadModel();
    });
    
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // If we're rendering with ambient occlusion
    drawModel(true);
}

namespace fbo {

namespace dbuffer {
GLuint shadowDepthFBO;
GLuint shadowDepthTexture;

void generate(int width, int height) {
    if (!shadowDepthFBO)
        glGenFramebuffers(1, &shadowDepthFBO);
    // shadow depth buffer
    glDeleteTextures(1, &shadowDepthTexture);
    glGenTextures(1, &shadowDepthTexture);
    glBindTexture(GL_TEXTURE_2D, shadowDepthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

void bind() {
    glBindFramebuffer(GL_FRAMEBUFFER, shadowDepthFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowDepthTexture, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    
    glClear(GL_DEPTH_BUFFER_BIT);
}

}

void generate(int width, int height) {
    static int w = 0, h = 0;
    if (w != width || h != height) {
        w = width;
        h = height;
        
        dbuffer::generate(w, h);
    }
}

}

void renderDBuffer(const Mat4& lightMatrix,
                   std::function<void()> renderCallback,
                   std::function<void()> bindScreenFbo = nullptr) {
    
    if (bindScreenFbo) bindScreenFbo();
    else fbo::dbuffer::bind();
    
    glDisable(GL_BLEND);
    
    GLfloat color[4];
    glGetFloatv(GL_COLOR_CLEAR_VALUE, color);
    
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    static GLint program = 0;
    if (program) {
        static programs::UniformLocation u0(program, "u_lightMatrix");
        glUniformMatrix4fv(u0, 1, GL_FALSE, reinterpret_cast<const float*>(&lightMatrix));
    }
    
    renderCallback();
    
    glGetIntegerv(GL_CURRENT_PROGRAM, &program);
    glClearColor(color[0], color[1], color[2], color[3]);
    
}

GLuint renderShadowDepthBuffer(int width, int height, const Mat4& lightMatrix, std::function<void()> renderCallback) {
    fbo::generate(width, height);
    renderDBuffer(lightMatrix, renderCallback);
    return fbo::dbuffer::shadowDepthTexture;
}


}   // end shadow

}   // end nav



