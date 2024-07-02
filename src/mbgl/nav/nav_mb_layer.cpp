//
//  nav_mb_layer_filter.hpp
//
//  Created by BernieZhao on 2023/11/21.
//

#include "mbgl/nav/nav_mb_layer.hpp"
#include "mbgl/nav/nav_log.hpp"

#include <map>
#include <mutex>

namespace nav {
namespace layer {

void print(const char* id, const char* type, const char* source, const char* sourceLayer) {
    nav::log::printf("{ \"%s\", { \"%s\", \"%s\", \"%s\", } }, ", id, type, source, sourceLayer);
}

struct Source {
    const char* type;
    const char* source;
    const char* sourceLayer;
};

static const std::map<std::string, Source> layerIds = {
    { "land", { "background", "", "", } },
    { "national-park", { "fill", "composite", "landuse_overlay", } },
    { "landuse", { "fill", "composite", "landuse", } },
    { "hillshade", { "fill", "composite", "hillshade", } },
    
    { "waterway", { "line", "composite", "waterway", } },
    
    { "water", { "fill", "composite", "water", } },
    
    { "land-structure-polygon", { "fill", "composite", "structure", } },
    { "land-structure-line", { "line", "composite", "structure", } },
    { "aeroway-polygon", { "fill", "composite", "aeroway", } },
    
    { "aeroway-line", { "line", "composite", "aeroway", } },
    
    { "tunnel-path-trail", { "line", "composite", "road", } },
    { "tunnel-path-cycleway-piste", { "line", "composite", "road", } },
    { "tunnel-path", { "line", "composite", "road", } },
    { "tunnel-steps", { "line", "composite", "road", } },
    { "tunnel-pedestrian", { "line", "composite", "road", } },
    { "tunnel-simple", { "line", "composite", "road", } },
    
    { "road-path-trail", { "line", "composite", "road", } },
    { "road-path-cycleway-piste", { "line", "composite", "road", } },
    { "road-path", { "line", "composite", "road", } },
    { "road-steps", { "line", "composite", "road", } },
    { "road-pedestrian", { "line", "composite", "road", } },
    { "road-simple", { "line", "composite", "road", } },
    { "road-rail", { "line", "composite", "road", } },
    { "road-rail-tracks", { "line", "composite", "road", } },
    
    { "bridge-path-trail", { "line", "composite", "road", } },
    { "bridge-path-cycleway-piste", { "line", "composite", "road", } },
    { "bridge-path", { "line", "composite", "road", } },
    { "bridge-steps", { "line", "composite", "road", } },
    { "bridge-pedestrian", { "line", "composite", "road", } },
    { "bridge-case-simple", { "line", "composite", "road", } },
    { "bridge-simple", { "line", "composite", "road", } },
    { "bridge-rail", { "line", "composite", "road", } },
    { "bridge-rail-tracks", { "line", "composite", "road", } },
    
    { "building-extrusion", { "fill-extrusion", "composite", "building", } },
    
    { "admin-1-boundary-bg", { "line", "composite", "admin", } },
    { "admin-0-boundary-bg", { "line", "composite", "admin", } },
    { "admin-1-boundary", { "line", "composite", "admin", } },
    { "admin-0-boundary", { "line", "composite", "admin", } },
    { "admin-0-boundary-disputed", { "line", "composite", "admin", } },
    
    { "road-label-simple", { "symbol", "composite", "road", } },
    { "waterway-label", { "symbol", "composite", "natural_label", } },
    { "natural-line-label", { "symbol", "composite", "natural_label", } },
    { "natural-point-label", { "symbol", "composite", "natural_label", } },
    { "water-line-label", { "symbol", "composite", "natural_label", } },
    { "water-point-label", { "symbol", "composite", "natural_label", } },
    { "poi-label", { "symbol", "composite", "poi_label", } },
    { "airport-label", { "symbol", "composite", "airport_label", } },
    { "settlement-subdivision-label", { "symbol", "composite", "place_label", } },
    { "settlement-minor-label", { "symbol", "composite", "place_label", } },
    { "settlement-major-label", { "symbol", "composite", "place_label", } },
    { "state-label", { "symbol", "composite", "place_label", } },
    { "country-label", { "symbol", "composite", "place_label", } },
    { "continent-label", { "symbol", "composite", "natural_label", } },
};


bool filter(const std::string& id) {
    return true;
//    return layerIds.find(id) != layerIds.end();
}

static std::map<std::string, int> layerRenderIndexs;
static std::mutex l;

int renderIndex(const std::string& id) {
    if (layerRenderIndexs.empty()) {
        std::lock_guard<std::mutex> lock(l);
        if (layerRenderIndexs.empty()) {
            int i = 0;
            for (auto it = layerIds.begin(); it != layerIds.end(); it++) {
                layerRenderIndexs[id] = i++;
            }
        }
    }
    
    return layerRenderIndexs[id];
}

static std::map<std::string, float> layerHeights = {
    { "bridge-path", 2.0 },
    { "bridge-steps", 2.0 },
    { "bridge-pedestrian", 2.0 },

    { "bridge-minor-case-navigation", 3.0 },
    { "bridge-minor-navigation", 3.0 },

    { "bridge-secondary-tertiary-case-navigation", 3.0 },
    { "bridge-secondary-tertiary-navigation", 3.0 },

    { "bridge-street-case-navigation", 3.5 },
    { "bridge-street-navigation", 3.5 },

    { "bridge-primary-case-navigation", 3.5 },
    { "bridge-primary-navigation", 3.5 },

    { "bridge-major-link-case-navigation", 4.0 },
    { "bridge-major-link-navigation", 4.0 },

    { "bridge-motorway-trunk-case-navigation", 4.5 },
    { "bridge-motorway-trunk-navigation", 4.5 },

    { "bridge-major-link-2-case-navigation", 4.5 },
    { "bridge-major-link-2-navigation", 4.5 },

    { "bridge-motorway-trunk-2-case-navigation", 5.0 },
    { "bridge-motorway-trunk-2-navigation", 5.0 },

    { "bridge-rail-bg-white", 5.5 },
    { "bridge-rail", 5.5 },
};

float getHeight(const std::string& layerId) {
    auto it = layerHeights.find(layerId);
    return it == layerHeights.end() ? 0 : it->second;
}


}
}
