//
//  nav.emissive.hpp
//
//  Created by BernieZhao on 2024/11/9.
//

#pragma once

#include <functional>
#include <gl_headers.hpp>

namespace nav {
namespace render {
namespace emissive {

GLuint render(uint32_t width, uint32_t height, std::function<void()> renderDelegate);

}   // halo
}   // render
}   // nav

