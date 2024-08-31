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
bool needsUpdate();
bool enableBuildingReflection();
std::tuple<Hsla,bool> colorProperty(const std::string& uri, Hsla color);
bool enableLayerMonoPalette(const nav::stringid& layer);
int shaderIndex();

}

}
