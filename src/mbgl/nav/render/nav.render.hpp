//
//  nav.render.hpp
//  Mapbox GL Native
//
//  Created by BernieZhao on 2024/10/10.
//

#pragma once

#include <functional>
#include <mbgl/renderer/paint_parameters.hpp>

namespace nav {
namespace render {

void switchDebugWindow();

namespace procedure {
enum Value : int32_t {
    None = 0,
    Depth = 1 << 0,
    Geo = 1 << 1,
    AO = 1 << 2,
    Blur = 1 << 3,
};
Value value();
}

void renderDeferred(const mbgl::PaintParameters& parameters,
                    std::function<void()> renderShadowDepthDelegate,
                    std::function<void()> renderGeoDelegate);

}
}

