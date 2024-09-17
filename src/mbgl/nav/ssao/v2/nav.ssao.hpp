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
#include "mbgl/nav/ssao/v1/mat4.h"

namespace nav {
namespace ssao {
namespace v2 {
    void draw(std::function<void()> renderCallback, Mat4 projMatrix);
}
}
}
