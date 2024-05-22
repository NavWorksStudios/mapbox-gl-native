//
//  nav_mb_log.hpp
//  Mapbox GL Native
//
//  Created by BernieZhao on 2024/5/22.
//

#ifndef nav_mb_log_hpp
#define nav_mb_log_hpp

#include <stdio.h>
#include <string>
#include <mbgl/tile/tile_id.hpp>

namespace nav {
namespace mb {

std::string tileId(const mbgl::CanonicalTileID& canonical, const std::string& layerId, const std::string& sourceId);
void log(const char* format, ...);


}
}

#endif /* nav_mb_log_hpp */
