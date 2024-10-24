//
//  nav.blur.hpp
//  Mapbox GL Native
//
//  Created by BernieZhao on 2024/10/10.
//

#pragma once

#include <gl_headers.hpp>
#include <functional>

namespace nav {
namespace blur {

void render(int width, int height, GLint buffer, bool enableBlur, std::function<void()> bindScreen);

}
}

