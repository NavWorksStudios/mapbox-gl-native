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
void renderStandard(GLint buffer);
void renderMono(GLint buffer);
void renderBlur(GLint buffer, uint32_t width, uint32_t height);

}
}
}

