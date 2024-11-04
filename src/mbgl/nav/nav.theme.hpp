//
//  nav.theme.hpp
//
//  Created by BernieZhao on 2024/8/5.
//

#pragma once

#include "mbgl/nav/nav.hsla.hpp"
#include "mbgl/nav/nav.stringid.hpp"
#include <string>

namespace nav {

namespace theme {

const std::string& style();
bool needsAutoUpdate();
bool isBuildingReflectionEnabled();
std::tuple<Hsla,bool> getColorProperty(const std::string& uri, Hsla color);
bool isLayerPaletteEnabled(const nav::stringid& layer);
int getShaderIndex();

}

}
