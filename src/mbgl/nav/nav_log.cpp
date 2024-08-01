//
//  nav_log.cpp
//  Mapbox GL Native
//
//  Created by BernieZhao on 2024/5/22.
//

#include "nav_log.hpp"
#include <chrono>


template <size_t T=51200> struct PrintBuf {
    char* buf = nullptr;
    PrintBuf() { buf = (char*) malloc(T); }
    ~PrintBuf() { free(buf); }
    inline operator char* () const { return buf; }
    inline void print() const { printf("%s\n", buf); }
};

char* print_date_time(char* buf) {
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

char* print_time(char* buf) {
    const std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    const std::time_t now_time_t = std::chrono::system_clock::to_time_t(now);
    const std::tm* now_tm = std::localtime(&now_time_t);
    const std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
    
    buf[0] = '[';
    strftime(buf + 1, 100, "%T", now_tm);
    sprintf(buf + 9, ":%lld]", ms.count());
    
    const size_t len = strlen(buf);

    if (len < 15) {
        memset(buf + len, ' ', 15 - len);
    }
    
    return buf + 15;
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

const LogLevel LEVEL = LogLevel::Verbose;

void v(const char* tag, const char* format, ...) {
    if (LEVEL <= LogLevel::Verbose) {
        PrintBuf<> buf;
        char* p = buf;
        
        p = print_date_time(p);
        *p++ = 'V';
        p = print_tag(p, tag);
        
        va_list args;
        va_start(args, format);
        vsprintf(p, format, args);
        va_end(args);

        buf.print();
    }
}

void i(const char* tag, const char* format, ...) {
    if (LEVEL <= LogLevel::Infomation) {
        PrintBuf<> buf;
        char* p = buf;
        
        p = print_date_time(p);
        *p++ = 'I';
        p = print_tag(p, tag);
        
        va_list args;
        va_start(args, format);
        vsprintf(p, format, args);
        va_end(args);

        buf.print();
    }
}

void d(const char* tag, const char* format, ...) {
    if (LEVEL <= LogLevel::Debug) {
        PrintBuf<> buf;
        char* p = buf;
        
        p = print_date_time(p);
        *p++ = 'D';
        p = print_tag(p, tag);
        
        va_list args;
        va_start(args, format);
        vsprintf(p, format, args);
        va_end(args);

        buf.print();
    }
}

void w(const char* tag, const char* format, ...) {
    if (LEVEL <= LogLevel::Warning) {
        PrintBuf<> buf;
        char* p = buf;
        
        p = print_date_time(p);
        *p++ = 'W';
        p = print_tag(p, tag);
        
        va_list args;
        va_start(args, format);
        vsprintf(p, format, args);
        va_end(args);

        buf.print();
    }
}

void e(const char* tag, const char* format, ...) {
    if (LEVEL <= LogLevel::Error) {
        PrintBuf<> buf;
        char* p = buf;
        
        p = print_date_time(p);
        *p++ = 'E';
        p = print_tag(p, tag);
        
        va_list args;
        va_start(args, format);
        vsprintf(p, format, args);
        va_end(args);

        buf.print();
    }
}

void sprintf(char* buf, const char* format, ...) {
    buf = print_time(buf);
    
    va_list args;
    va_start(args, format);
    vsprintf(buf, format, args);
    va_end(args);
}

}
}
