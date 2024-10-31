//
//  nav.ssao.hpp
//
//  Created by BernieZhao on 2024/9/1.
//

#pragma once

#include <functional>
#include <gl_headers.hpp>
#include "mbgl/nav/render/mat4.h"

namespace nav {

namespace ssao {

void render(int width, int height,
            GLuint renderBuffer, const std::array<GLuint, 3>& gbuffer,
            float zoom, const Mat4& projMatrix, std::function<void()> bindScreen=nullptr);

}

}
