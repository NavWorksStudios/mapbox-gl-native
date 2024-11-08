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
namespace render {
namespace ssao {

void render(uint32_t width, uint32_t height, GLuint renderBuffer,
            const std::array<GLuint, 3>& gbuffer,
            float zoom, const Mat4& projMatrix);

}
}
}
