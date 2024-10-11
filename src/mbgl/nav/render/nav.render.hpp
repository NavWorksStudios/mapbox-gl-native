//
//  nav.render.hpp
//  Mapbox GL Native
//
//  Created by BernieZhao on 2024/10/10.
//

#pragma once

#include <functional>
#include <gl_headers.hpp>
#include <mbgl/util/mat4.hpp>

namespace nav {
namespace render {

void deferred(float zoom,
              mbgl::mat4 projMatrix,
              mbgl::mat4 lightMatrix,
              mbgl::vec3 lightPos,
              std::function<void()> shadowRenderDelegate,
              std::function<void()> geoRenderDelegate);

void renderTileFloor(const mbgl::mat4& mvp, const mbgl::mat4& mv, const mbgl::mat4& normal);

namespace util {
GLuint genTexture(GLint internalformat, GLsizei width, GLsizei height, GLenum format, GLenum type);
void renderQuad(GLint program);
}

}
}

