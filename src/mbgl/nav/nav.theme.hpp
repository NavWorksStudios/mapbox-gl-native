//
//  nav.theme.hpp
//
//  Created by BernieZhao on 2024/8/5.
//

#pragma once

#include "mbgl/nav/nav.stringid.hpp"
#include <string>

namespace nav {

namespace theme {

const std::string& style();
bool needsUpdate();
bool isStylibleColor(const std::string& uri);
bool enableLayerMonoPalette(const nav::stringid& layer);

}

}
