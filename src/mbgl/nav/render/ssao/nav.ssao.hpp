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

namespace geo {
GLint renderGeoAndShadowBuffer(int width, int height, GLint shadow,
                               std::function<bool()> renderCallback,
                               std::function<void()> bindScreen=nullptr);
}

namespace ssao {
GLint render(int width, int height, float zoom, const Mat4& projMatrix, std::function<void()> bindScreen=nullptr);
}

}
