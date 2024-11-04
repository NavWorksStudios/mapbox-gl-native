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

namespace shadow {

const int width = 4096 * 1.;
const int height = 4096 * 1.;

GLuint render(int width, int height,
              std::function<void()> renderDelegate,
              std::function<void()> bindScreen = nullptr);

void setDepthBuffer(GLuint buffer);

void renderGround(const mbgl::mat4& mvp, const mbgl::mat4& mv, const mbgl::mat4& normal, const mbgl::mat4& lightmvp);

}   // shadow

}   // nav

