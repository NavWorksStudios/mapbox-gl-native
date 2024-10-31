//
//  nav.render.cpp
//  Mapbox GL Native
//
//  Created by BernieZhao on 2024/10/10.
//

#include "mbgl/nav/render/nav.render.hpp"

#include "mbgl/nav/nav.style.hpp"

#include "mbgl/nav/render/shaders.h"
#include "mbgl/nav/render/vec3.h"
#include "mbgl/nav/render/mat4.h"

#include "mbgl/nav/render/nav.shadow.hpp"
#include "mbgl/nav/render/nav.ssao.hpp"
#include "mbgl/nav/render/nav.quad.hpp"
#include "mbgl/nav/render/programs/nav.program.hpp"

#include <mbgl/programs/nav_fill_extrusion_ssao_program.hpp>
#include <mbgl/programs/nav_fill_extrusion_shadow_program.hpp>


namespace nav {

static auto convertVec3 = [] (mbgl::vec3 v) {
    return Vec3((float) v[0], (float) v[1], (float) v[2]);
};

static auto convertMatrix4 = [] (mbgl::mat4 matrix) {
    Mat4 m;
    for (int i=0; i<16; i++) ((float*)&m)[i] = float(matrix[i]);
    return m;
};


namespace renderer {

GLint depthBuffer = 0;

namespace ground {

// Set up buffer for floor data, 6 triangles
const static GLfloat vertices[36] = {
    0.0f, 8192.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    8192.0f, 8192.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    
    8192.0f, 8192.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    8192.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f };

GLuint program() {
    static GLint pass = 0;
    if (!pass) {
        pass =
        createProgram(compileShader(GL_VERTEX_SHADER, mbgl::floorVertexShader()),
                      compileShader(GL_FRAGMENT_SHADER, mbgl::floorFragmentShader()));
    }
    
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
        glBufferData(GL_ARRAY_BUFFER, 36 * sizeof(GLfloat), vertices, GL_STATIC_DRAW);
        
        static programs::AttribLocation a0(program, "a_pos");
        glEnableVertexAttribArray(a0);
        glVertexAttribPointer(a0, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void*>(0));
        
        static programs::AttribLocation a1(program, "a_normal_ed");
        glEnableVertexAttribArray(a1);
        glVertexAttribPointer(a1, 4, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void*>(2 * sizeof(GLfloat)));
        
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    
    return vao;
}

void render(const mbgl::mat4& mvp, const mbgl::mat4& mv, const mbgl::mat4& normal, const mbgl::mat4& lightmvp) {
    GLboolean cullfaceEnabled;
    glGetBooleanv(GL_CULL_FACE, &cullfaceEnabled);
    
    const GLint program = ground::program();
    glUseProgram(program);
    glDisable(GL_CULL_FACE); // for render ground
    
    {
        static programs::UniformLocation u0(program, "u_matrix");
        const Mat4 MVP = convertMatrix4(mvp);
        glUniformMatrix4fv(u0, 1, GL_FALSE, reinterpret_cast<const float*>(&MVP));
        
        static programs::UniformLocation u1(program, "u_model_view_matrix");
        const Mat4 MV = convertMatrix4(mv);
        glUniformMatrix4fv(u1, 1, GL_FALSE, reinterpret_cast<const float*>(&MV));
        
        static programs::UniformLocation u2(program, "u_normal_matrix");
        const Mat4 NORMAL = convertMatrix4(normal);
        glUniformMatrix4fv(u2, 1, GL_FALSE, reinterpret_cast<const float*>(&NORMAL));
        
        static programs::UniformLocation u3(program, "u_light_matrix");
        const Mat4 LIGHTMVP = convertMatrix4(lightmvp);
        glUniformMatrix4fv(u3, 1, GL_FALSE, reinterpret_cast<const float*>(&LIGHTMVP));
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, depthBuffer);
        static programs::UniformLocation u4(program, "u_shadow_map");
        glUniform1i(u4, 0);
    }
    
    glBindVertexArray(ground::vao(program));
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
    
    cullfaceEnabled ? glEnable(GL_CULL_FACE) : glDisable(GL_CULL_FACE);
}

} // ground


namespace deferred {

const float BUFFER_RATIO = 1.;

int width() {
    return nav::display::pixels::width() * BUFFER_RATIO;
}

int height() {
    return nav::display::pixels::height() * BUFFER_RATIO;
}

void render(float zoom, mbgl::mat4 projMatrix,
            std::function<bool()> shadowRenderDelegate,
            std::function<bool()> geoRenderDelegate) {
    
    if (zoom < 15.) return;
    
    GLfloat clearColor[4];
    glGetFloatv(GL_COLOR_CLEAR_VALUE, clearColor);
    
    GLboolean blendEnabled;
    glGetBooleanv(GL_BLEND, &blendEnabled);
    
    {
        GLint viewport[4];
        glGetIntegerv(GL_VIEWPORT, viewport);
        auto bindScreen = [viewport] () {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
            
        };
        
        const int w = width();
        const int h = height();
        glViewport(0, 0, w, h);
        
        // 1
        depthBuffer = nav::shadow::depth::render(w, h, shadowRenderDelegate);
        
        // 2
        nav::geo::renderGeoAndShadow(w, h, depthBuffer, geoRenderDelegate);
        
        // 3
        const GLint shadowAndAO = nav::ssao::render(w, h, zoom, convertMatrix4(projMatrix));
        
        // 4
        nav::quad::renderBlur(w, h, shadowAndAO, bindScreen);
        
        // debug info window
        if (1) {
            int x = 20, y = 20;
            int ww = w / 6., hh = h / 6.;
            
            nav::shadow::frustum::ortho::sunlight().render([x, y, ww, hh] () {
                glBindFramebuffer(GL_FRAMEBUFFER, 0);
                glViewport(x, y, ww, hh);
            });
            
            nav::quad::render(w, h, depthBuffer, [x, y, ww, hh] () {
                glBindFramebuffer(GL_FRAMEBUFFER, 0);
                glViewport(x, y, ww, hh);
            });
            
            x += ww + 20;
            
            nav::quad::render(w, h, shadowAndAO, [x, y, ww, hh] () {
                glBindFramebuffer(GL_FRAMEBUFFER, 0);
                glViewport(x, y, ww, hh);
            });
            
            glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
        }
        
    }
    
    glClearColor(clearColor[0], clearColor[1], clearColor[2], clearColor[3]);
    blendEnabled ? glEnable(GL_BLEND) : glDisable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA); // mapbox config
    
}

} // deffered

} // renderer

} // nav
