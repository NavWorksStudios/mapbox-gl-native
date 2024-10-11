//
//  nav.ssao.hpp
//
//  Created by BernieZhao on 2024/9/1.
//

#pragma once

// intro
// https://learnopengl.com/Advanced-Lighting/SSAO

// source code
// https://learnopengl.com/code_viewer_gh.php?code=src/5.advanced_lighting/9.ssao/ssao.cpp

#include <functional>
#include <gl_headers.hpp>
#include "mbgl/nav/render/mat4.h"

namespace nav {

namespace shadow {

    GLuint renderShadowDepthBuffer(int width, int height,
                                   const Mat4& lightMatrix,
                                   std::function<void()> renderCallback);   // draw / drawShadowDepthBuffer

}   // shadow

}   // nav

