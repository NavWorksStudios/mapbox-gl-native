//
//  nav.geo.hpp
//
//  Created by BernieZhao on 2024/10/31.
//

#pragma once

#include <functional>
#include <gl_headers.hpp>

namespace nav {
namespace geo {

using GBuffer = std::array<GLuint, 3>;
GBuffer renderGeoAndShadow(int width, int height, GLuint shadow, GLuint shadowDepth,
                           std::function<bool()> renderCallback,
                           std::function<void()> bindScreen=nullptr);

}
}
