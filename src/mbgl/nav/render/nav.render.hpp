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

namespace renderer {

namespace ground {
void render(const mbgl::mat4& mvp, const mbgl::mat4& mv, const mbgl::mat4& normal, const mbgl::mat4& lightmvp);
}

namespace deferred {
int width();
int height();

void render(float zoom, mbgl::mat4 projMatrix,
            std::function<bool()> shadowRenderDelegate,
            std::function<bool()> geoRenderDelegate);
}

}

}

