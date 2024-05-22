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
    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    std::time_t now_time_t = std::chrono::system_clock::to_time_t(now);
    std::tm* now_tm = std::localtime(&now_time_t);
    
    char buffer[128];
    strftime(buffer, sizeof(buffer), "%F %T", now_tm);
    std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
    
    static char timeStr[64];
    sprintf(timeStr, "[%s:%lld]", buffer, ms.count());
    const size_t len = strlen(timeStr);
    memset(timeStr + len, ' ', 26 - len);
    timeStr[26] = 0;
    
    printf("%s", timeStr);

    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
}

std::map<std::string, int>& bucketMap() {
    static std::map<std::string, int> map;
    return map;
}

}
}
