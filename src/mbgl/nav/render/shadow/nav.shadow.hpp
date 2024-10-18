//
//  nav.shadow.hpp
//
//  Created by ZHM on 2024/10/11.
//

#pragma once

#include <functional>
#include <gl_headers.hpp>
#include "mbgl/nav/render/mat4.h"

namespace nav {

namespace shadow {

namespace depth {

const int width = 4096;
const int height = 4096;

GLuint render(int width, int height,
              std::function<bool()> renderCallback,
              std::function<void()> bindScreenFbo = nullptr);

}

}   // shadow

}   // nav

