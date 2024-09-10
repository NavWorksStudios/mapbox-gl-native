#pragma once

#include <OpenGL/OpenGL.h>
#include <OpenGL/gl.h>
#include <OpenGL/glext.h>

enum {
    GL_RGB16F = GL_RGB16F_ARB,
    GL_RGBA16F = GL_RGBA16F_ARB,
};

inline void glGenVertexArrays(GLsizei n, GLuint *ids) {
    glGenVertexArraysAPPLE(n, ids);
}

inline void glBindVertexArray(GLuint id) {
    glBindVertexArrayAPPLE(id);
}

