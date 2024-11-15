//
//  nav.layer.hpp
//
//  Created by BernieZhao on 2023/11/21.
//

#include "mbgl/nav/nav.layer.hpp"
#include "mbgl/nav/nav.log.hpp"

#include <mutex>
#include <unordered_map>

namespace nav {
namespace layer {

void print(const char* id, const char* type, const char* source, const char* sourceLayer) {
    printf("layer : { \"%s\", { \"%s\", \"%s\", \"%s\", } }, \n", id, type, source, sourceLayer);
}

struct Source {
    const char* type;
    const char* source;
    const char* sourceLayer;
};

#define BACKGROUND
#define LINE
#define FILL
#define SYMBOL
#define TRAFFIC_SIGN
#define EXTRUSION
#define ROUTE_LINE

namespace mono {

static const std::unordered_map<std::string, Source> layerIds = {
    
#ifdef BACKGROUND
    { "land", { "background", "", "", } },
#endif
    
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
    
#ifdef TRAFFIC_SIGN
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
    
#ifdef TRAFFIC_SIGN
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
    
#ifdef TRAFFIC_SIGN
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
    
#ifdef ROUTE_LINE
    { "route", { "line", "composite", "road", } },
    { "route-dimmed", { "line", "composite", "road", } },
#endif
    
};

}

namespace pureness {

static const std::unordered_map<std::string, Source> layerIds = {

#ifdef BACKGROUND
    { "land", { "background", "", "", } },
#endif
    
#ifdef FILL
    { "landcover", { "fill", "composite", "landcover", } },
    { "national-park", { "fill", "composite", "landuse_overlay", } },
    { "landuse", { "fill", "composite", "landuse", } },
    { "hillshade", { "fill", "composite", "hillshade", } },
    { "pitch-outline", { "line", "composite", "landuse", } },
#endif

//    { "waterway-shadow", { "error", "null", "null", } },
//    { "water-shadow", { "error", "null", "null", } },
    
#ifdef LINE
    { "waterway", { "line", "composite", "waterway", } },
#endif

#ifdef FILL
    { "water", { "fill", "composite", "water", } },
    { "water-depth", { "fill", "composite", "depth", } },
    
    { "land-structure-polygon", { "fill", "composite", "structure", } },
    { "land-structure-line", { "line", "composite", "structure", } },
#endif
    
//    { "aeroway-polygon", { "fill", "composite", "aeroway", } },
//    { "aeroway-line", { "line", "composite", "aeroway", } },
    
#ifdef LINE
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

#ifdef TRAFFIC_SIGN
    { "tunnel-oneway-arrow-blue-navigation", { "symbol", "composite", "road", } },
    { "tunnel-oneway-arrow-white-navigation", { "symbol", "composite", "road", } },
#endif

#ifdef FILL
    { "road-pedestrian-polygon-fill", { "fill", "composite", "road", } },
    { "road-pedestrian-polygon-pattern", { "fill", "composite", "road", } },
#endif
    
#ifdef LINE
    { "road-path-bg", { "line", "composite", "road", } },
    { "road-steps-bg", { "line", "composite", "road", } },
    { "road-pedestrian-case", { "line", "composite", "road", } },
    { "road-path", { "line", "composite", "road", } },
    { "road-steps", { "line", "composite", "road", } },
    { "road-pedestrian", { "line", "composite", "road", } },
    { "golf-hole-line", { "line", "composite", "road", } },
    
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

#ifdef TRAFFIC_SIGN
    { "level-crossing-navigation", { "symbol", "composite", "road", } },
    { "road-oneway-arrow-blue-navigation", { "symbol", "composite", "road", } },
    { "road-oneway-arrow-white-navigation", { "symbol", "composite", "road", } },
    { "turning-feature-navigation", { "circle", "composite", "road", } },
    { "crosswalks", { "symbol", "composite", "structure", } },
#endif

#ifdef LINE
    { "ferry", { "line", "composite", "road", } },
    { "ferry-auto", { "line", "composite", "road", } },
    { "road-rail", { "line", "composite", "road", } },
    { "road-rail-tracks", { "line", "composite", "road", } },

    { "bridge-path-bg", { "line", "composite", "road", } },
    { "bridge-steps-bg", { "line", "composite", "road", } },
    { "bridge-pedestrian-case", { "line", "composite", "road", } },
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
    
#ifdef TRAFFIC_SIGN
    { "bridge-oneway-arrow-blue-navigation", { "symbol", "composite", "road", } },
    { "bridge-oneway-arrow-white-navigation", { "symbol", "composite", "road", } },
#endif
    
#ifdef LINE
    { "bridge-rail", { "line", "composite", "road", } },
    { "bridge-rail-tracks", { "line", "composite", "road", } },
    { "aerialway", { "line", "composite", "road", } },
#endif
    
#ifdef EXTRUSION
    { "building-extrusion", { "fill-extrusion", "composite", "building", } },
#endif
    
#ifdef LINE
    { "admin-1-boundary-bg", { "line", "composite", "admin", } },
    { "admin-0-boundary-bg", { "line", "composite", "admin", } },
    { "admin-1-boundary", { "line", "composite", "admin", } },
    { "admin-0-boundary", { "line", "composite", "admin", } },
    { "admin-0-boundary-disputed", { "line", "composite", "admin", } },
#endif
    
#ifdef SYMBOL
    { "building-entrance", { "symbol", "composite", "structure", } },
    { "building-number-label", { "symbol", "composite", "housenum_label", } },
    { "block-number-label", { "symbol", "composite", "place_label", } },
    { "road-intersection", { "symbol", "composite", "road", } },
    { "road-label-navigation", { "symbol", "composite", "road", } },
    { "road-number-shield-navigation", { "symbol", "composite", "road", } },
    { "road-exit-shield-navigation", { "symbol", "composite", "motorway_junction", } },
    { "path-pedestrian-label", { "symbol", "composite", "road", } },
    { "golf-hole-label", { "symbol", "composite", "road", } },
    { "ferry-aerialway-label", { "symbol", "composite", "road", } },
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

static const std::unordered_map<std::string, Source> trafficIds = {

    { "tunnel-minor-navigation", { "line", "composite", "road", } },
    { "tunnel-major-link-navigation", { "line", "composite", "road", } },
    { "tunnel-street-navigation", { "line", "composite", "road", } },
    { "tunnel-street-low-navigation", { "line", "composite", "road", } },
    { "tunnel-secondary-tertiary-navigation", { "line", "composite", "road", } },
    { "tunnel-primary-navigation", { "line", "composite", "road", } },
    { "tunnel-motorway-trunk-navigation", { "line", "composite", "road", } },

    { "road-minor-navigation", { "line", "composite", "road", } },
    { "road-major-link-navigation", { "line", "composite", "road", } },
    { "road-street-navigation", { "line", "composite", "road", } },
    { "road-street-low-navigation", { "line", "composite", "road", } },
    { "road-secondary-tertiary-navigation", { "line", "composite", "road", } },
    { "road-primary-navigation", { "line", "composite", "road", } },
    { "road-motorway-trunk-navigation", { "line", "composite", "road", } },

    { "bridge-minor-navigation", { "line", "composite", "road", } },
    { "bridge-major-link-navigation", { "line", "composite", "road", } },
    { "bridge-street-navigation", { "line", "composite", "road", } },
    { "bridge-street-low-navigation", { "line", "composite", "road", } },
    { "bridge-secondary-tertiary-navigation", { "line", "composite", "road", } },
    { "bridge-primary-navigation", { "line", "composite", "road", } },
    { "bridge-motorway-trunk-navigation", { "line", "composite", "road", } },
    { "bridge-major-link-2-navigation", { "line", "composite", "road", } },
    { "bridge-motorway-trunk-2-navigation", { "line", "composite", "road", } },

};

}

const auto& layerIds() {
    return mono::layerIds;
    return pureness::layerIds;
}

const auto& trafficIds() {
    return pureness::trafficIds;
}



bool isAvailable(const std::string& id) {
    return layerIds().find(id) != layerIds().end();
}

bool hasTrafficConditions(const std::string& id) {
    return trafficIds().find(id) != trafficIds().end();
}

std::unordered_map<std::string, float> layerHeights = {
    { "bridge-path", 1.0 },
    { "bridge-steps", 1.0 },
    { "bridge-pedestrian", 1.0 },

    { "bridge-minor-case-navigation", 1.5 },
    { "bridge-minor-navigation", 1.5 },

    { "bridge-secondary-tertiary-case-navigation", 1.5 },
    { "bridge-secondary-tertiary-navigation", 1.5 },

    { "bridge-street-case-navigation", 1.75 },
    { "bridge-street-navigation", 1.75 },

    { "bridge-primary-case-navigation", 1.75 },
    { "bridge-primary-navigation", 1.75 },

    { "bridge-major-link-case-navigation", 2.0 },
    { "bridge-major-link-navigation", 2.0 },

    { "bridge-motorway-trunk-case-navigation", 2.25 },
    { "bridge-motorway-trunk-navigation", 2.25 },

    { "bridge-major-link-2-case-navigation", 2.25 },
    { "bridge-major-link-2-navigation", 2.25 },

    { "bridge-motorway-trunk-2-case-navigation", 2.5 },
    { "bridge-motorway-trunk-2-navigation", 2.5 },

    { "bridge-rail-bg-white", 2.75 },
    { "bridge-rail", 2.75 },
};

float getHeight(const std::string& layerId) {
    auto it = layerHeights.find(layerId);
    if (it != layerHeights.end()) return it->second;
    else return 0.;
}

}
}
