//
//  nav.render.hpp
//  Mapbox GL Native
//
//  Created by BernieZhao on 2024/10/10.
//

#pragma once

#include <functional>
#include <mbgl/util/mat4.hpp>

namespace nav {

namespace renderer {

void debugPrint();

enum RenderProcedure : int32_t {
    None = 0,
    Depth = 1 << 0,
    GBuffer = 1 << 1,
};

RenderProcedure procedure();

void render(float zoom, mbgl::mat4 projMatrix,
            std::function<void()> renderShadowDepthDelegate,
            std::function<void()> renderGeoDelegate);

}

}

