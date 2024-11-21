//
//  nav.shadow.hpp
//
//  Created by ZHM on 2024/10/11.
//

#pragma once

#include <functional>
#include <gl_headers.hpp>
#include <mbgl/util/mat4.hpp>

namespace nav {
namespace render {
namespace skybox {

void render(const mbgl::mat4& vp);

}   // skybox
}   // render
}   // nav

