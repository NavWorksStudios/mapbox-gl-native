//
//  nav.quad.hpp
//  Mapbox GL Native
//
//  Created by BernieZhao on 2024/10/10.
//

#pragma once

#include <gl_headers.hpp>
#include <functional>

namespace nav {
namespace quad {
void render(GLint program);
void render(int width, int height, GLint buffer, std::function<void()> bindScreen);
void renderBlur(int width, int height, GLint buffer, std::function<void()> bindScreen);
}
}

