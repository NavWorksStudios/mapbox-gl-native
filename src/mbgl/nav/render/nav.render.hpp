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

void renderDeferred(const mbgl::PaintParameters& parameters,
                    std::function<void()> renderShadowDelegate,
                    std::function<void()> renderHaloDelegate,
                    std::function<void()> renderGeoDelegate);

void renderLogo(const mbgl::PaintParameters& parameters);

}
}

