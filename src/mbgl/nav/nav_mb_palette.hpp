//
//  nav_mb_palette.hpp
//
//  Created by BernieZhao on 2024/7/9.
//

#pragma once

#include <mbgl/util/color.hpp>

namespace nav {

namespace palette {

using Binding = std::function<void(const mbgl::Color& color)>;

void setColorBase(const mbgl::Color& color);
const mbgl::Color& getColorBase();

bool enableShaderPalette(const std::string& id);

void bind(const std::string& uri, const mbgl::Color& color, const Binding& binding);

}

}
