//
//  nav.theme.cpp
//
//  Created by BernieZhao on 2024/8/5.
//

#include "mbgl/nav/nav.theme.hpp"
#include <functional>
#include <unordered_map>


namespace nav {

namespace theme {

static const auto normalize = [] (float value) {
    return fmin(fmax(value, 0.), 1.);
};

struct Config {
    std::string name;
    std::string mapbox_studio_url;
    std::string style_url;
    bool needsUpdate;
    bool enableBuildReflection;
    std::function<std::tuple<Hsla,bool>(const std::string&, Hsla)> getConfig;
    std::function<bool(const nav::stringid&)> enableLayerMonoPalette;
    int shaderIndex;
};

const Config COLORFUL = {
    "流光",
    "https://studio.mapbox.com/styles/navworks/clxx105i700yr01po4zbn2jc1/edit/#20.32/39.8591796/116.3684606/8/70",
    "mapbox://styles/navworks/clxx105i700yr01po4zbn2jc1",
    true,
    true,
    [] (const std::string& uri, Hsla color) -> std::tuple<Hsla,bool> {
        if (uri.find("water-depth") != std::string::npos) {
            color.s = 0;
        } else if(uri.find("hillshade") != std::string::npos) {
            color.s = 0;
        } else if (uri.find("building-extrusion") != std::string::npos) {
//            color.s = .3;
//            color.l = .6;
        } else if (uri.find("case") == std::string::npos) {
            if (uri.find("path") != std::string::npos ||
                uri.find("steps") != std::string::npos ||
                uri.find("pedestrian") != std::string::npos) {

            } else if (uri.find("tunnel") != std::string::npos) {
                color.l = normalize(color.l * 1.2);
            } else if (uri.find("road") != std::string::npos) {
                color.l = normalize(color.l * 1.4);
            } else if (uri.find("bridge") != std::string::npos) {
                color.l = normalize(color.l * 1.6);
            }
        }

        bool stylible = true;
        if (uri.find("water-depth") != std::string::npos ||
            uri.find("hillshade") != std::string::npos) {
            stylible = false;
        }
        
        return std::make_tuple(color, stylible);
    },
    [] (const nav::stringid& layer) {
        static std::unordered_map<std::string, bool> layerIds = {
            { "water-depth", true },
            { "hillshade", true },
        };

        return layerIds.find(layer) != layerIds.end();
    },
    1,
};

const Config GOLDEN_BLACK = {
    "星汉",
    "https://studio.mapbox.com/styles/navworks/clzdv9emu00f301r27uym15w9/edit/#14.4/22.2936/114.15234/0/1",
    "mapbox://styles/navworks/clzdv9emu00f301r27uym15w9",
    false,
    false,
    [] (const std::string& uri, Hsla color) -> std::tuple<Hsla,bool> {
        if (uri.find("building-extrusion") != std::string::npos) {
        }
        
        return std::make_tuple(color, false);
    },
    [] (const nav::stringid&) {
        return false;
    },
    2,
};

const Config MANHATTAN = {
    "曼哈顿",
    "https://studio.mapbox.com/styles/navworks/clzqn4giv00a801pi06quhgz7/edit/#7.03/31.635/120.897",
    "mapbox://styles/navworks/clzqn4giv00a801pi06quhgz7",
    false,
    false,
    [] (const std::string& uri, Hsla color) -> std::tuple<Hsla,bool> {
        if (uri.find("building-extrusion") != std::string::npos) {
        }
        
        return std::make_tuple(color, false);
    },
    [] (const nav::stringid&) {
        return false;
    },
    2,
};

const Config ROUTE_TEST = {
    "导航测试",
    "https://studio.mapbox.com/styles/navworks/clzqn4giv00a801pi06quhgz7/edit/#7.03/31.635/120.897",
    "mapbox://styles/navworks/clyxyqksj00ap01qnc17kbs8x",
    false,
    false,
    [] (const std::string& uri, Hsla color) -> std::tuple<Hsla,bool> {
        if (uri.find("building-extrusion") != std::string::npos) {
        }
        
        return std::make_tuple(color, false);
    },
    [] (const nav::stringid&) {
        return false;
    },
    1,
};

//const Config& THEME = COLORFUL;
//const Config& THEME = GOLDEN_BLACK;
//const Config& THEME = MANHATTAN;
const Config& THEME = ROUTE_TEST;

const std::string& style() {
    return THEME.style_url;
}

bool needsUpdate() {
    return THEME.needsUpdate;
}

bool enableBuildingReflection() {
    return THEME.enableBuildReflection;
}

std::tuple<Hsla,bool> colorProperty(const std::string& uri, Hsla color) {
    return THEME.getConfig(uri, color);
}

bool enableLayerMonoPalette(const nav::stringid& layer) {
    return THEME.enableLayerMonoPalette(layer);
}

int shaderIndex() {
    return THEME.shaderIndex;
}

}

}
