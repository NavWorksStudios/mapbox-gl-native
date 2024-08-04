//
//  nav.log.hpp
//  Mapbox GL Native
//
//  Created by BernieZhao on 2024/5/22.
//

#ifndef nav_log_hpp
#define nav_log_hpp

#include <stdio.h>
#include <string>
#include <map>
#include <mbgl/tile/tile_id.hpp>

namespace nav {
namespace log {

enum LogLevel : uint8_t {
    Verbose,
    Infomation,
    Debug,
    Warning,
    Error,
    None,
};

void v(const char* tag, const char* format, ...);
void i(const char* tag, const char* format, ...);
void d(const char* tag, const char* format, ...);
void w(const char* tag, const char* format, ...);
void e(const char* tag, const char* format, ...);

void sprintf(char* buf, const char* format, ...);

template <int VOLUME>
class Stream {
    char buf[VOLUME] = { 0 };
    char* p = { buf };

public:
    void operator()(const char* format, ...) {
        va_list args;
        va_start(args, format);
        sprintf(p, format, args);
        va_end(args);
        p += strlen(p);

        assert(p-buf < VOLUME);
    }
    
    void v(const char* tag) { log::v(tag, "%s", buf); }
    void i(const char* tag) { log::i(tag, "%s", buf); }
    void d(const char* tag) { log::d(tag, "%s", buf); }
    void w(const char* tag) { log::w(tag, "%s", buf); }
    void e(const char* tag) { log::e(tag, "%s", buf); }
};

}
}

#endif /* nav_log_hpp */
