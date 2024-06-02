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

namespace mb {

void printLayerTable(const char* id, const char* type, const char* source, const char* sourceLayer) {
    nav::printf("{ \"%s\", { \"%s\", \"%s\", \"%s\", } },  \n", id, type, source, sourceLayer);
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
};


bool filterLayerByType(const std::string& id) {
    return true;
//    return layerIds.find(id) != layerIds.end();
}

static std::map<std::string, int> layerRenderIndexs;
static std::mutex l;

int layerRenderIndex(const std::string& id) {
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

}

}
