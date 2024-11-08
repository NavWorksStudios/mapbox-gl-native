//
//  nav.geo.hpp
//
//  Created by BernieZhao on 2024/10/31.
//

#pragma once

#include <functional>
#include <gl_headers.hpp>

namespace nav {
namespace render {
namespace geo {

void setCurrentProgram();

using GBuffer = std::array<GLuint, 3>;
GBuffer renderGeoAndShadow(uint32_t width, uint32_t height,
                           GLuint shadow, GLuint shadowDepth,
                           std::function<void()> renderDelegate);

}
}
}
