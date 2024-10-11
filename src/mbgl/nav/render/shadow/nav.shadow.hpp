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

    GLuint renderShadowDepthBuffer(int width, int height,
                                   std::function<bool()> renderCallback);   // draw / drawShadowDepthBuffer

}   // shadow

}   // nav

