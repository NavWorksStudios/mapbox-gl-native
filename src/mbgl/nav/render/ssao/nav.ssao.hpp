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

void initResource(int width, int height);
void renderGeoAndShadowBuffer(GLint shadowDepth, std::function<void()> renderCallback, std::function<void()> bindScreen=nullptr);
GLint renderAOBuffer(int width, int height, float zoom, const Mat4& projMatrix, const Mat4& lightMatrix, std::function<void()> bindScreen=nullptr);

}
}
