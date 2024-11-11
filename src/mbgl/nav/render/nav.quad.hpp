//
//  nav.quad.hpp
//  Mapbox GL Native
//
//  Created by BernieZhao on 2024/10/10.
//

#pragma once

#include <gl_headers.hpp>

namespace nav {
namespace render {
namespace quad {

void render(GLint program);
void renderStandard(GLint buffer, float opacity=1.);
void renderRedChannel(GLint buffer, float opacity=1.);
void renderBlur(GLint buffer, uint32_t width, uint32_t height);

}
}
}

