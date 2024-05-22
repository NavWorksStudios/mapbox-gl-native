//
//  nav_mb_log.cpp
//  Mapbox GL Native
//
//  Created by BernieZhao on 2024/5/22.
//

#include "nav_mb_log.hpp"
#include <chrono>

namespace nav {
namespace mb {

std::string tileId(const mbgl::CanonicalTileID& canonical, const std::string& layerId, const std::string& sourceId) {
    char tileid[32];
    sprintf(tileid, "(%d,%d,%d)", canonical.x, canonical.y, (int)canonical.z);
    
    std::string tile;
    tile += tileid; while (tile.length() < 20) tile += " ";
    tile += layerId; while (tile.length() < 40) tile += " ";
    tile += sourceId; while (tile.length() < 60) tile += " ";
    return tile;
}

void log(const char* format, ...) {
    const std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    const std::time_t now_time_t = std::chrono::system_clock::to_time_t(now);
    const std::tm* now_tm = std::localtime(&now_time_t);
    const std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
    
    // time
    char buffer[512];
    buffer[0] = '[';
    strftime(buffer + 1, 100, "%F %T", now_tm);
    sprintf(buffer + 20, ":%lld]", ms.count());
    const size_t len = strlen(buffer);
    char* buf = buffer + len;
    memset(buf, ' ', 26 - len);
    
    // va
    va_list args;
    va_start(args, format);
    vsprintf(buffer + 26, format, args);
    va_end(args);
    
    // print
    printf("%s", buffer);
}

std::map<std::string, int>& bucketMap() {
    static std::map<std::string, int> map;
    return map;
}

}
}
