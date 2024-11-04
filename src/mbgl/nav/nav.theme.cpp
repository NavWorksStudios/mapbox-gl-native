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

enum Update { Continuous, AsNeed };
enum BuildReflection { Enabled, Disabled };

struct Config {
    std::string name;
    std::string mapboxStudioUrl;
    std::string styleUrl;
    
    Update needsUpdate;
    BuildReflection enableBuildReflection;
    
    std::function<std::tuple<Hsla,bool>(const std::string&, Hsla)> layerColorConfig;
    std::function<bool(const nav::stringid&)> layerMonoPaletteConfig;

    int shaderIndex;
};

const Config COLORFUL = {
    "Crystal 水晶",
    "https://studio.mapbox.com/styles/navworks/clxx105i700yr01po4zbn2jc1/edit/#20.32/39.8591796/116.3684606/8/70",
    "mapbox://styles/navworks/clxx105i700yr01po4zbn2jc1",
    
    Update::Continuous,
    BuildReflection::Enabled,
    
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

const Config PURENESS = {
    "Pureness 纯白",
    "https://studio.mapbox.com/styles/navworks/clzqn4giv00a801pi06quhgz7/edit/#7.03/31.635/120.897",
    "mapbox://styles/navworks/clzqn4giv00a801pi06quhgz7",
    
    Update::AsNeed,
    BuildReflection::Disabled,
    
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
    
    Update::AsNeed,
    BuildReflection::Disabled,
    
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
const Config& THEME = PURENESS;
//const Config& THEME = ROUTE_TEST;

const std::string& style() {
    return THEME.styleUrl;
}

bool needsAutoUpdate() {
    return THEME.needsUpdate == Update::Continuous;
}

bool isBuildingReflectionEnabled() {
    return THEME.enableBuildReflection == BuildReflection::Enabled;
}

std::tuple<Hsla,bool> getColorProperty(const std::string& uri, Hsla color) {
    return THEME.layerColorConfig(uri, color);
}

bool isLayerPaletteEnabled(const nav::stringid& layer) {
    return THEME.layerMonoPaletteConfig(layer);
}

int getShaderIndex() {
    return THEME.shaderIndex;
}

}

}
