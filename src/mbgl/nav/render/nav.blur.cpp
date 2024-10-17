//
//  nav.blur.cpp
//  Mapbox GL Native
//
//  Created by BernieZhao on 2024/10/10.
//

#include "mbgl/nav/render/nav.blur.hpp"

#include "mbgl/nav/render/nav.render.hpp"
#include "mbgl/nav/render/shaders.h"

#include <mbgl/programs/gl/nav.ssao.shader.hpp>

namespace nav {
namespace blur {

GLint program() {
    static GLint pass = 0;
    if (!pass) {
        pass =
        createProgram(compileShader(GL_VERTEX_SHADER, nav::programs::ssao::vertexShader()),
                      compileShader(GL_FRAGMENT_SHADER, nav::programs::ssao::blurFragmentShader()));
    }
    
    return pass;
}

void render(GLint buffer, int width, int height) {

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    GLint program = blur::program();
    glUseProgram(program);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, buffer);
    static programs::UniformLocation u0(program, "u_ssao");
    glUniform1i(u0, 0);
    
    static programs::UniformLocation u1(program, "u_offset[0]");
    glUniform2f(u1, 1.1 / width, 1.1 / height);
    
//    static programs::UniformLocation u2(program, "u_offset[1]");
//    glUniform2f(u2, 2. / width, 2. / height);
    
//    static programs::UniformLocation u3(program, "u_offset[2]");
//    glUniform2f(u3, 2.9 / width, 2.9 / height);

    nav::render::util::renderQuad(program);

}

}
}
