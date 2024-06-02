//
//  nav_mb_layer.hpp
//
//  Created by BernieZhao on 2023/11/21.
//

#pragma once

#include <string>

namespace nav {

namespace mb {

//const std::string ID_NAV_LAND = "nav:land";
//const std::string ID_NAV_3D_LAND = "nav:3d-land";

bool filterLayerByType(const std::string& id);

int layerRenderIndex(const std::string& id);

}

}
