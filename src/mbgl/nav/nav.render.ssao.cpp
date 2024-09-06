//
//  nav.render.ssao.cpp
//
//  Created by BernieZhao on 2024/9/6.
//

#include "mbgl/nav/nav.render.ssao.hpp"
#include "mbgl/nav/vec3.h"
#include "mbgl/nav/mat4.h"
#include "mbgl/nav/nav.style.hpp"

//#include <vector>
//#include <random>
//#include <iostream>


#define wWidth nav::display::width()
#define wHeight nav::display::height()


namespace nav {
namespace render {
namespace ssao {

// the camera info
Vec3 eye;
Vec3 lookat;

// Program functionality variables
// Shading related
int ambientOcclusionState;
float depthDiscontinuityRadius;

// Framebuffers, textures to render to, renderbuffers, other textures
GLuint framebuffer;
GLuint depthTexture;
GLuint colorTexture;
GLuint normalTexture;
GLuint aoTexture;
GLuint depthRenderbuffer;
GLuint randomTexture;

GLuint vertexDataBuf;
GLuint floorBuf;

GLsizei faceIndexCount;



void initSSAO() {
    glEnable(GL_DEPTH_TEST);
    
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);
    
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
    
    // Set up buffer for floor data
    const static GLfloat floorData[36] = { -10.0f, -0.4f, -10.0f, 0.0f, 1.0f, 0.0f,
        -10.0f, -0.4f, 10.0f, 0.0f, 1.0f, 0.0f, 10.0f, -0.4f, 10.0f, 0.0f, 1.0f, 0.0f,
        10.0f, -0.4f, 10.0f, 0.0f, 1.0f, 0.0f, 10.0f, -0.4f, -10.0f, 0.0f, 1.0f, 0.0f,
        -10.0f, -0.4f, -10.0f, 0.0f, 1.0f, 0.0f};
    
    glGenBuffers(1, &floorBuf);
    glBindBuffer(GL_ARRAY_BUFFER, floorBuf);
    glBufferData(GL_ARRAY_BUFFER, 36 * sizeof(GLfloat), floorData, GL_STATIC_DRAW);
}


// Shader programs and attrib/uniform locations
GLuint phongProg;
GLint phongProgPosAttrib;
GLint phongProgNormAttrib;
GLint phongProgModelViewMat;
GLint phongProgMvpMat;
GLint phongProgNormalMat;
GLint phongProgLAmb;
GLint phongProgLPos;
GLint phongProgLDif;
GLint phongProgLSpc;
GLint phongProgKAmb;
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

GLuint loadShader(GLenum shaderType, const char* filename) {
    return 0;
}

GLuint createProgram(GLuint vertexShader, GLuint fragShader) {
    return 0;
}

void loadShaders() {
    // Phong shaders
    phongProg = createProgram(loadShader(GL_VERTEX_SHADER, "shaders/phong.vert"),
                              loadShader(GL_FRAGMENT_SHADER, "shaders/phong.frag"));
    
    phongProgPosAttrib = glGetAttribLocation(phongProg, "positionIn");
    phongProgNormAttrib = glGetAttribLocation(phongProg, "normalIn");
    phongProgModelViewMat = glGetUniformLocation(phongProg, "modelViewMat");
    phongProgMvpMat = glGetUniformLocation(phongProg, "modelViewProjMat");
    phongProgNormalMat = glGetUniformLocation(phongProg, "normalMat");
    phongProgLAmb = glGetUniformLocation(phongProg, "lAmbient");
    phongProgLPos = glGetUniformLocation(phongProg, "lPosition");
    phongProgLDif = glGetUniformLocation(phongProg, "lDiffuse");
    phongProgLSpc = glGetUniformLocation(phongProg, "lSpecular");
    phongProgKAmb = glGetUniformLocation(phongProg, "kAmbient");
    phongProgKDif = glGetUniformLocation(phongProg, "kDiffuse");
    phongProgKSpc = glGetUniformLocation(phongProg, "kSpecular");
    phongProgKShn = glGetUniformLocation(phongProg, "kShininess");
    phongProgDoSSAO = glGetUniformLocation(phongProg, "doSSAO");
    
    // Ambient occlusion shaders
    aoProg = createProgram(loadShader(GL_VERTEX_SHADER, "shaders/ssao.vert"),
                           loadShader(GL_FRAGMENT_SHADER, "shaders/ssao.frag"));
    
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
    blurProg = createProgram(loadShader(GL_VERTEX_SHADER, "shaders/blur.vert"),
                             loadShader(GL_FRAGMENT_SHADER, "shaders/blur.frag"));
    
    blurProgPosAttrib = glGetAttribLocation(blurProg, "positionIn");
    blurProgAoTexture = glGetUniformLocation(blurProg, "aoTex");
    blurProgColorTexture = glGetUniformLocation(blurProg, "colorTex");
    blurProgInvRes = glGetUniformLocation(blurProg, "invRes");
    
}

void drawModel() {
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTexture, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, normalTexture, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);
    GLenum bufs[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, bufs);
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glUseProgram(phongProg);

    const static double pi = acos(0.0) * 2;
    Mat4 ident = Mat4::identityMatrix();
    Mat4 view, viewNorm;
    Mat4::lookAtMatrix(eye, lookat, Vec3(0, 1, 0), view, viewNorm);
    Mat4 proj = Mat4::perspectiveMatrix(pi * 0.5, 1.333333f, 0.1f, 1000.0f);
    Mat4 mvp = proj * view;
    glUniformMatrix4fv(phongProgModelViewMat, 1, GL_FALSE, reinterpret_cast<float*>(&view));
    glUniformMatrix4fv(phongProgMvpMat, 1, GL_FALSE, reinterpret_cast<float*>(&mvp));
    glUniformMatrix4fv(phongProgNormalMat, 1, GL_FALSE, reinterpret_cast<float*>(&ident));

    // Lighting uniforms
    static GLfloat lAmb[3] = {1.0f, 1.0f, 1.0f};
    static GLfloat lPos[3] = {0.0f, 0.0f, 0.5f};
    static GLfloat lDif[3] = {1.0f, 1.0f, 1.0f};
    static GLfloat lSpc[3] = {0.3f, 0.3f, 0.3f};
    static GLfloat kAmb[3] = {0.2f, 0.1f, 0.0f};
    static GLfloat kDif[3] = {0.6f, 0.2f, 0.1f};
    static GLfloat kSpc[3] = {0.0f, 0.0f, 0.0f};
    static GLfloat kShn = 0.0f;
    glUniform3fv(phongProgLAmb, 1, lAmb);
    glUniform3fv(phongProgLPos, 1, lPos);
    glUniform3fv(phongProgLDif, 1, lDif);
    glUniform3fv(phongProgLSpc, 1, lSpc);
    glUniform3fv(phongProgKAmb, 1, kAmb);
    glUniform3fv(phongProgKDif, 1, kDif);
    glUniform3fv(phongProgKSpc, 1, kSpc);
    glUniform1f(phongProgKShn, kShn);

    glUniform1f(phongProgDoSSAO, 1.0f);

    // Set up vertex attributes
    glEnableVertexAttribArray(phongProgPosAttrib);
    glEnableVertexAttribArray(phongProgNormAttrib);

    glBindBuffer(GL_ARRAY_BUFFER, vertexDataBuf);
    glVertexAttribPointer(phongProgPosAttrib, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void*>(0));
    glVertexAttribPointer(phongProgNormAttrib, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void*>(3*sizeof(GLfloat)));

    glDrawArrays(GL_TRIANGLES, 0, faceIndexCount);

    static GLfloat floorKAmb[3] = {1.0f, 1.0f, 1.0f};
    static GLfloat floorKDif[3] = {1.0f, 1.0f, 1.0f};
    static GLfloat floorKSpc[3] = {1.0f, 1.0f, 1.0f};
    static GLfloat floorKShn = 2.0f;
    glUniform3fv(phongProgKAmb, 1, floorKAmb);
    glUniform3fv(phongProgKDif, 1, floorKDif);
    glUniform3fv(phongProgKSpc, 1, floorKSpc);
    glUniform1f(phongProgKShn, floorKShn);

    // Now draw the floor
    glBindBuffer(GL_ARRAY_BUFFER, floorBuf);
    glVertexAttribPointer(phongProgPosAttrib, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void*>(0));
    glVertexAttribPointer(phongProgNormAttrib, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void*>(3*sizeof(GLfloat)));
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void doSSAO() {
    // All these were calculated offline
    const static GLfloat offsets[48] = { -0.030026, -0.091874, 0.025644, 0.005745, -0.060426, 0.083852,
        0.110698, -0.025912, 0.009211, -0.066202, 0.109803, 0.029828, 0.005172, 0.112933, 0.107859,
        0.098871, -0.094032, 0.129172, -0.116010, -0.168980, 0.096531, 0.232979, 0.061169, 0.126916,
        -0.243828, -0.177320, 0.121368, 0.079705, -0.237290, 0.292208, -0.333035, 0.151770, 0.264504,
        -0.027741, -0.338065, 0.401220, 0.421871, -0.422317, 0.105886, -0.619888, -0.288012, 0.120912,
        -0.485098, 0.605901, 0.142069, -0.783585, 0.276209, 0.321887 };

    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, aoTexture, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, 0, 0);
    // Detach any depth textures
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, 0, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderbuffer);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glUseProgram(aoProg);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, depthTexture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, normalTexture);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, randomTexture);
    glActiveTexture(GL_TEXTURE0);

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

    static const float verts[8] = { 1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f, -1.0f, -1.0f };

    glEnableVertexAttribArray(aoProgPosAttrib);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glVertexAttribPointer(aoProgPosAttrib, 2, GL_FLOAT, GL_FALSE, 0, verts);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void doBlur() {
    // Actually render to the screen
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glUseProgram(blurProg);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, aoTexture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, colorTexture);
    glActiveTexture(GL_TEXTURE0);

    glUniform1i(blurProgAoTexture, 0);
    glUniform1i(blurProgColorTexture, 1);

    glUniform2f(blurProgInvRes, 1.0f / wWidth, 1.0f / wHeight);

    static const float verts[8] = { 1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f, -1.0f, -1.0f };

    glEnableVertexAttribArray(blurProgPosAttrib);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glVertexAttribPointer(blurProgPosAttrib, 2, GL_FLOAT, GL_FALSE, 0, verts);
    
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}


}
}
}
