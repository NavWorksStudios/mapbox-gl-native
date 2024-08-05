//
//  nav.theme.cpp
//
//  Created by BernieZhao on 2024/8/5.
//

#include "mbgl/nav/nav.theme.hpp"
#include <functional>


namespace nav {

namespace theme {

struct Config {
    std::string name;
    std::string url;
    std::string style;
    bool needsUpdate;
    std::function<bool(const std::string&)> isStylibleColor;
};

const Config COLORFUL = {
    "色彩",
    "https://studio.mapbox.com/styles/navworks/clxx105i700yr01po4zbn2jc1/edit/#20.32/39.8591796/116.3684606/8/70",
    "mapbox://styles/navworks/clxx105i700yr01po4zbn2jc1",
    true,
    [] (const std::string& uri) {
        if (uri.find("water-depth") != std::string::npos ||
            uri.find("hillshade") != std::string::npos) {
            return false;
        } else {
            return true;
        }
    },
};

const Config GOLDEN_BLACK = {
    "星汉",
    "https://studio.mapbox.com/styles/navworks/clzdv9emu00f301r27uym15w9/edit/#14.4/22.2936/114.15234/0/1",
    "mapbox://styles/navworks/clzdv9emu00f301r27uym15w9",
    false,
    [] (const std::string&) {
        return false;
    },
};

const Config& THEME = COLORFUL;
//const Config& THEME = GOLDEN_BLACK;

const std::string& style() {
    return THEME.style;
}

bool needsUpdate() {
    return THEME.needsUpdate;
}

bool isStylibleColor(const std::string& uri) {
    return THEME.isStylibleColor(uri);
}

}

}
