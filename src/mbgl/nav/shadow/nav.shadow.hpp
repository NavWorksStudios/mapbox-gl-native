//
//  nav.ssao.hpp
//
//  Created by BernieZhao on 2024/9/1.
//

#pragma once

// intro
// https://learnopengl.com/Advanced-Lighting/SSAO

// source code
// https://learnopengl.com/code_viewer_gh.php?code=src/5.advanced_lighting/9.ssao/ssao.cpp

#include <functional>
#include <gl_headers.hpp>
#include <mbgl/util/mat4.hpp>

namespace nav {

namespace shadow {

namespace sample {

//    void draw(float zoom, mbgl::mat4 projMatrix, std::function<void()> renderCallback);
    void draw(std::function<void()> renderCallback);

//    void enableShadowEnv();

}   // sample

}   // shadow

}   // nav

