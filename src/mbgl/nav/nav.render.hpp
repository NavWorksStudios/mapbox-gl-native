//
//  nav.render.hpp
//
//  Created by BernieZhao on 2024/9/1.
//

#pragma once

// https://zhuanlan.zhihu.com/p/547180466

#include "gl_headers.hpp"

namespace nav {

namespace render {

void createRenderbuffer();
GLuint gBuffer();
void prepareGBuffer();
void prepareAOBuffer();
void renderAO();

}

}
