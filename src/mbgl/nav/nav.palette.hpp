//
//  nav.palette.hpp
//
//  Created by BernieZhao on 2024/7/9.
//

#pragma once

#include <mbgl/util/color.hpp>

namespace nav {

namespace palette {

void setColorBase(const mbgl::Color& color);
const mbgl::Color& getColorBase();

bool enableLayerMonoPalette(const std::string& layer);

using Binding = std::function<void(const mbgl::Color& color)>;
void bind(const std::string& uri, const mbgl::Color& color, const void* binder, const Binding& binding);

void unbind(const void* binder);

}

}
