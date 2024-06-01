//
//  nav_log.hpp
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

enum LogLevel : uint8_t { Verbose, Infomation, Debug, Warning, Error };
const LogLevel LEVEL = LogLevel::Debug;

void v(const char* tag, const char* format, ...);
void i(const char* tag, const char* format, ...);
void d(const char* tag, const char* format, ...);
void i(const char* tag, const char* format, ...);
void w(const char* tag, const char* format, ...);
void e(const char* tag, const char* format, ...);

std::string tileId(const mbgl::CanonicalTileID& canonical, const std::string& layerId, const std::string& sourceId);
std::map<std::string, int>& bucketMap();

}
}

#endif /* nav_log_hpp */
