//
//  nav_mb_layer_filter.hpp
//
//  Created by BernieZhao on 2023/11/21.
//

#include "mbgl/nav/nav_mb_layer.hpp"
#include "mbgl/nav/nav_log.hpp"

#include <mutex>
#include <unordered_map>

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

#define LINE
#define FILL
#define SYMBOL
#define EXTRUSION

static const std::unordered_map<std::string, Source> layerIds = {
    { "land", { "background", "", "", } },
    
#ifdef FILL
    { "national-park", { "fill", "composite", "landuse_overlay", } },
    { "landuse", { "fill", "composite", "landuse", } },
    { "hillshade", { "fill", "composite", "hillshade", } },
    
    { "waterway", { "line", "composite", "waterway", } },
    { "water", { "fill", "composite", "water", } },
    { "water-depth", { "fill", "composite", "depth", } },
    { "land-structure-polygon", { "fill", "composite", "structure", } },
    { "land-structure-line", { "line", "composite", "structure", } },
    
    { "aeroway-polygon", { "fill", "composite", "aeroway", } },
#endif
    
#ifdef LINE
    { "aeroway-line", { "line", "composite", "aeroway", } },
    
    { "tunnel-path-trail", { "line", "composite", "road", } },
    { "tunnel-path-cycleway-piste", { "line", "composite", "road", } },
    { "tunnel-path", { "line", "composite", "road", } },
    { "tunnel-steps", { "line", "composite", "road", } },
    { "tunnel-pedestrian", { "line", "composite", "road", } },
    { "tunnel-minor-case-navigation", { "line", "composite", "road", } },
    { "tunnel-street-case-navigation", { "line", "composite", "road", } },
    { "tunnel-secondary-tertiary-case-navigation", { "line", "composite", "road", } },
    { "tunnel-primary-case-navigation", { "line", "composite", "road", } },
    { "tunnel-major-link-case-navigation", { "line", "composite", "road", } },
    { "tunnel-motorway-trunk-case-navigation", { "line", "composite", "road", } },
    { "tunnel-construction-navigation", { "line", "composite", "road", } },
    { "tunnel-minor-navigation", { "line", "composite", "road", } },
    { "tunnel-major-link-navigation", { "line", "composite", "road", } },
    { "tunnel-street-navigation", { "line", "composite", "road", } },
    { "tunnel-street-low-navigation", { "line", "composite", "road", } },
    { "tunnel-secondary-tertiary-navigation", { "line", "composite", "road", } },
    { "tunnel-primary-navigation", { "line", "composite", "road", } },
    { "tunnel-motorway-trunk-navigation", { "line", "composite", "road", } },
#endif
    
#ifdef SYMBOL
    { "tunnel-oneway-arrow-blue-navigation", { "symbol", "composite", "road", } },
    { "tunnel-oneway-arrow-white-navigation", { "symbol", "composite", "road", } },
#endif
    
#ifdef LINE
    { "road-path-trail", { "line", "composite", "road", } },
    { "road-path-cycleway-piste", { "line", "composite", "road", } },
    { "road-path", { "line", "composite", "road", } },
    { "road-steps", { "line", "composite", "road", } },
    { "road-pedestrian", { "line", "composite", "road", } },
    { "turning-feature-outline-navigation", { "circle", "composite", "road", } },

    { "road-minor-case-navigation", { "line", "composite", "road", } },
    { "road-street-case-navigation", { "line", "composite", "road", } },
    { "road-secondary-tertiary-case-navigation", { "line", "composite", "road", } },
    { "road-primary-case-navigation", { "line", "composite", "road", } },
    { "road-major-link-case-navigation", { "line", "composite", "road", } },
    { "road-motorway-trunk-case-navigation", { "line", "composite", "road", } },
    { "road-construction-navigation", { "line", "composite", "road", } },
    { "road-minor-navigation", { "line", "composite", "road", } },
    { "road-major-link-navigation", { "line", "composite", "road", } },
    { "road-street-navigation", { "line", "composite", "road", } },
    { "road-street-low-navigation", { "line", "composite", "road", } },
    { "road-secondary-tertiary-navigation", { "line", "composite", "road", } },
    { "road-primary-navigation", { "line", "composite", "road", } },
    { "road-motorway-trunk-case-low-navigation", { "line", "composite", "road", } },
    { "road-motorway-trunk-navigation", { "line", "composite", "road", } },
#endif

#ifdef SYMBOL
    { "level-crossing-navigation", { "symbol", "composite", "road", } },
    { "road-oneway-arrow-blue-navigation", { "symbol", "composite", "road", } },
    { "road-oneway-arrow-white-navigation", { "symbol", "composite", "road", } },
    
    { "turning-feature-navigation", { "circle", "composite", "road", } },
    { "crosswalks", { "symbol", "composite", "structure", } }, // 斑马线
#endif
    
#ifdef LINE
    { "road-rail", { "line", "composite", "road", } },
    { "road-rail-tracks", { "line", "composite", "road", } },
    { "bridge-path-trail", { "line", "composite", "road", } },
    { "bridge-path-cycleway-piste", { "line", "composite", "road", } },
    { "bridge-path", { "line", "composite", "road", } },
    { "bridge-steps", { "line", "composite", "road", } },
    { "bridge-pedestrian", { "line", "composite", "road", } },
    { "bridge-minor-case-navigation", { "line", "composite", "road", } },
    { "bridge-street-case-navigation", { "line", "composite", "road", } },
    { "bridge-secondary-tertiary-case-navigation", { "line", "composite", "road", } },
    { "bridge-primary-case-navigation", { "line", "composite", "road", } },
    { "bridge-major-link-case-navigation", { "line", "composite", "road", } },
    { "bridge-motorway-trunk-case-navigation", { "line", "composite", "road", } },
    { "bridge-construction-navigation", { "line", "composite", "road", } },
    { "bridge-minor-navigation", { "line", "composite", "road", } },
    { "bridge-major-link-navigation", { "line", "composite", "road", } },
    { "bridge-street-navigation", { "line", "composite", "road", } },
    { "bridge-street-low-navigation", { "line", "composite", "road", } },
    { "bridge-secondary-tertiary-navigation", { "line", "composite", "road", } },
    { "bridge-primary-navigation", { "line", "composite", "road", } },
    { "bridge-motorway-trunk-navigation", { "line", "composite", "road", } },
    { "bridge-major-link-2-case-navigation", { "line", "composite", "road", } },
    { "bridge-motorway-trunk-2-case-navigation", { "line", "composite", "road", } },
    { "bridge-major-link-2-navigation", { "line", "composite", "road", } },
    { "bridge-motorway-trunk-2-navigation", { "line", "composite", "road", } },
#endif
    
#ifdef SYMBOL
    { "bridge-oneway-arrow-blue-navigation", { "symbol", "composite", "road", } },
    { "bridge-oneway-arrow-white-navigation", { "symbol", "composite", "road", } },
#endif
    
#ifdef LINE
    { "bridge-rail", { "line", "composite", "road", } },
    { "bridge-rail-tracks", { "line", "composite", "road", } },
#endif
    
#ifdef EXTRUSION
    { "building-extrusion", { "fill-extrusion", "composite", "building", } },
#endif
    
#ifdef LINE
    { "admin-2-boundary-bg", { "line", "composite", "admin", } },
    { "admin-1-boundary-bg", { "line", "composite", "admin", } },
    { "admin-0-boundary-bg", { "line", "composite", "admin", } },
    { "admin-2-boundary", { "line", "composite", "admin", } },
    { "admin-1-boundary", { "line", "composite", "admin", } },
    { "admin-0-boundary", { "line", "composite", "admin", } },
#endif
    
#ifdef SYMBOL
    { "building-entrance", { "symbol", "composite", "structure", } },
    { "building-number-label", { "symbol", "composite", "housenum_label", } },
    { "block-number-label", { "symbol", "composite", "place_label", } },
    
    { "road-intersection", { "symbol", "composite", "road", } },
    { "traffic-signal-navigation", { "error", "null", "null", } },
    { "road-label-navigation", { "symbol", "composite", "road", } },
    { "road-number-shield-navigation", { "symbol", "composite", "road", } },
    { "road-exit-shield-navigation", { "symbol", "composite", "motorway_junction", } },
    
    { "path-pedestrian-label", { "symbol", "composite", "road", } },
    { "waterway-label", { "symbol", "composite", "natural_label", } },
    { "natural-line-label", { "symbol", "composite", "natural_label", } },
    { "natural-point-label", { "symbol", "composite", "natural_label", } },
    { "water-line-label", { "symbol", "composite", "natural_label", } },
    { "water-point-label", { "symbol", "composite", "natural_label", } },
    
    { "poi-label", { "symbol", "composite", "poi_label", } },
    
    { "transit-label", { "symbol", "composite", "transit_stop_label", } },
    { "airport-label", { "symbol", "composite", "airport_label", } },
    
    { "settlement-subdivision-label", { "symbol", "composite", "place_label", } },
    { "settlement-minor-label", { "symbol", "composite", "place_label", } },
    { "settlement-major-label", { "symbol", "composite", "place_label", } },
    { "state-label", { "symbol", "composite", "place_label", } },
    { "country-label", { "symbol", "composite", "place_label", } },
    { "continent-label", { "symbol", "composite", "natural_label", } },
#endif

};


bool filter(const std::string& id) {
    return true;
//    return layerIds.find(id) != layerIds.end();
}

static std::unordered_map<std::string, float> layerHeights = {
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
