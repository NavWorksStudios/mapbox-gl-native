//
//  nav_mb_layer.hpp
//
//  Created by BernieZhao on 2023/11/21.
//

#pragma once

#include <string>

namespace nav {

namespace mb {

const std::string& accessToken();
void printLayerTable(const char* id, const char* sourceLayer, const char* sourceId, const char* type);
bool filterLayerByType(const std::string& id);
int layerRenderIndex(const std::string& id);

}

}
