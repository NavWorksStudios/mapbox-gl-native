//
//  nav.shadow.cpp
//
//  Created by ZHM on 2024/10/11.
//

#include "mbgl/nav/nav.runtime.hpp"
#include "mbgl/nav/nav.style.hpp"
#include "mbgl/nav/render/nav.skybox.hpp"
#include "mbgl/nav/render/vec3.h"
#include "mbgl/nav/render/mat4.h"

#include "mbgl/nav/render/shaders.h"
#include "mbgl/nav/render/programs/nav.program.skybox.hpp"
#include "mbgl/nav/render/nav.glvalue.hpp"

static auto convertVec3 = [] (mbgl::vec3 v) {
    return Vec3((float) v[0], (float) v[1], (float) v[2]);
};

static auto convertMatrix4 = [] (mbgl::mat4 matrix) {
    Mat4 m;
    for (int i=0; i<16; i++) ((float*)&m)[i] = float(matrix[i]);
    return m;
};

namespace nav {
namespace render {
namespace skybox {

const static std::vector<std::string> faces {
    "skybox/right.jpg",
    "skybox/left.jpg",
    "skybox/top.jpg",
    "skybox/bottom.jpg",
    "skybox/front.jpg",
    "skybox/back.jpg"
};

unsigned int loadSkyboxTextureMap() {
    unsigned int textureID;
    
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    const auto skybox = nav::runtime::texture::skybox();
    const auto& size = std::get<0>(skybox);
    
    unsigned int width = size.width, height = size.height;
    auto data1 = std::get<1>(skybox);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, &data1);
    auto data2 = std::get<2>(skybox);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, &data2);
    auto data3 = std::get<3>(skybox);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, &data3);
    auto data4 = std::get<4>(skybox);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, &data4);
    auto data5 = std::get<5>(skybox);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, &data5);
    auto data6 = std::get<6>(skybox);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, &data6);
    
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    
    return textureID;
}

GLuint skyboxTexture;

void initResource(int width, int height) {
    static int w = 0, h = 0;
    if (w != width || h != height) {
        w = width;
        h = height;
        
        skyboxTexture = loadSkyboxTextureMap();
    }
}

//const int factor = 100000000.0;
const int factor = 1.0;
const static GLfloat skyboxVertices[] = {
    // positions
    -1.0f * factor,  1.0f * factor, -1.0f * factor,
    -1.0f * factor, -1.0f * factor, -1.0f * factor,
     1.0f * factor, -1.0f * factor, -1.0f * factor,
     1.0f * factor, -1.0f * factor, -1.0f * factor,
     1.0f * factor,  1.0f * factor, -1.0f * factor,
    -1.0f * factor,  1.0f * factor, -1.0f * factor,

    -1.0f * factor, -1.0f * factor,  1.0f * factor,
    -1.0f * factor, -1.0f * factor, -1.0f * factor,
    -1.0f * factor,  1.0f * factor, -1.0f * factor,
    -1.0f * factor,  1.0f * factor, -1.0f * factor,
    -1.0f * factor,  1.0f * factor,  1.0f * factor,
    -1.0f * factor, -1.0f * factor,  1.0f * factor,

     1.0f * factor, -1.0f * factor, -1.0f * factor,
     1.0f * factor, -1.0f * factor,  1.0f * factor,
     1.0f * factor,  1.0f * factor,  1.0f * factor,
     1.0f * factor,  1.0f * factor,  1.0f * factor,
     1.0f * factor,  1.0f * factor, -1.0f * factor,
     1.0f * factor, -1.0f * factor, -1.0f * factor,

    -1.0f * factor, -1.0f * factor,  1.0f * factor,
    -1.0f * factor,  1.0f * factor,  1.0f * factor,
     1.0f * factor,  1.0f * factor,  1.0f * factor,
     1.0f * factor,  1.0f * factor,  1.0f * factor,
     1.0f * factor, -1.0f * factor,  1.0f * factor,
    -1.0f * factor, -1.0f * factor,  1.0f * factor,

    -1.0f * factor,  1.0f * factor, -1.0f * factor,
     1.0f * factor,  1.0f * factor, -1.0f * factor,
     1.0f * factor,  1.0f * factor,  1.0f * factor,
     1.0f * factor,  1.0f * factor,  1.0f * factor,
    -1.0f * factor,  1.0f * factor,  1.0f * factor,
    -1.0f * factor,  1.0f * factor, -1.0f * factor,

    -1.0f * factor, -1.0f * factor, -1.0f * factor,
    -1.0f * factor, -1.0f * factor,  1.0f * factor,
     1.0f * factor, -1.0f * factor, -1.0f * factor,
     1.0f * factor, -1.0f * factor, -1.0f * factor,
    -1.0f * factor, -1.0f * factor,  1.0f * factor,
     1.0f * factor, -1.0f * factor,  1.0f * factor
};

GLuint skybox_program() {
    static GLint pass = createProgram(compileShader(GL_VERTEX_SHADER, nav::programs::skybox::vertexShader()),
                                      compileShader(GL_FRAGMENT_SHADER, nav::programs::skybox::fragmentShader()));
    return pass;
}

GLuint vao(GLuint program) {
    static GLuint vao = 0;
    if (!vao) {
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        
        static GLuint vbo;
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
        
        static programs::AttribLocation a0(program, "a_pos");
        glEnableVertexAttribArray(a0);
        glVertexAttribPointer(a0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), reinterpret_cast<void*>(0));
        
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    
    return vao;
}

void render(const mbgl::mat4& vp) {
    
    initResource(nav::display::pixels::width(), nav::display::pixels::height());
    
//    CullFace::Type cullFace = CullFace::Get();
    glDepthFunc(GL_LEQUAL);
    {
        
        const GLint program = skybox_program();
        Program::Set(program);
//        CullFace::Set(false);
        
        glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
        static programs::UniformLocation u0(program, "u_skybox");
        glUniform1i(u0, 0);
        
        static programs::UniformLocation u1(program, "u_matrix");
        const Mat4 VP = convertMatrix4(vp);
        glUniformMatrix4fv(u1, 1, GL_FALSE, reinterpret_cast<const float*>(&VP));
        
        glBindVertexArray(vao(program));
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
    glDepthFunc(GL_LESS);
//    CullFace::Set(cullFace);
}

}   // end skybox
}   // end render
}   // end nav



