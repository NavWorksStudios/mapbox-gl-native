//
//  nav_log.cpp
//  Mapbox GL Native
//
//  Created by BernieZhao on 2024/5/22.
//

#include "nav_log.hpp"
#include <chrono>


char* print_timestamp(char* buf) {
    const std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    const std::time_t now_time_t = std::chrono::system_clock::to_time_t(now);
    const std::tm* now_tm = std::localtime(&now_time_t);
    const std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
    
    buf[0] = '[';
    strftime(buf + 1, 100, "%F %T", now_tm);
    sprintf(buf + 20, ":%lld]", ms.count());
    
    const size_t len = strlen(buf);

    if (len < 26) {
        memset(buf + len, ' ', 26 - len);
    }
    
    return buf + 26;
}

char* print_tag(char* buf, const char* tag) {
    *buf++ = ' ';
    *buf++ = '<';
    
    const size_t len = strlen(tag);
    memcpy(buf, tag, len);
    buf += len;
    
    *buf++ = '>';
    *buf++ = ' ';
    return buf;
}

namespace nav {
namespace log {

void v(const char* tag, const char* format, ...) {
    if (LEVEL <= LogLevel::Verbose) {
        char buf[512];
        char* p = buf;
        
        p = print_timestamp(p);
        *p++ = 'V';
        p = print_tag(p, tag);
        
        va_list args;
        va_start(args, format);
        vsprintf(p, format, args);
        va_end(args);

        printf("%s", buf);
    }
}

void i(const char* tag, const char* format, ...) {
    if (LEVEL <= LogLevel::Infomation) {
        char buf[512];
        char* p = buf;
        
        p = print_timestamp(p);
        *p++ = 'I';
        p = print_tag(p, tag);
        
        va_list args;
        va_start(args, format);
        vsprintf(p, format, args);
        va_end(args);

        printf("%s", buf);
    }
}

void d(const char* tag, const char* format, ...) {
    if (LEVEL <= LogLevel::Debug) {
        char buf[512];
        char* p = buf;
        
        p = print_timestamp(p);
        *p++ = 'D';
        p = print_tag(p, tag);
        
        va_list args;
        va_start(args, format);
        vsprintf(p, format, args);
        va_end(args);

        printf("%s", buf);
    }
}

void w(const char* tag, const char* format, ...) {
    if (LEVEL <= LogLevel::Warning) {
        char buf[512];
        char* p = buf;
        
        p = print_timestamp(p);
        *p++ = 'W';
        p = print_tag(p, tag);
        
        va_list args;
        va_start(args, format);
        vsprintf(p, format, args);
        va_end(args);

        printf("%s", buf);
    }
}

void e(const char* tag, const char* format, ...) {
    if (LEVEL <= LogLevel::Error) {
        char buf[512];
        char* p = buf;
        
        p = print_timestamp(p);
        *p++ = 'E';
        p = print_tag(p, tag);
        
        va_list args;
        va_start(args, format);
        vsprintf(p, format, args);
        va_end(args);

        printf("%s", buf);
    }
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
