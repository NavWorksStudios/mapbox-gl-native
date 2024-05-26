//
//  nav_log.cpp
//  Mapbox GL Native
//
//  Created by BernieZhao on 2024/5/22.
//

#include "nav_log.hpp"
#include <chrono>

namespace nav {
namespace log {

void i(const char* tag, const char* format, ...) {
    char buffer[512];
    size_t offset = 0;
    
    // time
    {
        const std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
        const std::time_t now_time_t = std::chrono::system_clock::to_time_t(now);
        const std::tm* now_tm = std::localtime(&now_time_t);
        const std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
        
        buffer[0] = '[';
        strftime(buffer + 1, 100, "%F %T", now_tm);
        sprintf(buffer + 20, ":%lld]", ms.count());
        
        const size_t len = strlen(buffer);
        offset += len;
        
        if (offset < 25) {
            memset(buffer + offset, ' ', 25 - offset);
            offset = 25;
        }
    }
    
    // tag
    {
        buffer[offset++] = ' ';
        buffer[offset++] = '<';
        
        const size_t len = strlen(tag);
        memcpy(buffer + offset, tag, len);
        offset += len;
        
        buffer[offset++] = '>';
        buffer[offset++] = ' ';
    }
    
    // va
    {
        va_list args;
        va_start(args, format);
        vsprintf(buffer + offset, format, args);
        va_end(args);
    }

    
    // print
    printf("%s", buffer);
}

std::string tileId(const mbgl::CanonicalTileID& canonical, const std::string& layerId, const std::string& sourceId) {
    char tileid[32];
    sprintf(tileid, "(%d,%d,%d)", canonical.x, canonical.y, (int)canonical.z);
    
    std::string tile;
    tile += tileid; while (tile.length() < 20) tile += " ";
    tile += layerId; while (tile.length() < 40) tile += " ";
    tile += sourceId; while (tile.length() < 60) tile += " ";
    return tile;
}

std::map<std::string, int>& bucketMap() {
    static std::map<std::string, int> map;
    return map;
}

}
}
