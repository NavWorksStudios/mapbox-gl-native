//
//  nav.render.hpp
//
//  Created by BernieZhao on 2024/9/1.
//

#pragma once

// https://zhuanlan.zhihu.com/p/547180466

#include "TargetConditionals.h"

#if TARGET_OS_IPHONE
    #include <OpenGLES/ES2/gl.h>
    #include <OpenGLES/ES2/glext.h>
#elif TARGET_OS_SIMULATOR
    #include <OpenGLES/ES2/gl.h>
    #include <OpenGLES/ES2/glext.h>
#elif TARGET_OS_MAC
    #include <OpenGL/OpenGL.h>
    #include <OpenGL/gl.h>
    #include <OpenGL/glext.h>
#else
    #error Unsupported Apple platform
#endif


namespace nav {

namespace render {

void createRenderbuffer();
GLuint gBuffer();
void prepareGBuffer();
void prepareAOBuffer();
void renderAO();

}

}
