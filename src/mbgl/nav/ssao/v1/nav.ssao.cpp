//
//  nav.ssao.cpp
//
//  Created by BernieZhao on 2024/9/6.
//

#include "mbgl/nav/ssao/v1/nav.ssao.hpp"

#include "mbgl/nav/ssao/v1/vec3.h"
#include "mbgl/nav/ssao/v1/mat4.h"
#include "mbgl/nav/ssao/v1/shaders.h"
#include "mbgl/nav/ssao/v1/rply.h"

#include "mbgl/nav/nav.style.hpp"


#define wWidth nav::display::width() * 2
#define wHeight nav::display::height() * 2


namespace nav {
namespace ssao {


// Shader programs and attrib/uniform locations
GLuint phongProg;
GLint phongProgPosAttrib;
GLint phongProgNormAttrib;
GLint phongProgModelViewMat;
GLint phongProgMvpMat;
GLint phongProgNormalMat;

GLint phongProgLAmb;
GLint phongProgKAmb;

GLint phongProgLPos;

GLint phongProgLDif;
GLint phongProgLSpc;

GLint phongProgKDif;
GLint phongProgKSpc;
GLint phongProgKShn;

GLint phongProgDoSSAO;

GLuint aoProg;
GLint aoProgPosAttrib;
GLint aoProgDepthTexture;
GLint aoProgNormTexture;
GLint aoProgRandomTexture;
GLint aoProgProjMat;
GLint aoProgInvProjMat;
GLint aoProgSampleOffsets;
GLint aoProgSampleRadius;
GLint aoProgRandomTexCoordScale;

GLuint blurProg;
GLint blurProgPosAttrib;
GLint blurProgAoTexture;
GLint blurProgColorTexture;
GLint blurProgInvRes;

void loadShaders() {
    // Phong shaders
    phongProg = createProgram(loadShader(GL_VERTEX_SHADER, "/shaders/phong.vert"),
                              loadShader(GL_FRAGMENT_SHADER, "/shaders/phong.frag"));
    
    phongProgPosAttrib = glGetAttribLocation(phongProg, "positionIn");
    phongProgNormAttrib = glGetAttribLocation(phongProg, "normalIn");
    phongProgModelViewMat = glGetUniformLocation(phongProg, "modelViewMat");
    phongProgMvpMat = glGetUniformLocation(phongProg, "modelViewProjMat");
    phongProgNormalMat = glGetUniformLocation(phongProg, "normalMat");
    
    phongProgLAmb = glGetUniformLocation(phongProg, "lAmbient");
    phongProgKAmb = glGetUniformLocation(phongProg, "kAmbient");
    
    phongProgLPos = glGetUniformLocation(phongProg, "lPosition");
    
    phongProgLDif = glGetUniformLocation(phongProg, "lDiffuse");
    phongProgLSpc = glGetUniformLocation(phongProg, "lSpecular");

    phongProgKDif = glGetUniformLocation(phongProg, "kDiffuse");
    phongProgKSpc = glGetUniformLocation(phongProg, "kSpecular");
    phongProgKShn = glGetUniformLocation(phongProg, "kShininess");
    
    phongProgDoSSAO = glGetUniformLocation(phongProg, "doSSAO");
    
    // Ambient occlusion shaders
    aoProg = createProgram(loadShader(GL_VERTEX_SHADER, "/shaders/ssao.vert"),
                           loadShader(GL_FRAGMENT_SHADER, "/shaders/ssao.frag"));
    
    aoProgPosAttrib = glGetAttribLocation(aoProg, "positionIn");
    aoProgDepthTexture = glGetUniformLocation(aoProg, "depthTex");
    aoProgNormTexture = glGetUniformLocation(aoProg, "normTex");
    aoProgRandomTexture = glGetUniformLocation(aoProg, "randomTex");
    aoProgProjMat = glGetUniformLocation(aoProg, "projMat");
    aoProgInvProjMat = glGetUniformLocation(aoProg, "invProjMat");
    aoProgSampleOffsets = glGetUniformLocation(aoProg, "sampleOffsets");
    aoProgSampleRadius = glGetUniformLocation(aoProg, "sampleRadius");
    aoProgRandomTexCoordScale = glGetUniformLocation(aoProg, "randomTexCoordScale");
    
    // Blur texture shaders
    blurProg = createProgram(loadShader(GL_VERTEX_SHADER, "/shaders/blur.vert"),
                             loadShader(GL_FRAGMENT_SHADER, "/shaders/blur.frag"));
    
    blurProgPosAttrib = glGetAttribLocation(blurProg, "positionIn");
    blurProgAoTexture = glGetUniformLocation(blurProg, "aoTex");
    blurProgColorTexture = glGetUniformLocation(blurProg, "colorTex");
    blurProgInvRes = glGetUniformLocation(blurProg, "invRes");
    
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


// the camera info
Vec3 eye = Vec3(0., 1.5, 1.5);
Vec3 lookat = Vec3(0, 0, 0);

// Program functionality variables
// Shading related
float depthDiscontinuityRadius = 10.f;

// Framebuffers, textures to render to, renderbuffers, other textures
GLuint framebuffer;
GLuint depthTexture;
GLuint colorTexture;
GLuint normalTexture;
GLuint aoTexture;
GLuint depthRenderbuffer;
GLuint randomTexture;


void initializeResources()
{
    // Enable and configure textures on applicable texture units
    glActiveTexture(GL_TEXTURE0);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    
    // Create our framebuffer
    glGenFramebuffers(1, &framebuffer);
    
    // Setup a texture to render depth to
    glGenTextures(1, &depthTexture);
    glBindTexture(GL_TEXTURE_2D, depthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, wWidth, wHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    // Setup a texture to render color to
    glGenTextures(1, &colorTexture);
    glBindTexture(GL_TEXTURE_2D, colorTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, wWidth, wHeight, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    // Setup a texture to store normals in
    glGenTextures(1, &normalTexture);
    glBindTexture(GL_TEXTURE_2D, normalTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, wWidth, wHeight, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    // Setup a texture to store occlusion values in
    glGenTextures(1, &aoTexture);
    glBindTexture(GL_TEXTURE_2D, aoTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, wWidth, wHeight, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    // Setup a renderbuffer to use for depth
    glGenRenderbuffers(1, &depthRenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthRenderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, wWidth, wHeight);
    
    // Set up a texture used to store random sample offset values
    glGenTextures(1, &randomTexture);
    glBindTexture(GL_TEXTURE_2D, randomTexture);
    // All these were calculated offline, the values may look weird because they're
    // already scaled and biased from [-1, 1] to [0, 1].
    static const GLfloat randomDirections[48] = { 0.682909, 0.965344, 0.500000,
        0.164830, 0.871026, 0.500000, 0.896964, 0.804005, 0.500000, 0.127030, 0.833006, 0.500000,
        0.960280, 0.695300, 0.500000, 0.198356, 0.898761, 0.500000, 0.245320, 0.069723, 0.500000,
        0.928136, 0.241738, 0.500000, 0.704112, 0.956441, 0.500000, 0.984019, 0.374598, 0.500000,
        0.171691, 0.122888, 0.500000, 0.806018, 0.895415, 0.500000, 0.951012, 0.715844, 0.500000,
        0.633597, 0.981822, 0.500000, 0.173631, 0.878791, 0.500000, 0.338373, 0.973156, 0.500000 };
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 4, 4, 0, GL_RGB, GL_FLOAT, randomDirections);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
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


void drawModel(bool ssao)
{
    // switch frame buffer
    if (ssao) {
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer); // 往 framebuffer 上画
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTexture, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, normalTexture, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);
        GLenum bufs[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
        glDrawBuffers(2, bufs);
    } else {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(phongProg);
    
    // set uniform
    {
        // 设置mat
        const static double pi = acos(0.0) * 2;
        Mat4 ident = Mat4::identityMatrix();
        Mat4 view, viewNorm;
        Mat4::lookAtMatrix(eye, lookat, Vec3(0, 1, 0), view, viewNorm);
        Mat4 proj = Mat4::perspectiveMatrix(pi * 0.5, 1.333333f, 0.1f, 1000.0f);
        
        Mat4 mvp = proj * view;
        glUniformMatrix4fv(phongProgModelViewMat, 1, GL_FALSE, reinterpret_cast<float*>(&view));
        glUniformMatrix4fv(phongProgMvpMat, 1, GL_FALSE, reinterpret_cast<float*>(&mvp));
        glUniformMatrix4fv(phongProgNormalMat, 1, GL_FALSE, reinterpret_cast<float*>(&ident));

        // 设置 Lighting uniforms
        static GLfloat lAmb[3] = {1.0f, 1.0f, 1.0f}; // 环境光颜色
        static GLfloat kAmb[3] = {0.6f, 0.6f, 0.6f};
        
        static GLfloat lPos[3] = {0.0f, 0.0f, 0.5f};
        
        static GLfloat lDif[3] = {1.0f, 1.0f, 1.0f}; // 漫反光颜色
        static GLfloat kDif[3] = {0.3f, 0.3f, 0.3f};
        
        static GLfloat lSpc[3] = {1.0f, 1.0f, 1.0f}; // 镜面光颜色
        static GLfloat kSpc[3] = {0.01f, 0.01f, 0.01f};
        static GLfloat kShn = 0.4f;
        
        glUniform3fv(phongProgLAmb, 1, lAmb);
        glUniform3fv(phongProgKAmb, 1, kAmb);
        
        glUniform3fv(phongProgLPos, 1, lPos);
        
        glUniform3fv(phongProgLDif, 1, lDif);
        glUniform3fv(phongProgLSpc, 1, lSpc);
        
        glUniform3fv(phongProgKDif, 1, kDif);
        glUniform3fv(phongProgKSpc, 1, kSpc);
        glUniform1f(phongProgKShn, kShn);
        
        // 设置 ao 开关
        glUniform1f(phongProgDoSSAO, ssao ? 1.0f : 0.0f);
    }
    
    // 绘制 ply obj
    {
        // Set up vertex attributes
        glEnableVertexAttribArray(phongProgPosAttrib);
        glEnableVertexAttribArray(phongProgNormAttrib);
        
        glBindBuffer(GL_ARRAY_BUFFER, vertexDataBuf);
        glVertexAttribPointer(phongProgPosAttrib, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void*>(0));
        glVertexAttribPointer(phongProgNormAttrib, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void*>(3*sizeof(GLfloat)));
        
        glDrawArrays(GL_TRIANGLES, 0, faceIndexCount);
    }
    
    // 绘制 floor
    {
        static GLfloat floorKAmb[3] = {1.0f, 1.0f, 1.0f};
        static GLfloat floorKDif[3] = {1.0f, 1.0f, 1.0f};
        static GLfloat floorKSpc[3] = {1.0f, 1.0f, 1.0f};
        static GLfloat floorKShn = 2.0f;

        glUniform3fv(phongProgKAmb, 1, floorKAmb);
        glUniform3fv(phongProgKDif, 1, floorKDif);
        glUniform3fv(phongProgKSpc, 1, floorKSpc);
        glUniform1f(phongProgKShn, floorKShn);
        
        // 绑定一个顶点缓冲对象（VBO）。floorBuf是特定的缓冲区对象标识符。这使得后续的操作都针对这个特定的缓冲区进行。
        glBindBuffer(GL_ARRAY_BUFFER, floorBuf);
        // 定义顶点属性指针。phongProgPosAttrib是顶点位置属性的索引。3表示该顶点属性有三个分量，可能对应三维空间中的坐标（x、y、z）。
        // GL_FLOAT是顶点属性的数据类型为浮点数。GL_FALSE表示不进行归一化。
        // 6 * sizeof(GLfloat)是步长，即两个连续顶点属性之间的字节数。这里表示每六个浮点数（每个浮点数大小为sizeof(GLfloat)）为一个完整的顶点数据。
        // reinterpret_cast<void*>(0)表示位置属性在缓冲区中的偏移量为 0，即从缓冲区起始位置开始读取位置数据。
        glVertexAttribPointer(phongProgPosAttrib, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void*>(0));
        glVertexAttribPointer(phongProgNormAttrib, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void*>(3*sizeof(GLfloat)));
        // 使用顶点数组数据进行绘制。GL_TRIANGLES表示使用三角形的方式进行绘制。0是起始顶点的索引。6表示要绘制的顶点数量。这意味着将绘制两个三角形，因为每个三角形需要三个顶点。
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

}

void doSSAO()
{
    // All these were calculated offline
    const static GLfloat offsets[48] = {
        -0.030026, -0.091874, 0.025644,
        0.005745, -0.060426, 0.083852,
        0.110698, -0.025912, 0.009211, 
        -0.066202, 0.109803, 0.029828,
        0.005172, 0.112933, 0.107859,
        0.098871, -0.094032, 0.129172, 
        -0.116010, -0.168980, 0.096531,
        0.232979, 0.061169, 0.126916,
        -0.243828, -0.177320, 0.121368, 
        0.079705, -0.237290, 0.292208,
        -0.333035, 0.151770, 0.264504,
        -0.027741, -0.338065, 0.401220, 
        0.421871, -0.422317, 0.105886,
        -0.619888, -0.288012, 0.120912,
        -0.485098, 0.605901, 0.142069, 
        -0.783585, 0.276209, 0.321887 };
    
    // switch frame buffer
    {
        // 绑定一个帧缓冲对象。framebuffer是一个特定的帧缓冲对象标识符。这使得后续的操作都针对这个特定的帧缓冲对象进行。
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        // 设置绘制缓冲区为颜色附件 0。这指定了渲染结果将被写入到帧缓冲对象的哪个颜色附件中。
        glDrawBuffer(GL_COLOR_ATTACHMENT0);
        
        // 将一个二维纹理（aoTexture）附着到当前绑定的帧缓冲对象的颜色附件 0 上。这样，渲染结果将被写入到这个纹理中。
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, aoTexture, 0);
        
        // 对于颜色附件 1，不附着任何纹理。这里将其设置为 0，表示没有纹理与之关联。
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, 0, 0);
        
        // Detach any depth textures
        // 从帧缓冲对象的深度附件中分离任何已附着的二维纹理。这确保深度测试使用的是渲染缓冲区而不是纹理。
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, 0, 0);
        // 将一个渲染缓冲区（depthRenderbuffer）附着到帧缓冲对象的深度附件上。这通常用于存储深度信息，以便进行深度测试和遮挡剔除。
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderbuffer);
    }
    
    // 清除颜色缓冲区和深度缓冲区
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glUseProgram(aoProg);
    
    // bind texture
    {
        // 激活纹理单元 0。
        glActiveTexture(GL_TEXTURE0);
        // 激活纹理单元 0。在 OpenGL 中，可以同时使用多个纹理单元，每个纹理单元可以绑定不同的纹理。这里激活了第一个纹理单元。
        glBindTexture(GL_TEXTURE_2D, depthTexture);
        
        // 激活纹理单元 1。
        glActiveTexture(GL_TEXTURE1);
        // 将名为normalTexture的二维纹理绑定到纹理单元 1。这样在渲染时可以根据需要切换到这个纹理来获取法线信息。
        glBindTexture(GL_TEXTURE_2D, normalTexture);
        
        // 激活纹理单元 2。
        glActiveTexture(GL_TEXTURE2);
        // 将名为randomTexture的二维纹理绑定到纹理单元 2。这个纹理可能用于提供随机数据或其他特定的用途。
        glBindTexture(GL_TEXTURE_2D, randomTexture);
        
        // 最后再次激活纹理单元 0。这可能是为了确保在后续的操作中，主要的纹理单元处于激活状态，或者是为了方便后续对第一个纹理的操作。
        glActiveTexture(GL_TEXTURE0);
    }

    // set uniform
    {
        static const double pi = acos(0.0) * 0.5;
        Mat4 projMat = Mat4::perspectiveMatrix(pi * 0.5, 1.333333f, 0.1f, 1000.0f);
        Mat4 invProjMat = Mat4::perspectiveInvMatrix(pi * 0.5, 1.3333333f, 0.1f, 1000.0f);
        
        glUniform1i(aoProgDepthTexture, 0);
        glUniform1i(aoProgNormTexture, 1);
        glUniform1i(aoProgRandomTexture, 2);
        glUniform2f(aoProgRandomTexCoordScale, wWidth / 4, wHeight / 4);
        
        glUniformMatrix4fv(aoProgProjMat, 1, GL_FALSE, reinterpret_cast<GLfloat*>(&projMat));
        glUniformMatrix4fv(aoProgInvProjMat, 1, GL_FALSE, reinterpret_cast<GLfloat*>(&invProjMat));
        
        glUniform3fv(aoProgSampleOffsets, 16, offsets);
        glUniform1f(aoProgSampleRadius, depthDiscontinuityRadius);
    }
    
    // draw ao
    {
        // (x, y)
        static const float verts[8] = {
            1.0f, 1.0f,
            -1.0f, 1.0f,
            1.0f, -1.0f,
            -1.0f, -1.0f };
        
        // 启用顶点属性数组。
        glEnableVertexAttribArray(aoProgPosAttrib);
        // 绑定顶点缓冲对象（VBO）。这里将GL_ARRAY_BUFFER绑定到索引为 0 的缓冲区，通常意味着解除之前绑定的任何顶点缓冲对象。
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // 定义顶点属性指针。2表示该顶点属性的大小（这里表示有两个分量，可能是二维坐标）。GL_FLOAT是顶点属性的数据类型。 GL_FALSE表示是否进行归一化。0表示顶点属性之间的偏移量。
        glVertexAttribPointer(aoProgPosAttrib, 2, GL_FLOAT, GL_FALSE, 0, verts);
        
        // 纹理贴图 GL_TRIANGLE_STRIP 规则比较复杂
        // index (0,1,2) (2,1,3)
        // { (1,1),(-1,1),(1,-1) }, { (1,-1),(-1,1),(-1,-1) }
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    }
}

void doBlur()
{
    // switch frame buffer to draw
    {
        // Actually render to the screen
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glUseProgram(blurProg);
    
    // bind texture aoTexture & colorTexture
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, aoTexture);
        
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, colorTexture);
        
        glActiveTexture(GL_TEXTURE0);
    }
    
    // set uniform values
    {
        glUniform1i(blurProgAoTexture, 0);
        glUniform1i(blurProgColorTexture, 1);
        glUniform2f(blurProgInvRes, 1.0f / wWidth, 1.0f / wHeight);
    }
    
    // draw blur
    {
        static const float verts[8] = { 
            1.0f, 1.0f,
            -1.0f, 1.0f,
            1.0f, -1.0f,
            -1.0f, -1.0f };
        
        glEnableVertexAttribArray(blurProgPosAttrib);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glVertexAttribPointer(blurProgPosAttrib, 2, GL_FLOAT, GL_FALSE, 0, verts);
        
        // 纹理贴图 GL_TRIANGLE_STRIP 规则比较复杂
        // index (0,1,2) (2,1,3)
        // { (1,1),(-1,1),(1,-1) }, { (1,-1),(-1,1),(-1,-1) }
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }
}

namespace v1 {

void draw() {
    static bool initized = false;
    if (!initized) {
        initized = true;
        
        initializeResources();
        loadShaders();
        loadModel();
    }
    
    glClearColor(0, 0, 0, 0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);
    
    // If we're rendering with ambient occlusion
    if (1) {
        drawModel(true);
        doSSAO();
        doBlur();
    } else {
        drawModel(false);
    }
}

}


}
}
