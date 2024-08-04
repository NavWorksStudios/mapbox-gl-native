//
//  nav.layer.hpp
//
//  Created by BernieZhao on 2023/11/21.
//

#pragma once

#include <string>

namespace nav {
namespace layer {

void print(const char* id, const char* sourceLayer, const char* sourceId, const char* type);

bool filter(const std::string& id);

float getHeight(const std::string& layerId);

}
}
