//
//  nav_mb_json.hpp
//  Mapbox GL Native
//
//  Created by BernieZhao on 2023/11/23.
//

#ifndef nav_mb_json_h
#define nav_mb_json_h

#include "mbgl/nav/nav_log.hpp"

namespace nav {
namespace mb {

void printJson(const std::string& json) {
//    nav::log::v("Style Json", "%s", json.c_str());
}

}
}

/*

{
    "version": 8,
    "name": "Monochrome",
    "metadata": {
        "mapbox:type": "default",
        "mapbox:variation": "monochrome-sky-v1",
        "mapbox:autocomposite": true,
        "mapbox:sdk-support": {
            "js": "3.3.0",
            "android": "11.2.0",
            "ios": "11.2.0"
        },
        "mapbox:origin": "monochrome-dark-v1",
        "mapbox:groups": {
            "Transit, transit-labels": {
                "name": "Transit, transit-labels",
                "collapsed": true
            },
            "Administrative boundaries, admin": {
                "name": "Administrative boundaries, admin",
                "collapsed": true
            },
            "Land & water, built": {
                "name": "Land & water, built",
                "collapsed": true
            },
            "Transit, bridges": {
                "name": "Transit, bridges",
                "collapsed": true
            },
            "Buildings, building-labels": {
                "collapsed": false,
                "name": "Buildings, building-labels"
            },
            "Transit, surface": {
                "name": "Transit, surface",
                "collapsed": true
            },
            "Land & water, land": {
                "name": "Land & water, land",
                "collapsed": true
            },
            "Road network, bridges": {
                "name": "Road network, bridges",
                "collapsed": true
            },
            "Road network, tunnels": {
                "name": "Road network, tunnels",
                "collapsed": true
            },
            "Road network, road-labels": {
                "name": "Road network, road-labels",
                "collapsed": true
            },
            "Buildings, built": {
                "name": "Buildings, built",
                "collapsed": false
            },
            "Natural features, natural-labels": {
                "name": "Natural features, natural-labels",
                "collapsed": true
            },
            "Road network, surface": {
                "name": "Road network, surface",
                "collapsed": true
            },
            "Walking, cycling, etc., barriers-bridges": {
                "name": "Walking, cycling, etc., barriers-bridges",
                "collapsed": true
            },
            "Place labels, place-labels": {
                "name": "Place labels, place-labels",
                "collapsed": true
            },
            "Buildings, extruded": {
                "name": "Buildings, extruded",
                "collapsed": true
            },
            "Point of interest labels, poi-labels": {
                "name": "Point of interest labels, poi-labels",
                "collapsed": false
            },
            "Walking, cycling, etc., tunnels": {
                "name": "Walking, cycling, etc., tunnels",
                "collapsed": true
            },
            "Walking, cycling, etc., walking-cycling-labels": {
                "collapsed": true,
                "name": "Walking, cycling, etc., walking-cycling-labels"
            },
            "Walking, cycling, etc., surface": {
                "name": "Walking, cycling, etc., surface",
                "collapsed": true
            },
            "Transit, built": {
                "name": "Transit, built",
                "collapsed": true
            },
            "Land & water, water": {
                "name": "Land & water, water",
                "collapsed": false
            }
        }
    },
    "center": [114.1544521105501, 22.285364593485696],
    "zoom": 16.304083860030197,
    "bearing": 128,
    "pitch": 42,
    "fog": {
        "range": [-1, 10],
        "color": "hsl(202, 61%, 91%)",
        "high-color": "hsl(193, 73%, 70%)",
        "space-color": "hsl(244, 56%, 55%)",
        "horizon-blend": 0.1,
        "star-intensity": 0
    },
    "sources": {
        "composite": {
            "url": "mapbox://mapbox.mapbox-streets-v8,mapbox.mapbox-terrain-v2",
            "type": "vector"
        }
    },
    "sprite": "mapbox://sprites/notheorem/clwvwdd2r01d301nyayhc6p6g/2qkxnlln3j4n7swhvmzr1l4if",
    "glyphs": "mapbox://fonts/mapbox/{fontstack}/{range}.pbf",
    "projection": {
        "name": "globe"
    },
    "layers": [{
        "id": "land",
        "type": "background",
        "layout": {},
        "paint": {
            "background-color": ["interpolate", ["linear"],
                ["zoom"], 9, "hsla(215, 2%, 41%, 0.86)", 11, "hsla(215, 0%, 28%, 0.86)"
            ]
        },
        "metadata": {
            "mapbox:featureComponent": "land-and-water",
            "mapbox:group": "Land & water, land"
        }
    }, {
        "minzoom": 5,
        "layout": {},
        "metadata": {
            "mapbox:featureComponent": "land-and-water",
            "mapbox:group": "Land & water, land"
        },
        "filter": ["==", ["get", "class"], "national_park"],
        "type": "fill",
        "source": "composite",
        "id": "national-park",
        "paint": {
            "fill-color": "hsla(215, 0%, 26%, 0.86)",
            "fill-opacity": ["interpolate", ["linear"],
                ["zoom"], 5, 0, 6, 0.6, 12, 0.2
            ]
        },
        "source-layer": "landuse_overlay"
    }, {
        "minzoom": 5,
        "layout": {},
        "metadata": {
            "mapbox:featureComponent": "land-and-water",
            "mapbox:group": "Land & water, land"
        },
        "filter": ["all", [">=", ["to-number", ["get", "sizerank"]], 0],
            ["match", ["get", "class"],
                ["agriculture", "wood", "grass", "scrub", "glacier", "pitch", "sand"],
                ["step", ["zoom"], false, 11, true], "residential", ["step", ["zoom"], true, 10, false],
                ["park", "airport"],
                ["step", ["zoom"], false, 8, ["case", ["==", ["get", "sizerank"], 1], true, false], 10, true], false
            ],
            ["<=", ["-", ["to-number", ["get", "sizerank"]],
                ["interpolate", ["exponential", 1.5],
                    ["zoom"], 12, 0, 18, 14
                ]
            ], 8]
        ],
        "type": "fill",
        "source": "composite",
        "id": "landuse",
        "paint": {
            "fill-color": "hsla(215, 0%, 26%, 0.86)",
            "fill-opacity": ["interpolate", ["linear"],
                ["zoom"], 8, ["match", ["get", "class"], "residential", 0.8, 0.2], 10, ["match", ["get", "class"], "residential", 0, 1]
            ],
            "fill-antialias": false
        },
        "source-layer": "landuse"
    }, {
        "layout": {},
        "metadata": {
            "mapbox:featureComponent": "land-and-water",
            "mapbox:group": "Land & water, land"
        },
        "maxzoom": 16,
        "filter": ["all", ["step", ["zoom"],
                ["==", ["get", "class"], "shadow"], 11, true
            ],
            ["match", ["get", "level"], 89, true, 78, ["step", ["zoom"], false, 5, true], 67, ["step", ["zoom"], false, 9, true], 56, ["step", ["zoom"], false, 6, true], 94, ["step", ["zoom"], false, 11, true], 90, ["step", ["zoom"], false, 12, true], false]
        ],
        "type": "fill",
        "source": "composite",
        "id": "hillshade",
        "paint": {
            "fill-color": ["interpolate", ["linear"],
                ["zoom"], 14, ["match", ["get", "class"], "shadow", "hsla(215, 0%, 41%, 0.06)", "hsla(215, 0%, 36%, 0.04)"], 16, ["match", ["get", "class"], "shadow", "hsla(215, 0%, 41%, 0)", "hsla(215, 0%, 36%, 0)"]
            ],
            "fill-antialias": false
        },
        "source-layer": "hillshade"
    }, {
        "id": "waterway",
        "type": "line",
        "source": "composite",
        "source-layer": "waterway",
        "minzoom": 8,
        "layout": {
            "line-cap": ["step", ["zoom"], "butt", 11, "round"],
            "line-join": ["step", ["zoom"], "miter", 11, "round"]
        },
        "paint": {
            "line-color": "hsla(215, 0%, 21%, 0.86)",
            "line-width": ["interpolate", ["exponential", 1.3],
                ["zoom"], 9, ["match", ["get", "class"],
                    ["canal", "river"], 0.1, 0
                ], 20, ["match", ["get", "class"],
                    ["canal", "river"], 8, 3
                ]
            ],
            "line-opacity": ["interpolate", ["linear"],
                ["zoom"], 8, 0, 8.5, 1
            ]
        },
        "metadata": {
            "mapbox:featureComponent": "land-and-water",
            "mapbox:group": "Land & water, water"
        }
    }, {
        "id": "water",
        "type": "fill",
        "source": "composite",
        "source-layer": "water",
        "layout": {},
        "paint": {
            "fill-color": "hsla(215, 0%, 21%, 0.86)"
        },
        "metadata": {
            "mapbox:featureComponent": "land-and-water",
            "mapbox:group": "Land & water, water"
        }
    }, {
        "minzoom": 13,
        "layout": {},
        "metadata": {
            "mapbox:featureComponent": "land-and-water",
            "mapbox:group": "Land & water, built"
        },
        "filter": ["all", ["==", ["get", "class"], "land"],
            ["==", ["geometry-type"], "Polygon"]
        ],
        "type": "fill",
        "source": "composite",
        "id": "land-structure-polygon",
        "paint": {
            "fill-color": ["interpolate", ["linear"],
                ["zoom"], 9, "hsla(215, 2%, 41%, 0.86)", 11, "hsla(215, 0%, 28%, 0.86)"
            ]
        },
        "source-layer": "structure"
    }, {
        "minzoom": 13,
        "layout": {
            "line-cap": "square"
        },
        "metadata": {
            "mapbox:featureComponent": "land-and-water",
            "mapbox:group": "Land & water, built"
        },
        "filter": ["all", ["==", ["get", "class"], "land"],
            ["==", ["geometry-type"], "LineString"]
        ],
        "type": "line",
        "source": "composite",
        "id": "land-structure-line",
        "paint": {
            "line-width": ["interpolate", ["exponential", 1.99],
                ["zoom"], 14, 0.75, 20, 40
            ],
            "line-color": ["interpolate", ["linear"],
                ["zoom"], 9, "hsla(215, 2%, 41%, 0.86)", 11, "hsla(215, 0%, 28%, 0.86)"
            ]
        },
        "source-layer": "structure"
    }, {
        "id": "aeroway-polygon",
        "type": "fill",
        "source": "composite",
        "source-layer": "aeroway",
        "minzoom": 11,
        "filter": ["all", ["match", ["get", "type"],
                ["runway", "taxiway", "helipad"], true, false
            ],
            ["==", ["geometry-type"], "Polygon"]
        ],
        "paint": {
            "fill-color": "hsla(215, 0%, 37%, 0.86)",
            "fill-opacity": ["interpolate", ["linear"],
                ["zoom"], 10, 0, 11, 1
            ]
        },
        "metadata": {
            "mapbox:featureComponent": "transit",
            "mapbox:group": "Transit, built"
        }
    }, {
        "id": "aeroway-line",
        "type": "line",
        "source": "composite",
        "source-layer": "aeroway",
        "minzoom": 9,
        "filter": ["==", ["geometry-type"], "LineString"],
        "paint": {
            "line-color": "hsla(215, 0%, 37%, 0.86)",
            "line-width": ["interpolate", ["exponential", 1.5],
                ["zoom"], 9, ["match", ["get", "type"], "runway", 1, 0.5], 18, ["match", ["get", "type"], "runway", 80, 20]
            ],
            "line-opacity": ["interpolate", ["linear"],
                ["zoom"], 10, 0, 11, 1
            ]
        },
        "metadata": {
            "mapbox:featureComponent": "transit",
            "mapbox:group": "Transit, built"
        }
    }, {
        "minzoom": 15,
        "layout": {
            "line-cap": "round"
        },
        "metadata": {
            "mapbox:featureComponent": "walking-cycling",
            "mapbox:group": "Walking, cycling, etc., tunnels"
        },
        "filter": ["all", ["==", ["get", "structure"], "tunnel"],
            ["==", ["get", "class"], "path"],
            ["match", ["get", "type"],
                ["hiking", "mountain_bike", "trail"], true, false
            ],
            ["==", ["geometry-type"], "LineString"]
        ],
        "type": "line",
        "source": "composite",
        "id": "tunnel-path-trail",
        "paint": {
            "line-width": ["interpolate", ["exponential", 1.5],
                ["zoom"], 15, 1, 18, 4
            ],
            "line-color": "hsla(215, 0%, 23%, 0.86)",
            "line-dasharray": [10, 0]
        },
        "source-layer": "road"
    }, {
        "minzoom": 12,
        "layout": {},
        "metadata": {
            "mapbox:featureComponent": "walking-cycling",
            "mapbox:group": "Walking, cycling, etc., tunnels"
        },
        "filter": ["all", ["==", ["get", "structure"], "tunnel"],
            ["==", ["get", "class"], "path"],
            ["match", ["get", "type"],
                ["cycleway", "piste"], true, false
            ],
            ["==", ["geometry-type"], "LineString"]
        ],
        "type": "line",
        "source": "composite",
        "id": "tunnel-path-cycleway-piste",
        "paint": {
            "line-width": ["interpolate", ["exponential", 1.5],
                ["zoom"], 15, 1, 18, 4
            ],
            "line-color": "hsla(215, 0%, 23%, 0.86)",
            "line-dasharray": [10, 0]
        },
        "source-layer": "road"
    }, {
        "minzoom": 15,
        "layout": {
            "line-cap": "round"
        },
        "metadata": {
            "mapbox:featureComponent": "walking-cycling",
            "mapbox:group": "Walking, cycling, etc., tunnels"
        },
        "filter": ["all", ["==", ["get", "structure"], "tunnel"],
            ["==", ["get", "class"], "path"],
            ["!=", ["get", "type"], "steps"],
            ["==", ["geometry-type"], "LineString"]
        ],
        "type": "line",
        "source": "composite",
        "id": "tunnel-path",
        "paint": {
            "line-width": ["interpolate", ["exponential", 1.5],
                ["zoom"], 15, 1, 18, 4
            ],
            "line-color": "hsla(215, 0%, 23%, 0.86)",
            "line-dasharray": [10, 0]
        },
        "source-layer": "road"
    }, {
        "id": "tunnel-steps",
        "type": "line",
        "source": "composite",
        "source-layer": "road",
        "minzoom": 15,
        "filter": ["all", ["==", ["get", "structure"], "tunnel"],
            ["==", ["get", "type"], "steps"],
            ["==", ["geometry-type"], "LineString"]
        ],
        "paint": {
            "line-width": ["interpolate", ["exponential", 1.5],
                ["zoom"], 15, 1, 16, 1.6, 18, 6
            ],
            "line-color": "hsla(215, 0%, 23%, 0.86)",
            "line-dasharray": ["step", ["zoom"],
                ["literal", [1, 0]], 15, ["literal", [1.75, 1]], 16, ["literal", [1, 0.75]], 17, ["literal", [0.3, 0.3]]
            ]
        },
        "metadata": {
            "mapbox:featureComponent": "walking-cycling",
            "mapbox:group": "Walking, cycling, etc., tunnels"
        }
    }, {
        "id": "tunnel-pedestrian",
        "type": "line",
        "source": "composite",
        "source-layer": "road",
        "minzoom": 15,
        "filter": ["all", ["==", ["get", "structure"], "tunnel"],
            ["==", ["get", "class"], "pedestrian"],
            ["==", ["geometry-type"], "LineString"]
        ],
        "paint": {
            "line-width": ["interpolate", ["exponential", 1.5],
                ["zoom"], 14, 0.5, 18, 12
            ],
            "line-color": "hsla(215, 0%, 23%, 0.86)",
            "line-dasharray": [10, 0]
        },
        "metadata": {
            "mapbox:featureComponent": "walking-cycling",
            "mapbox:group": "Walking, cycling, etc., tunnels"
        }
    }, {
        "minzoom": 13,
        "layout": {},
        "metadata": {
            "mapbox:featureComponent": "road-network",
            "mapbox:group": "Road network, tunnels"
        },
        "filter": ["all", ["==", ["get", "structure"], "tunnel"],
            ["step", ["zoom"],
                ["match", ["get", "class"],
                    ["motorway", "motorway_link", "trunk", "trunk_link", "primary", "secondary", "tertiary", "street", "street_limited", "primary_link", "track"], true, false
                ], 14, ["match", ["get", "class"],
                    ["motorway", "motorway_link", "trunk", "trunk_link", "primary", "primary_link", "secondary", "secondary_link", "tertiary", "tertiary_link", "street", "street_limited", "service", "track"], true, false
                ]
            ],
            ["==", ["geometry-type"], "LineString"]
        ],
        "type": "line",
        "source": "composite",
        "id": "tunnel-simple",
        "paint": {
            "line-width": ["interpolate", ["exponential", 1.5],
                ["zoom"], 13, ["match", ["get", "class"],
                    ["motorway", "trunk", "primary"], 3.2, ["secondary", "tertiary"], 2, ["motorway_link", "trunk_link", "street", "street_limited", "primary_link"], 0.8, 0.5
                ], 18, ["match", ["get", "class"],
                    ["motorway", "trunk", "primary"], 25.6, ["secondary", "tertiary"], 20.8, ["motorway_link", "trunk_link", "street", "street_limited", "primary_link"], 14.4, 9.600000000000001
                ], 22, ["match", ["get", "class"],
                    ["motorway", "trunk", "primary"], 256, ["secondary", "tertiary"], 208, ["motorway_link", "trunk_link", "primary_link", "street", "street_limited"], 144, 96
                ]
            ],
            "line-color": "hsla(29, 73%, 62%, 0.4)"
        },
        "source-layer": "road"
    }, {
        "minzoom": 12,
        "layout": {
            "line-cap": "round",
            "line-join": ["step", ["zoom"], "miter", 14, "round"]
        },
        "metadata": {
            "mapbox:featureComponent": "walking-cycling",
            "mapbox:group": "Walking, cycling, etc., surface"
        },
        "filter": ["all", ["==", ["get", "class"], "path"],
            ["match", ["get", "type"],
                ["hiking", "mountain_bike", "trail"], true, false
            ],
            ["match", ["get", "structure"],
                ["none", "ford"], true, false
            ],
            ["==", ["geometry-type"], "LineString"]
        ],
        "type": "line",
        "source": "composite",
        "id": "road-path-trail",
        "paint": {
            "line-width": ["interpolate", ["exponential", 1.5],
                ["zoom"], 15, 1, 18, 4
            ],
            "line-color": "hsla(215, 0%, 36%, 0.86)",
            "line-dasharray": [10, 0]
        },
        "source-layer": "road"
    }, {
        "minzoom": 12,
        "layout": {
            "line-join": ["step", ["zoom"], "miter", 14, "round"]
        },
        "metadata": {
            "mapbox:featureComponent": "walking-cycling",
            "mapbox:group": "Walking, cycling, etc., surface"
        },
        "filter": ["all", ["==", ["get", "class"], "path"],
            ["match", ["get", "type"],
                ["cycleway", "piste"], true, false
            ],
            ["match", ["get", "structure"],
                ["none", "ford"], true, false
            ],
            ["==", ["geometry-type"], "LineString"]
        ],
        "type": "line",
        "source": "composite",
        "id": "road-path-cycleway-piste",
        "paint": {
            "line-width": ["interpolate", ["exponential", 1.5],
                ["zoom"], 15, 1, 18, 4
            ],
            "line-color": "hsla(215, 0%, 36%, 0.86)",
            "line-dasharray": [10, 0]
        },
        "source-layer": "road"
    }, {
        "minzoom": 12,
        "layout": {
            "line-cap": "round",
            "line-join": ["step", ["zoom"], "miter", 14, "round"]
        },
        "metadata": {
            "mapbox:featureComponent": "walking-cycling",
            "mapbox:group": "Walking, cycling, etc., surface"
        },
        "filter": ["all", ["==", ["get", "class"], "path"],
            ["step", ["zoom"],
                ["!", ["match", ["get", "type"],
                    ["steps", "sidewalk", "crossing"], true, false
                ]], 16, ["!=", ["get", "type"], "steps"]
            ],
            ["match", ["get", "structure"],
                ["none", "ford"], true, false
            ],
            ["==", ["geometry-type"], "LineString"]
        ],
        "type": "line",
        "source": "composite",
        "id": "road-path",
        "paint": {
            "line-width": ["interpolate", ["exponential", 1.5],
                ["zoom"], 13, 0.5, 14, 1, 15, 1, 18, 4
            ],
            "line-color": "rgba(251, 213, 177, 0.86)",
            "line-dasharray": [10, 0]
        },
        "source-layer": "road"
    }, {
        "minzoom": 14,
        "layout": {
            "line-join": "round"
        },
        "metadata": {
            "mapbox:featureComponent": "walking-cycling",
            "mapbox:group": "Walking, cycling, etc., surface"
        },
        "filter": ["all", ["==", ["get", "type"], "steps"],
            ["match", ["get", "structure"],
                ["none", "ford"], true, false
            ],
            ["==", ["geometry-type"], "LineString"]
        ],
        "type": "line",
        "source": "composite",
        "id": "road-steps",
        "paint": {
            "line-width": ["interpolate", ["exponential", 1.5],
                ["zoom"], 15, 1, 16, 1.6, 18, 6
            ],
            "line-color": "hsla(215, 0%, 36%, 0.86)",
            "line-dasharray": ["step", ["zoom"],
                ["literal", [1, 0]], 15, ["literal", [1.75, 1]], 16, ["literal", [1, 0.75]], 17, ["literal", [0.3, 0.3]]
            ]
        },
        "source-layer": "road"
    }, {
        "minzoom": 12,
        "layout": {
            "line-cap": "round",
            "line-join": ["step", ["zoom"], "miter", 14, "round"]
        },
        "metadata": {
            "mapbox:featureComponent": "walking-cycling",
            "mapbox:group": "Walking, cycling, etc., surface"
        },
        "filter": ["all", ["==", ["get", "class"], "pedestrian"],
            ["match", ["get", "structure"],
                ["none", "ford"], true, false
            ],
            ["case", ["has", "layer"],
                [">=", ["get", "layer"], 0], true
            ],
            ["==", ["geometry-type"], "LineString"]
        ],
        "type": "line",
        "source": "composite",
        "id": "road-pedestrian",
        "paint": {
            "line-width": ["interpolate", ["exponential", 1.5],
                ["zoom"], 14, 0.5, 18, 12
            ],
            "line-color": "hsla(29, 75%, 91%, 0.86)",
            "line-dasharray": [10, 0]
        },
        "source-layer": "road"
    }, {
        "minzoom": 5,
        "layout": {
            "line-cap": ["step", ["zoom"], "butt", 14, "round"],
            "line-join": ["step", ["zoom"], "miter", 14, "round"]
        },
        "filter": ["all", ["step", ["zoom"],
                ["match", ["get", "class"],
                    ["motorway", "trunk"], true, false
                ], 6, ["match", ["get", "class"],
                    ["motorway", "trunk", "primary"], true, false
                ], 8, ["match", ["get", "class"],
                    ["motorway", "trunk", "primary", "secondary"], true, false
                ], 10, ["match", ["get", "class"],
                    ["motorway", "trunk", "primary", "secondary", "tertiary", "motorway_link", "trunk_link"], true, false
                ], 11, ["match", ["get", "class"],
                    ["motorway", "motorway_link", "trunk", "trunk_link", "primary", "secondary", "tertiary", "street"], true, false
                ], 12, ["match", ["get", "class"],
                    ["motorway", "motorway_link", "trunk", "trunk_link", "primary", "secondary", "tertiary", "street", "street_limited", "primary_link"], true, false
                ], 13, ["match", ["get", "class"],
                    ["motorway", "motorway_link", "trunk", "trunk_link", "primary", "secondary", "tertiary", "street", "street_limited", "primary_link", "track"], true, false
                ], 14, ["match", ["get", "class"],
                    ["motorway", "motorway_link", "trunk", "trunk_link", "primary", "primary_link", "secondary", "secondary_link", "tertiary", "tertiary_link", "street", "street_limited", "service", "track"], true, false
                ]
            ],
            ["match", ["get", "structure"],
                ["none", "ford"], true, false
            ],
            ["==", ["geometry-type"], "LineString"]
        ],
        "type": "line",
        "source": "composite",
        "id": "road-case-simple",
        "paint": {
            "line-width": ["interpolate", ["exponential", 1.99],
                ["zoom"], 5, ["match", ["get", "class"],
                    ["motorway", "trunk", "primary"], 1.2375, ["secondary", "tertiary"], 0.165, 0
                ], 13, ["match", ["get", "class"],
                    ["motorway", "trunk", "primary"], 7.28, ["secondary", "tertiary"], 3.3, ["motorway_link", "trunk_link", "primary_link", "street", "street_limited"], 4.32, 1.66
                ], 18, ["match", ["get", "class"],
                    ["motorway", "trunk", "primary"], 38.4, ["secondary", "tertiary"], 34.32, ["motorway_link", "trunk_link", "primary_link", "street", "street_limited"], 23.76, 15.2
                ], 22, ["match", ["get", "class"],
                    ["motorway", "trunk", "primary"], 387.2, ["secondary", "tertiary"], 314.6, ["motorway_link", "trunk_link", "primary_link", "street", "street_limited"], 217.8, 121
                ]
            ],
            "line-color": "hsl(29, 85%, 38%)",
            "line-blur": 1
        },
        "source-layer": "road"
    }, {
        "minzoom": 5,
        "layout": {
            "line-cap": ["step", ["zoom"], "butt", 14, "round"],
            "line-join": ["step", ["zoom"], "miter", 14, "round"]
        },
        "metadata": {
            "mapbox:featureComponent": "road-network",
            "mapbox:group": "Road network, surface"
        },
        "filter": ["all", ["step", ["zoom"],
                ["match", ["get", "class"],
                    ["motorway", "trunk"], true, false
                ], 6, ["match", ["get", "class"],
                    ["motorway", "trunk", "primary"], true, false
                ], 8, ["match", ["get", "class"],
                    ["motorway", "trunk", "primary", "secondary"], true, false
                ], 10, ["match", ["get", "class"],
                    ["motorway", "trunk", "primary", "secondary", "tertiary", "motorway_link", "trunk_link"], true, false
                ], 11, ["match", ["get", "class"],
                    ["motorway", "motorway_link", "trunk", "trunk_link", "primary", "secondary", "tertiary", "street"], true, false
                ], 12, ["match", ["get", "class"],
                    ["motorway", "motorway_link", "trunk", "trunk_link", "primary", "secondary", "tertiary", "street", "street_limited", "primary_link"], true, false
                ], 13, ["match", ["get", "class"],
                    ["motorway", "motorway_link", "trunk", "trunk_link", "primary", "secondary", "tertiary", "street", "street_limited", "primary_link", "track"], true, false
                ], 14, ["match", ["get", "class"],
                    ["motorway", "motorway_link", "trunk", "trunk_link", "primary", "primary_link", "secondary", "secondary_link", "tertiary", "tertiary_link", "street", "street_limited", "service", "track"], true, false
                ]
            ],
            ["match", ["get", "structure"],
                ["none", "ford"], true, false
            ],
            ["==", ["geometry-type"], "LineString"]
        ],
        "type": "line",
        "source": "composite",
        "id": "road-simple",
        "paint": {
            "line-width": ["interpolate", ["exponential", 1.99],
                ["zoom"], 5, ["match", ["get", "class"],
                    ["motorway", "trunk", "primary"], 0.8250000000000001, ["secondary", "tertiary"], 0.11000000000000001, 0
                ], 13, ["match", ["get", "class"],
                    ["motorway", "trunk", "primary"], 4.4, ["secondary", "tertiary"], 2.75, ["motorway_link", "trunk_link", "primary_link", "street", "street_limited"], 1.1, 0.55
                ], 18, ["match", ["get", "class"],
                    ["motorway", "trunk", "primary"], 35.2, ["secondary", "tertiary"], 28.6, ["motorway_link", "trunk_link", "primary_link", "street", "street_limited"], 19.8, 11
                ], 22, ["match", ["get", "class"],
                    ["motorway", "trunk", "primary"], 352, ["secondary", "tertiary"], 286, ["motorway_link", "trunk_link", "primary_link", "street", "street_limited"], 198.00000000000003, 110.00000000000001
                ]
            ],
            "line-color": "hsl(29, 82%, 60%)"
        },
        "source-layer": "road"
    }, {
        "id": "road-rail",
        "type": "line",
        "source": "composite",
        "source-layer": "road",
        "minzoom": 13,
        "filter": ["all", ["match", ["get", "class"],
                ["major_rail", "minor_rail"], true, false
            ],
            ["match", ["get", "structure"],
                ["none", "ford"], true, false
            ]
        ],
        "paint": {
            "line-color": ["interpolate", ["linear"],
                ["zoom"], 13, "hsla(215, 0%, 26%, 0.86)", 17, "hsla(215, 0%, 23%, 0.86)"
            ],
            "line-width": ["interpolate", ["exponential", 1.5],
                ["zoom"], 14, 0.5, 20, 1
            ]
        },
        "metadata": {
            "mapbox:featureComponent": "transit",
            "mapbox:group": "Transit, surface"
        }
    }, {
        "id": "road-rail-tracks",
        "type": "line",
        "source": "composite",
        "source-layer": "road",
        "minzoom": 13,
        "filter": ["all", ["match", ["get", "class"],
                ["major_rail", "minor_rail"], true, false
            ],
            ["match", ["get", "structure"],
                ["none", "ford"], true, false
            ]
        ],
        "paint": {
            "line-color": "hsla(215, 0%, 23%, 0.86)",
            "line-width": ["interpolate", ["exponential", 1.5],
                ["zoom"], 14, 4, 20, 8
            ],
            "line-dasharray": [0.1, 15],
            "line-opacity": ["interpolate", ["linear"],
                ["zoom"], 13.75, 0, 14, 1
            ]
        },
        "metadata": {
            "mapbox:featureComponent": "transit",
            "mapbox:group": "Transit, surface"
        }
    }, {
        "minzoom": 14,
        "layout": {
            "line-cap": "round"
        },
        "filter": ["all", ["==", ["get", "structure"], "bridge"],
            ["==", ["get", "class"], "path"],
            ["!=", ["get", "type"], "steps"],
            ["==", ["geometry-type"], "LineString"]
        ],
        "type": "line",
        "source": "composite",
        "id": "bridge-case-path",
        "paint": {
            "line-width": ["interpolate", ["exponential", 1.51],
                ["zoom"], 15, 5, 18, 25
            ],
            "line-color": "hsl(29, 77%, 14%)",
            "line-dasharray": [10, 0],
            "line-blur": 10
        },
        "source-layer": "road"
    }, {
        "minzoom": 14,
        "layout": {
            "line-cap": "round"
        },
        "metadata": {
            "mapbox:featureComponent": "walking-cycling",
            "mapbox:group": "Walking, cycling, etc., barriers-bridges"
        },
        "filter": ["all", ["==", ["get", "structure"], "bridge"],
            ["==", ["get", "class"], "path"],
            ["match", ["get", "type"],
                ["hiking", "mountain_bike", "trail"], true, false
            ],
            ["==", ["geometry-type"], "LineString"]
        ],
        "type": "line",
        "source": "composite",
        "id": "bridge-path-trail",
        "paint": {
            "line-width": ["interpolate", ["exponential", 1.5],
                ["zoom"], 15, 1, 18, 4
            ],
            "line-color": "hsla(215, 0%, 36%, 0.86)",
            "line-dasharray": [10, 0]
        },
        "source-layer": "road"
    }, {
        "minzoom": 14,
        "layout": {},
        "metadata": {
            "mapbox:featureComponent": "walking-cycling",
            "mapbox:group": "Walking, cycling, etc., barriers-bridges"
        },
        "filter": ["all", ["==", ["get", "structure"], "bridge"],
            ["==", ["get", "class"], "path"],
            ["match", ["get", "type"],
                ["cycleway", "piste"], true, false
            ],
            ["==", ["geometry-type"], "LineString"]
        ],
        "type": "line",
        "source": "composite",
        "id": "bridge-path-cycleway-piste",
        "paint": {
            "line-width": ["interpolate", ["exponential", 1.5],
                ["zoom"], 15, 1, 18, 4
            ],
            "line-color": "hsla(215, 0%, 36%, 0.86)",
            "line-dasharray": [10, 0]
        },
        "source-layer": "road"
    }, {
        "minzoom": 14,
        "layout": {
            "line-cap": "round"
        },
        "metadata": {
            "mapbox:featureComponent": "walking-cycling",
            "mapbox:group": "Walking, cycling, etc., barriers-bridges"
        },
        "filter": ["all", ["==", ["get", "structure"], "bridge"],
            ["==", ["get", "class"], "path"],
            ["!=", ["get", "type"], "steps"],
            ["==", ["geometry-type"], "LineString"]
        ],
        "type": "line",
        "source": "composite",
        "id": "bridge-path",
        "paint": {
            "line-width": ["interpolate", ["exponential", 1.51],
                ["zoom"], 15, 3, 18, 15
            ],
            "line-color": "hsl(29, 77%, 90%)",
            "line-dasharray": [10, 0]
        },
        "source-layer": "road"
    }, {
        "id": "bridge-steps",
        "type": "line",
        "source": "composite",
        "source-layer": "road",
        "minzoom": 14,
        "filter": ["all", ["==", ["get", "type"], "steps"],
            ["==", ["get", "structure"], "bridge"],
            ["==", ["geometry-type"], "LineString"]
        ],
        "paint": {
            "line-width": ["interpolate", ["exponential", 1.5],
                ["zoom"], 15, 1, 16, 1.6, 18, 6
            ],
            "line-color": "hsla(215, 0%, 36%, 0.86)",
            "line-dasharray": ["step", ["zoom"],
                ["literal", [1, 0]], 15, ["literal", [1.75, 1]], 16, ["literal", [1, 0.75]], 17, ["literal", [0.3, 0.3]]
            ]
        },
        "metadata": {
            "mapbox:featureComponent": "walking-cycling",
            "mapbox:group": "Walking, cycling, etc., barriers-bridges"
        }
    }, {
        "id": "bridge-pedestrian",
        "type": "line",
        "source": "composite",
        "source-layer": "road",
        "minzoom": 13,
        "filter": ["all", ["==", ["get", "structure"], "bridge"],
            ["==", ["get", "class"], "pedestrian"],
            ["==", ["geometry-type"], "LineString"]
        ],
        "paint": {
            "line-width": ["interpolate", ["exponential", 1.5],
                ["zoom"], 14, 0.5, 18, 12
            ],
            "line-color": "hsla(215, 0%, 36%, 0.86)",
            "line-dasharray": [10, 0]
        },
        "metadata": {
            "mapbox:featureComponent": "walking-cycling",
            "mapbox:group": "Walking, cycling, etc., barriers-bridges"
        }
    }, {
        "minzoom": 13,
        "layout": {
            "line-cap": "round"
        },
        "filter": ["all", ["==", ["get", "structure"], "bridge"],
            ["step", ["zoom"],
                ["match", ["get", "class"],
                    ["motorway", "motorway_link", "trunk", "trunk_link", "primary", "secondary", "tertiary", "street", "street_limited", "primary_link", "track"], true, false
                ], 14, ["match", ["get", "class"],
                    ["motorway", "motorway_link", "trunk", "trunk_link", "primary", "primary_link", "secondary", "secondary_link", "tertiary", "tertiary_link", "street", "street_limited", "service", "track"], true, false
                ]
            ],
            ["==", ["geometry-type"], "LineString"]
        ],
        "type": "line",
        "source": "composite",
        "id": "bridge-case-simple copy",
        "paint": {
            "line-width": ["interpolate", ["exponential", 1.5],
                ["zoom"], 13, ["match", ["get", "class"],
                    ["motorway", "trunk", "primary"], 4.800000000000001, ["secondary", "tertiary"], 3.2, ["motorway_link", "trunk_link", "street", "street_limited", "primary_link"], 2, 1
                ], 18, ["match", ["get", "class"],
                    ["motorway", "trunk", "primary"], 28.8, ["secondary", "tertiary"], 24, ["motorway_link", "trunk_link", "street", "street_limited", "primary_link"], 17.6, 12.8
                ], 22, ["match", ["get", "class"],
                    ["motorway", "trunk", "primary"], 288, ["secondary", "tertiary"], 240, ["motorway_link", "trunk_link", "primary_link", "street", "street_limited"], 176, 128
                ]
            ],
            "line-color": "hsla(0, 87%, 53%, 0.5)",
            "line-blur": 15
        },
        "source-layer": "road"
    }, {
        "minzoom": 13,
        "layout": {
            "line-cap": "round"
        },
        "metadata": {
            "mapbox:featureComponent": "road-network",
            "mapbox:group": "Road network, bridges"
        },
        "filter": ["all", ["==", ["get", "structure"], "bridge"],
            ["step", ["zoom"],
                ["match", ["get", "class"],
                    ["motorway", "motorway_link", "trunk", "trunk_link", "primary", "secondary", "tertiary", "street", "street_limited", "primary_link", "track"], true, false
                ], 14, ["match", ["get", "class"],
                    ["motorway", "motorway_link", "trunk", "trunk_link", "primary", "primary_link", "secondary", "secondary_link", "tertiary", "tertiary_link", "street", "street_limited", "service", "track"], true, false
                ]
            ],
            ["==", ["geometry-type"], "LineString"]
        ],
        "type": "line",
        "source": "composite",
        "id": "bridge-case-simple",
        "paint": {
            "line-width": ["interpolate", ["exponential", 1.5],
                ["zoom"], 13, ["match", ["get", "class"],
                    ["motorway", "trunk", "primary"], 4.800000000000001, ["secondary", "tertiary"], 3.2, ["motorway_link", "trunk_link", "street", "street_limited", "primary_link"], 2, 1
                ], 18, ["match", ["get", "class"],
                    ["motorway", "trunk", "primary"], 28.8, ["secondary", "tertiary"], 24, ["motorway_link", "trunk_link", "street", "street_limited", "primary_link"], 17.6, 12.8
                ], 22, ["match", ["get", "class"],
                    ["motorway", "trunk", "primary"], 288, ["secondary", "tertiary"], 240, ["motorway_link", "trunk_link", "primary_link", "street", "street_limited"], 176, 128
                ]
            ],
            "line-color": "hsla(215, 10%, 37%, 0.86)"
        },
        "source-layer": "road"
    }, {
        "minzoom": 13,
        "layout": {
            "line-cap": ["step", ["zoom"], "butt", 14, "round"]
        },
        "metadata": {
            "mapbox:featureComponent": "road-network",
            "mapbox:group": "Road network, bridges"
        },
        "filter": ["all", ["==", ["get", "structure"], "bridge"],
            ["step", ["zoom"],
                ["match", ["get", "class"],
                    ["motorway", "trunk"], true, false
                ], 13, ["match", ["get", "class"],
                    ["motorway", "motorway_link", "trunk", "trunk_link", "primary", "secondary", "tertiary", "street", "street_limited", "primary_link", "track"], true, false
                ], 14, ["match", ["get", "class"],
                    ["motorway", "motorway_link", "trunk", "trunk_link", "primary", "primary_link", "secondary", "secondary_link", "tertiary", "tertiary_link", "street", "street_limited", "service", "track"], true, false
                ]
            ],
            ["==", ["geometry-type"], "LineString"]
        ],
        "type": "line",
        "source": "composite",
        "id": "bridge-simple",
        "paint": {
            "line-width": ["interpolate", ["exponential", 1.5],
                ["zoom"], 13, ["match", ["get", "class"],
                    ["motorway", "trunk", "primary"], 3.2, ["secondary", "tertiary"], 2, ["motorway_link", "trunk_link", "street", "street_limited", "primary_link"], 0.8, 0.5
                ], 18, ["match", ["get", "class"],
                    ["motorway", "trunk", "primary"], 25.6, ["secondary", "tertiary"], 20.8, ["motorway_link", "trunk_link", "street", "street_limited", "primary_link"], 14.4, 9.600000000000001
                ], 22, ["match", ["get", "class"],
                    ["motorway", "trunk", "primary"], 256, ["secondary", "tertiary"], 208, ["motorway_link", "trunk_link", "primary_link", "street", "street_limited"], 144, 96
                ]
            ],
            "line-color": "hsl(215, 11%, 67%)"
        },
        "source-layer": "road"
    }, {
        "id": "bridge-rail",
        "type": "line",
        "source": "composite",
        "source-layer": "road",
        "minzoom": 13,
        "filter": ["all", ["==", ["get", "structure"], "bridge"],
            ["match", ["get", "class"],
                ["major_rail", "minor_rail"], true, false
            ]
        ],
        "paint": {
            "line-color": "hsla(215, 0%, 23%, 0.86)",
            "line-width": ["interpolate", ["exponential", 1.5],
                ["zoom"], 14, 0.5, 20, 1
            ]
        },
        "metadata": {
            "mapbox:featureComponent": "transit",
            "mapbox:group": "Transit, bridges"
        }
    }, {
        "id": "bridge-rail-tracks",
        "type": "line",
        "source": "composite",
        "source-layer": "road",
        "minzoom": 13,
        "filter": ["all", ["==", ["get", "structure"], "bridge"],
            ["match", ["get", "class"],
                ["major_rail", "minor_rail"], true, false
            ]
        ],
        "paint": {
            "line-color": "hsla(215, 0%, 23%, 0.86)",
            "line-width": ["interpolate", ["exponential", 1.5],
                ["zoom"], 14, 4, 20, 8
            ],
            "line-dasharray": [0.1, 15],
            "line-opacity": ["interpolate", ["linear"],
                ["zoom"], 13.75, 0, 14, 1
            ]
        },
        "metadata": {
            "mapbox:featureComponent": "transit",
            "mapbox:group": "Transit, bridges"
        }
    }, {
        "minzoom": 15,
        "layout": {},
        "metadata": {
            "mapbox:featureComponent": "buildings",
            "mapbox:group": "Buildings, extruded"
        },
        "filter": ["==", ["get", "extrude"], "true"],
        "type": "fill-extrusion",
        "source": "composite",
        "id": "building-extrusion",
        "paint": {
            "fill-extrusion-color": "hsla(215, 1%, 23%, 0.86)",
            "fill-extrusion-height": ["get", "height"],
            "fill-extrusion-base": ["get", "min_height"],
            "fill-extrusion-opacity": ["interpolate", ["linear"],
                ["zoom"], 15, 0, 16, 0.8
            ]
        },
        "source-layer": "building"
    }, {
        "id": "admin-2-boundary-bg",
        "type": "line",
        "source": "composite",
        "source-layer": "admin",
        "minzoom": 7,
        "filter": ["all", ["==", ["get", "admin_level"], 2],
            ["==", ["get", "maritime"], "false"],
            ["match", ["get", "worldview"],
                ["all", "CN"], true, false
            ]
        ],
        "paint": {
            "line-color": "hsla(215, 0%, 23%, 0.86)",
            "line-width": ["interpolate", ["linear"],
                ["zoom"], 3, 3.9000000000000004, 12, 7.800000000000001
            ],
            "line-opacity": ["interpolate", ["linear"],
                ["zoom"], 7, 0, 8, 0.5
            ],
            "line-dasharray": [1, 0],
            "line-blur": ["interpolate", ["linear"],
                ["zoom"], 3, 0, 12, 3.9000000000000004
            ]
        },
        "metadata": {
            "mapbox:featureComponent": "admin-boundaries",
            "mapbox:group": "Administrative boundaries, admin"
        }
    }, {
        "id": "admin-1-boundary-bg",
        "type": "line",
        "source": "composite",
        "source-layer": "admin",
        "minzoom": 7,
        "filter": ["all", ["==", ["get", "admin_level"], 1],
            ["==", ["get", "maritime"], "false"],
            ["match", ["get", "worldview"],
                ["all", "CN"], true, false
            ]
        ],
        "paint": {
            "line-color": "hsla(215, 0%, 23%, 0.86)",
            "line-width": ["interpolate", ["linear"],
                ["zoom"], 3, 3.5999999999999996, 12, 7.199999999999999
            ],
            "line-opacity": ["interpolate", ["linear"],
                ["zoom"], 7, 0, 8, 0.5
            ],
            "line-dasharray": [1, 0],
            "line-blur": ["interpolate", ["linear"],
                ["zoom"], 3, 0, 12, 3.5999999999999996
            ]
        },
        "metadata": {
            "mapbox:featureComponent": "admin-boundaries",
            "mapbox:group": "Administrative boundaries, admin"
        }
    }, {
        "id": "admin-0-boundary-bg",
        "type": "line",
        "source": "composite",
        "source-layer": "admin",
        "minzoom": 1,
        "filter": ["all", ["==", ["get", "admin_level"], 0],
            ["==", ["get", "maritime"], "false"],
            ["match", ["get", "worldview"],
                ["all", "CN"], true, false
            ]
        ],
        "paint": {
            "line-width": ["interpolate", ["linear"],
                ["zoom"], 3, 5.2, 12, 10.4
            ],
            "line-color": "hsla(215, 0%, 23%, 0.86)",
            "line-opacity": ["interpolate", ["linear"],
                ["zoom"], 3, 0, 4, 0.5
            ],
            "line-blur": ["interpolate", ["linear"],
                ["zoom"], 3, 0, 12, 2.6
            ]
        },
        "metadata": {
            "mapbox:featureComponent": "admin-boundaries",
            "mapbox:group": "Administrative boundaries, admin"
        }
    }, {
        "minzoom": 5,
        "layout": {},
        "metadata": {
            "mapbox:featureComponent": "admin-boundaries",
            "mapbox:group": "Administrative boundaries, admin"
        },
        "filter": ["all", ["==", ["get", "admin_level"], 2],
            ["==", ["get", "maritime"], "false"],
            ["match", ["get", "worldview"],
                ["all", "CN"], true, false
            ]
        ],
        "type": "line",
        "source": "composite",
        "id": "admin-2-boundary",
        "paint": {
            "line-dasharray": ["step", ["zoom"],
                ["literal", [3, 2, 5]], 7, ["literal", [2, 1.5]]
            ],
            "line-width": ["interpolate", ["linear"],
                ["zoom"], 3, 0.39, 12, 1.3
            ],
            "line-opacity": ["interpolate", ["linear"],
                ["zoom"], 5, 0, 6, 1
            ],
            "line-color": "hsla(215, 0%, 51%, 0.86)"
        },
        "source-layer": "admin"
    }, {
        "minzoom": 2,
        "layout": {},
        "metadata": {
            "mapbox:featureComponent": "admin-boundaries",
            "mapbox:group": "Administrative boundaries, admin"
        },
        "filter": ["all", ["==", ["get", "admin_level"], 1],
            ["==", ["get", "maritime"], "false"],
            ["match", ["get", "worldview"],
                ["all", "CN"], true, false
            ]
        ],
        "type": "line",
        "source": "composite",
        "id": "admin-1-boundary",
        "paint": {
            "line-dasharray": ["step", ["zoom"],
                ["literal", [2, 0]], 7, ["literal", [2, 2, 6, 2]]
            ],
            "line-width": ["interpolate", ["linear"],
                ["zoom"], 3, 0.36, 12, 1.7999999999999998
            ],
            "line-opacity": ["interpolate", ["linear"],
                ["zoom"], 2, 0, 3, 1
            ],
            "line-color": "hsla(215, 0%, 51%, 0.86)"
        },
        "source-layer": "admin"
    }, {
        "minzoom": 1,
        "layout": {},
        "metadata": {
            "mapbox:featureComponent": "admin-boundaries",
            "mapbox:group": "Administrative boundaries, admin"
        },
        "filter": ["all", ["==", ["get", "admin_level"], 0],
            ["==", ["get", "disputed"], "false"],
            ["==", ["get", "maritime"], "false"],
            ["match", ["get", "worldview"],
                ["all", "CN"], true, false
            ]
        ],
        "type": "line",
        "source": "composite",
        "id": "admin-0-boundary",
        "paint": {
            "line-color": "hsla(215, 0%, 52%, 0.86)",
            "line-width": ["interpolate", ["linear"],
                ["zoom"], 3, 0.65, 12, 2.6
            ],
            "line-dasharray": [10, 0]
        },
        "source-layer": "admin"
    }, {
        "id": "admin-0-boundary-disputed",
        "type": "line",
        "source": "composite",
        "source-layer": "admin",
        "minzoom": 1,
        "filter": ["all", ["==", ["get", "disputed"], "true"],
            ["==", ["get", "admin_level"], 0],
            ["==", ["get", "maritime"], "false"],
            ["match", ["get", "worldview"],
                ["all", "CN"], true, false
            ]
        ],
        "paint": {
            "line-color": "hsla(215, 0%, 52%, 0.86)",
            "line-width": ["interpolate", ["linear"],
                ["zoom"], 3, 0.65, 12, 2.6
            ],
            "line-dasharray": ["step", ["zoom"],
                ["literal", [3, 2, 5]], 7, ["literal", [2, 1.5]]
            ]
        },
        "metadata": {
            "mapbox:featureComponent": "admin-boundaries",
            "mapbox:group": "Administrative boundaries, admin"
        }
    }, {
        "minzoom": 12,
        "layout": {
            "text-size": ["interpolate", ["linear"],
                ["zoom"], 10, ["match", ["get", "class"],
                    ["motorway", "trunk", "primary", "secondary", "tertiary"], 12, 10.799999999999999
                ], 18, ["match", ["get", "class"],
                    ["motorway", "trunk", "primary", "secondary", "tertiary"], 19.2, 16.8
                ]
            ],
            "text-max-angle": 30,
            "symbol-spacing": 600,
            "text-font": ["step", ["zoom"],
                ["literal", ["DIN Pro Italic", "Arial Unicode MS Regular"]], 22, ["literal", ["DIN Pro Italic", "Arial Unicode MS Regular"]]
            ],
            "symbol-placement": "line",
            "text-padding": 30,
            "text-rotation-alignment": "map",
            "text-pitch-alignment": "viewport",
            "text-field": ["coalesce", ["get", "name_zh-Hans"],
                ["get", "name"]
            ],
            "text-letter-spacing": 0.01
        },
        "metadata": {
            "mapbox:featureComponent": "road-network",
            "mapbox:group": "Road network, road-labels"
        },
        "filter": ["all", ["has", "name"],
            ["match", ["get", "class"],
                ["motorway", "trunk", "primary", "secondary", "tertiary", "street", "street_limited"], true, false
            ],
            ["step", ["pitch"], true, 50, ["<", ["distance-from-center"], 2], 60, ["<", ["distance-from-center"], 2.5], 70, ["<", ["distance-from-center"], 3]]
        ],
        "type": "symbol",
        "source": "composite",
        "id": "road-label-simple",
        "paint": {
            "text-color": "hsla(29, 74%, 100%, 0.86)",
            "text-halo-color": "hsla(29, 68%, 27%, 0.86)",
            "text-halo-width": 0.5
        },
        "source-layer": "road"
    }, {
        "minzoom": 12,
        "layout": {
            "text-size": ["interpolate", ["linear"],
                ["zoom"], 10, ["match", ["get", "class"], "pedestrian", 10.799999999999999, 7.8], 18, ["match", ["get", "class"], "pedestrian", 16.8, 15.6]
            ],
            "text-max-angle": 30,
            "text-font": ["DIN Pro Regular", "Arial Unicode MS Regular"],
            "symbol-placement": "line",
            "text-padding": 1,
            "text-rotation-alignment": "map",
            "text-pitch-alignment": "viewport",
            "text-field": ["coalesce", ["get", "name_zh-Hans"],
                ["get", "name"]
            ],
            "text-letter-spacing": 0.01
        },
        "metadata": {
            "mapbox:featureComponent": "walking-cycling",
            "mapbox:group": "Walking, cycling, etc., walking-cycling-labels"
        },
        "filter": ["all", ["case", ["has", "layer"],
                [">=", ["get", "layer"], 0], true
            ],
            ["step", ["zoom"],
                ["match", ["get", "class"],
                    ["pedestrian"], true, false
                ], 15, ["match", ["get", "class"],
                    ["path", "pedestrian"], true, false
                ]
            ],
            ["step", ["pitch"], true, 50, ["<", ["distance-from-center"], 1], 60, ["<", ["distance-from-center"], 1.5], 70, ["<", ["distance-from-center"], 2]]
        ],
        "type": "symbol",
        "source": "composite",
        "id": "path-pedestrian-label",
        "paint": {
            "text-color": "hsla(215, 0%, 79%, 0.86)",
            "text-halo-color": "hsla(215, 0%, 17%, 0.86)",
            "text-halo-width": 1,
            "text-halo-blur": 1
        },
        "source-layer": "road"
    }, {
        "minzoom": 13,
        "layout": {
            "text-font": ["DIN Pro Italic", "Arial Unicode MS Regular"],
            "text-max-angle": 30,
            "symbol-spacing": ["interpolate", ["linear", 1],
                ["zoom"], 15, 250, 17, 400
            ],
            "text-size": ["interpolate", ["linear"],
                ["zoom"], 13, 12, 18, 18
            ],
            "symbol-placement": "line",
            "text-pitch-alignment": "viewport",
            "text-field": ["coalesce", ["get", "name_zh-Hans"],
                ["get", "name"]
            ]
        },
        "metadata": {
            "mapbox:featureComponent": "natural-features",
            "mapbox:group": "Natural features, natural-labels"
        },
        "filter": ["all", ["match", ["get", "class"],
                ["canal", "river", "stream", "disputed_canal", "disputed_river", "disputed_stream"],
                ["match", ["get", "worldview"],
                    ["all", "CN"], true, false
                ], false
            ],
            ["step", ["pitch"], true, 50, ["<", ["distance-from-center"], 1], 60, ["<", ["distance-from-center"], 1.5], 70, ["<", ["distance-from-center"], 2]],
            ["==", ["geometry-type"], "LineString"]
        ],
        "type": "symbol",
        "source": "composite",
        "id": "waterway-label",
        "paint": {
            "text-color": "hsla(215, 0%, 48%, 0.86)",
            "text-halo-color": "hsla(215, 0%, 17%, 0.5)"
        },
        "source-layer": "natural_label"
    }, {
        "minzoom": 4,
        "layout": {
            "text-size": ["step", ["zoom"],
                ["step", ["get", "sizerank"], 16.2, 5, 10.8], 17, ["step", ["get", "sizerank"], 16.2, 13, 10.8]
            ],
            "text-max-angle": 30,
            "text-field": ["coalesce", ["get", "name_zh-Hans"],
                ["get", "name"]
            ],
            "text-font": ["DIN Pro Italic", "Arial Unicode MS Regular"],
            "symbol-placement": "line-center",
            "text-pitch-alignment": "viewport"
        },
        "metadata": {
            "mapbox:featureComponent": "natural-features",
            "mapbox:group": "Natural features, natural-labels"
        },
        "filter": ["all", ["match", ["get", "class"],
                ["glacier", "landform", "disputed_glacier", "disputed_landform"],
                ["match", ["get", "worldview"],
                    ["all", "CN"], true, false
                ], false
            ],
            ["<=", ["get", "filterrank"], 1],
            ["step", ["pitch"], true, 50, ["<", ["distance-from-center"], 1], 60, ["<", ["distance-from-center"], 1.5], 70, ["<", ["distance-from-center"], 2]],
            ["==", ["geometry-type"], "LineString"]
        ],
        "type": "symbol",
        "source": "composite",
        "id": "natural-line-label",
        "paint": {
            "text-halo-width": 0.5,
            "text-halo-color": "hsla(215, 0%, 17%, 0.86)",
            "text-halo-blur": 0.5,
            "text-color": "hsla(215, 0%, 66%, 0.86)"
        },
        "source-layer": "natural_label"
    }, {
        "minzoom": 4,
        "layout": {
            "text-size": ["step", ["zoom"],
                ["step", ["get", "sizerank"], 16.2, 5, 10.8], 17, ["step", ["get", "sizerank"], 16.2, 13, 10.8]
            ],
            "icon-image": "",
            "text-font": ["DIN Pro Italic", "Arial Unicode MS Regular"],
            "text-offset": ["literal", [0, 0]],
            "text-field": ["coalesce", ["get", "name_zh-Hans"],
                ["get", "name"]
            ]
        },
        "metadata": {
            "mapbox:featureComponent": "natural-features",
            "mapbox:group": "Natural features, natural-labels"
        },
        "filter": ["all", ["match", ["get", "class"],
                ["dock", "glacier", "landform", "water_feature", "wetland", "disputed_dock", "disputed_glacier", "disputed_landform", "disputed_water_feature", "disputed_wetland"],
                ["match", ["get", "worldview"],
                    ["all", "CN"], true, false
                ], false
            ],
            ["<=", ["get", "filterrank"], 1],
            ["step", ["pitch"], true, 50, ["<", ["distance-from-center"], 1], 60, ["<", ["distance-from-center"], 1.5], 70, ["<", ["distance-from-center"], 2]],
            ["==", ["geometry-type"], "Point"]
        ],
        "type": "symbol",
        "source": "composite",
        "id": "natural-point-label",
        "paint": {
            "icon-opacity": ["step", ["zoom"],
                ["step", ["get", "sizerank"], 0, 5, 1], 17, ["step", ["get", "sizerank"], 0, 13, 1]
            ],
            "text-halo-color": "hsla(215, 0%, 17%, 0.86)",
            "text-halo-width": 0.5,
            "text-halo-blur": 0.5,
            "text-color": "hsla(215, 0%, 66%, 0.86)"
        },
        "source-layer": "natural_label"
    }, {
        "minzoom": 1,
        "layout": {
            "text-size": ["interpolate", ["linear"],
                ["zoom"], 0, ["*", ["-", 16, ["sqrt", ["get", "sizerank"]]], 1], 22, ["*", ["-", 22, ["sqrt", ["get", "sizerank"]]], 1]
            ],
            "text-max-angle": 30,
            "text-letter-spacing": ["match", ["get", "class"], "ocean", 0.25, ["sea", "bay"], 0.15, 0],
            "text-font": ["DIN Pro Italic", "Arial Unicode MS Regular"],
            "symbol-placement": "line-center",
            "text-pitch-alignment": "viewport",
            "text-field": ["coalesce", ["get", "name_zh-Hans"],
                ["get", "name"]
            ]
        },
        "metadata": {
            "mapbox:featureComponent": "natural-features",
            "mapbox:group": "Natural features, natural-labels"
        },
        "filter": ["all", ["match", ["get", "class"],
                ["bay", "ocean", "reservoir", "sea", "water", "disputed_bay", "disputed_ocean", "disputed_reservoir", "disputed_sea", "disputed_water"],
                ["match", ["get", "worldview"],
                    ["all", "CN"], true, false
                ], false
            ],
            ["step", ["pitch"], true, 50, ["<", ["distance-from-center"], 1], 60, ["<", ["distance-from-center"], 1.5], 70, ["<", ["distance-from-center"], 2]],
            ["==", ["geometry-type"], "LineString"]
        ],
        "type": "symbol",
        "source": "composite",
        "id": "water-line-label",
        "paint": {
            "text-color": "hsla(215, 0%, 48%, 0.86)",
            "text-halo-color": "hsla(215, 0%, 17%, 0.5)"
        },
        "source-layer": "natural_label"
    }, {
        "minzoom": 1,
        "layout": {
            "text-line-height": 1.3,
            "text-size": ["interpolate", ["linear"],
                ["zoom"], 0, ["*", ["-", 16, ["sqrt", ["get", "sizerank"]]], 1], 22, ["*", ["-", 22, ["sqrt", ["get", "sizerank"]]], 1]
            ],
            "text-font": ["DIN Pro Italic", "Arial Unicode MS Regular"],
            "text-field": ["coalesce", ["get", "name_zh-Hans"],
                ["get", "name"]
            ],
            "text-letter-spacing": ["match", ["get", "class"], "ocean", 0.25, ["bay", "sea"], 0.15, 0.01],
            "text-max-width": ["match", ["get", "class"], "ocean", 4, "sea", 5, ["bay", "water"], 7, 10]
        },
        "metadata": {
            "mapbox:featureComponent": "natural-features",
            "mapbox:group": "Natural features, natural-labels"
        },
        "filter": ["all", ["match", ["get", "class"],
                ["bay", "ocean", "reservoir", "sea", "water", "disputed_bay", "disputed_ocean", "disputed_reservoir", "disputed_sea", "disputed_water"],
                ["match", ["get", "worldview"],
                    ["all", "CN"], true, false
                ], false
            ],
            ["step", ["pitch"], true, 50, ["<", ["distance-from-center"], 1], 60, ["<", ["distance-from-center"], 1.5], 70, ["<", ["distance-from-center"], 2]],
            ["==", ["geometry-type"], "Point"]
        ],
        "type": "symbol",
        "source": "composite",
        "id": "water-point-label",
        "paint": {
            "text-color": "hsla(215, 0%, 48%, 0.86)",
            "text-halo-color": "hsla(215, 0%, 17%, 0.5)"
        },
        "source-layer": "natural_label"
    }, {
        "minzoom": 6,
        "layout": {
            "text-size": ["step", ["zoom"],
                ["step", ["get", "sizerank"], 16.2, 5, 10.8], 17, ["step", ["get", "sizerank"], 16.2, 13, 10.8]
            ],
            "icon-image": ["case", ["has", "maki_beta"],
                ["coalesce", ["image", ["get", "maki_beta"]],
                    ["image", ["get", "maki"]]
                ],
                ["image", ["get", "maki"]]
            ],
            "text-font": ["DIN Pro Italic", "Arial Unicode MS Regular"],
            "text-offset": ["step", ["zoom"],
                ["step", ["get", "sizerank"],
                    ["literal", [0, 0]], 5, ["literal", [0, 1.5]]
                ], 17, ["step", ["get", "sizerank"],
                    ["literal", [0, 0]], 13, ["literal", [0, 1.5]]
                ]
            ],
            "text-anchor": ["step", ["zoom"],
                ["step", ["get", "sizerank"], "center", 5, "top"], 17, ["step", ["get", "sizerank"], "center", 13, "top"]
            ],
            "text-field": ["coalesce", ["get", "name_zh-Hans"],
                ["get", "name"]
            ]
        },
        "metadata": {
            "mapbox:featureComponent": "point-of-interest-labels",
            "mapbox:group": "Point of interest labels, poi-labels"
        },
        "filter": ["all", ["<=", ["get", "filterrank"],
                ["+", ["step", ["zoom"], 0, 16, 1, 17, 2], 4]
            ],
            ["step", ["pitch"], true, 50, ["<", ["distance-from-center"], 2], 60, ["<", ["distance-from-center"], 2.5], 70, ["<", ["distance-from-center"], 3]]
        ],
        "type": "symbol",
        "source": "composite",
        "id": "poi-label",
        "paint": {
            "icon-opacity": ["step", ["zoom"],
                ["step", ["get", "sizerank"], 0, 5, 1], 17, ["step", ["get", "sizerank"], 0, 13, 1]
            ],
            "text-halo-color": "hsla(29, 10%, 29%, 0.4)",
            "text-halo-width": 2,
            "text-halo-blur": 0.5,
            "text-color": "hsla(21, 33%, 89%, 0.86)"
        },
        "source-layer": "poi_label"
    }, {
        "minzoom": 12,
        "layout": {
            "text-size": 10.8,
            "icon-image": "",
            "text-font": ["DIN Pro Italic", "Arial Unicode MS Regular"],
            "text-justify": ["match", ["get", "stop_type"], "entrance", "left", "center"],
            "text-offset": [0, 0],
            "text-anchor": ["match", ["get", "stop_type"], "entrance", "left", "top"],
            "text-field": ["step", ["zoom"], "", 13, ["match", ["get", "mode"],
                ["rail", "metro_rail"],
                ["coalesce", ["get", "name_zh-Hans"],
                    ["get", "name"]
                ], ""
            ], 14, ["match", ["get", "mode"],
                ["bus", "bicycle"], "", ["coalesce", ["get", "name_zh-Hans"],
                    ["get", "name"]
                ]
            ], 18, ["coalesce", ["get", "name_zh-Hans"],
                ["get", "name"]
            ]],
            "text-letter-spacing": 0.01,
            "text-max-width": ["match", ["get", "stop_type"], "entrance", 15, 9]
        },
        "metadata": {
            "mapbox:featureComponent": "transit",
            "mapbox:group": "Transit, transit-labels"
        },
        "filter": ["all", ["step", ["zoom"],
                ["all", ["<=", ["get", "filterrank"], 4],
                    ["match", ["get", "mode"], "rail", true, "metro_rail", true, false],
                    ["!=", ["get", "stop_type"], "entrance"]
                ], 14, ["all", ["match", ["get", "mode"], "rail", true, "metro_rail", true, false],
                    ["!=", ["get", "stop_type"], "entrance"]
                ], 15, ["all", ["match", ["get", "mode"], "rail", true, "metro_rail", true, "light_rail", true, false],
                    ["!=", ["get", "stop_type"], "entrance"]
                ], 16, ["all", ["match", ["get", "mode"], "ferry", false, "bus", false, true],
                    ["!=", ["get", "stop_type"], "entrance"]
                ], 17, ["all", ["match", ["get", "mode"], "ferry", false, true],
                    ["!=", ["get", "stop_type"], "entrance"]
                ], 19, ["match", ["get", "mode"], "ferry", false, true]
            ],
            ["step", ["pitch"], true, 50, ["<", ["distance-from-center"], 1], 60, ["<", ["distance-from-center"], 1.5], 70, ["<", ["distance-from-center"], 2]]
        ],
        "type": "symbol",
        "source": "composite",
        "id": "transit-label",
        "paint": {
            "text-halo-color": "hsla(215, 4%, 58%, 0.86)",
            "text-color": "hsla(215, 26%, 91%, 0.86)",
            "text-halo-blur": 0.5,
            "text-halo-width": 0.5
        },
        "source-layer": "transit_stop_label"
    }, {
        "minzoom": 8,
        "layout": {
            "text-line-height": 1.1,
            "text-size": ["step", ["get", "sizerank"], 16.2, 9, 10.8],
            "icon-image": "",
            "text-font": ["DIN Pro Italic", "Arial Unicode MS Regular"],
            "text-offset": [0, 0],
            "text-rotation-alignment": "viewport",
            "text-anchor": "top",
            "text-field": ["step", ["get", "sizerank"],
                ["case", ["has", "ref"],
                    ["concat", ["get", "ref"], " -\n", ["coalesce", ["get", "name_zh-Hans"],
                        ["get", "name"]
                    ]],
                    ["coalesce", ["get", "name_zh-Hans"],
                        ["get", "name"]
                    ]
                ], 15, ["get", "ref"]
            ],
            "text-letter-spacing": 0.01,
            "text-max-width": 9
        },
        "metadata": {
            "mapbox:featureComponent": "transit",
            "mapbox:group": "Transit, transit-labels"
        },
        "filter": ["all", ["match", ["get", "class"],
                ["military", "civil", "disputed_military", "disputed_civil"],
                ["match", ["get", "worldview"],
                    ["all", "CN"], true, false
                ], false
            ],
            ["step", ["pitch"], true, 50, ["<", ["distance-from-center"], 3], 60, ["<", ["distance-from-center"], 4], 70, ["<", ["distance-from-center"], 5]]
        ],
        "type": "symbol",
        "source": "composite",
        "id": "airport-label",
        "paint": {
            "text-color": "hsla(215, 30%, 90%, 0.86)",
            "text-halo-color": "hsla(215, 13%, 41%, 0.86)",
            "text-halo-width": 1
        },
        "source-layer": "airport_label"
    }, {
        "minzoom": 10,
        "layout": {
            "text-field": ["coalesce", ["get", "name_zh-Hans"],
                ["get", "name"]
            ],
            "text-transform": "uppercase",
            "text-font": ["DIN Pro Regular", "Arial Unicode MS Regular"],
            "text-letter-spacing": ["match", ["get", "type"], "suburb", 0.15, 0.05],
            "text-max-width": 7,
            "text-padding": 3,
            "text-size": ["interpolate", ["cubic-bezier", 0.5, 0, 1, 1],
                ["zoom"], 11, ["match", ["get", "type"], "suburb", 14.3, 13.65], 15, ["match", ["get", "type"], "suburb", 19.5, 18.2]
            ]
        },
        "metadata": {
            "mapbox:featureComponent": "place-labels",
            "mapbox:group": "Place labels, place-labels"
        },
        "maxzoom": 15,
        "filter": ["all", ["match", ["get", "class"],
                ["settlement_subdivision", "disputed_settlement_subdivision"],
                ["match", ["get", "worldview"],
                    ["all", "CN"], true, false
                ], false
            ],
            ["<=", ["get", "filterrank"], 3],
            ["step", ["pitch"], true, 50, ["<", ["distance-from-center"], 3], 60, ["<", ["distance-from-center"], 4], 70, ["<", ["distance-from-center"], 5]]
        ],
        "type": "symbol",
        "source": "composite",
        "id": "settlement-subdivision-label",
        "paint": {
            "text-halo-color": "hsla(215, 0%, 17%, 0.86)",
            "text-halo-width": 1,
            "text-color": "hsla(215, 0%, 100%, 0.86)",
            "text-halo-blur": 0.5
        },
        "source-layer": "place_label"
    }, {
        "minzoom": 2,
        "layout": {
            "text-line-height": 1.1,
            "text-size": ["interpolate", ["cubic-bezier", 0.2, 0, 0.9, 1],
                ["zoom"], 3, ["step", ["get", "symbolrank"], 14.3, 9, 13], 6, ["step", ["get", "symbolrank"], 18.2, 9, 15.600000000000001, 12, 13], 8, ["step", ["get", "symbolrank"], 20.8, 9, 18.2, 12, 15.600000000000001, 15, 13], 13, ["step", ["get", "symbolrank"], 28.6, 9, 26, 12, 20.8, 15, 18.2]
            ],
            "text-radial-offset": ["step", ["zoom"],
                ["match", ["get", "capital"], 2, 0.6, 0.55], 8, 0
            ],
            "symbol-sort-key": ["get", "symbolrank"],
            "icon-image": "",
            "text-font": ["DIN Pro Regular", "Arial Unicode MS Regular"],
            "text-anchor": ["step", ["zoom"], "center", 8, "center"],
            "text-field": ["coalesce", ["get", "name_zh-Hans"],
                ["get", "name"]
            ],
            "text-max-width": 7
        },
        "metadata": {
            "mapbox:featureComponent": "place-labels",
            "mapbox:group": "Place labels, place-labels"
        },
        "maxzoom": 13,
        "filter": ["all", ["<=", ["get", "filterrank"], 3],
            ["match", ["get", "class"],
                ["settlement", "disputed_settlement"],
                ["match", ["get", "worldview"],
                    ["all", "CN"], true, false
                ], false
            ],
            ["step", ["zoom"],
                [">", ["get", "symbolrank"], 6], 4, [">=", ["get", "symbolrank"], 7], 6, [">=", ["get", "symbolrank"], 8], 7, [">=", ["get", "symbolrank"], 10], 10, [">=", ["get", "symbolrank"], 11], 11, [">=", ["get", "symbolrank"], 13], 12, [">=", ["get", "symbolrank"], 15]
            ],
            ["step", ["pitch"], true, 50, ["<", ["distance-from-center"], 3], 60, ["<", ["distance-from-center"], 4], 70, ["<", ["distance-from-center"], 5]]
        ],
        "type": "symbol",
        "source": "composite",
        "id": "settlement-minor-label",
        "paint": {
            "text-color": ["step", ["get", "symbolrank"], "hsla(215, 0%, 79%, 0.86)", 11, "hsla(215, 0%, 66%, 0.86)", 16, "hsla(215, 0%, 60%, 0.86)"],
            "text-halo-color": "hsla(215, 0%, 17%, 0.86)",
            "text-halo-width": 1,
            "icon-opacity": ["step", ["zoom"], 1, 8, 0],
            "text-halo-blur": 1
        },
        "source-layer": "place_label"
    }, {
        "minzoom": 2,
        "layout": {
            "text-line-height": 1.1,
            "text-size": ["interpolate", ["cubic-bezier", 0.2, 0, 0.9, 1],
                ["zoom"], 3, ["step", ["get", "symbolrank"], 19.5, 6, 16.5], 6, ["step", ["get", "symbolrank"], 27, 6, 24, 7, 21], 8, ["step", ["get", "symbolrank"], 30, 9, 24, 10, 21], 15, ["step", ["get", "symbolrank"], 36, 9, 30, 12, 24, 15, 21]
            ],
            "text-radial-offset": ["step", ["zoom"],
                ["match", ["get", "capital"], 2, 0.6, 0.55], 8, 0
            ],
            "symbol-sort-key": ["get", "symbolrank"],
            "icon-image": "",
            "text-font": ["DIN Pro Medium", "Arial Unicode MS Regular"],
            "text-anchor": ["step", ["zoom"], "center", 8, "center"],
            "text-field": ["coalesce", ["get", "name_zh-Hans"],
                ["get", "name"]
            ],
            "text-max-width": 7
        },
        "metadata": {
            "mapbox:featureComponent": "place-labels",
            "mapbox:group": "Place labels, place-labels"
        },
        "maxzoom": 15,
        "filter": ["all", ["<=", ["get", "filterrank"], 3],
            ["match", ["get", "class"],
                ["settlement", "disputed_settlement"],
                ["match", ["get", "worldview"],
                    ["all", "CN"], true, false
                ], false
            ],
            ["step", ["zoom"], false, 2, ["<=", ["get", "symbolrank"], 6], 4, ["<", ["get", "symbolrank"], 7], 6, ["<", ["get", "symbolrank"], 8], 7, ["<", ["get", "symbolrank"], 10], 10, ["<", ["get", "symbolrank"], 11], 11, ["<", ["get", "symbolrank"], 13], 12, ["<", ["get", "symbolrank"], 15], 13, [">=", ["get", "symbolrank"], 11], 14, [">=", ["get", "symbolrank"], 15]],
            ["step", ["pitch"], true, 50, ["<", ["distance-from-center"], 3], 60, ["<", ["distance-from-center"], 4], 70, ["<", ["distance-from-center"], 5]]
        ],
        "type": "symbol",
        "source": "composite",
        "id": "settlement-major-label",
        "paint": {
            "text-color": ["step", ["get", "symbolrank"], "hsla(215, 0%, 79%, 0.86)", 11, "hsla(215, 0%, 66%, 0.86)", 16, "hsla(215, 0%, 60%, 0.86)"],
            "text-halo-color": "hsla(215, 0%, 17%, 0.86)",
            "text-halo-width": 1,
            "icon-opacity": ["step", ["zoom"], 1, 8, 0],
            "text-halo-blur": 1
        },
        "source-layer": "place_label"
    }, {
        "minzoom": 3,
        "layout": {
            "text-size": ["interpolate", ["cubic-bezier", 0.85, 0.7, 0.65, 1],
                ["zoom"], 4, ["step", ["get", "symbolrank"], 14.4, 6, 12.8, 7, 11.200000000000001], 9, ["step", ["get", "symbolrank"], 33.6, 6, 25.6, 7, 22.400000000000002]
            ],
            "text-transform": "uppercase",
            "text-font": ["DIN Pro Bold", "Arial Unicode MS Bold"],
            "text-field": ["coalesce", ["get", "name_zh-Hans"],
                ["get", "name"]
            ],
            "text-letter-spacing": 0.15,
            "text-max-width": 6
        },
        "metadata": {
            "mapbox:featureComponent": "place-labels",
            "mapbox:group": "Place labels, place-labels"
        },
        "maxzoom": 9,
        "filter": ["all", ["match", ["get", "class"],
                ["state", "disputed_state"],
                ["match", ["get", "worldview"],
                    ["all", "CN"], true, false
                ], false
            ],
            ["step", ["pitch"], true, 50, ["<", ["distance-from-center"], 3], 60, ["<", ["distance-from-center"], 4], 70, ["<", ["distance-from-center"], 5]]
        ],
        "type": "symbol",
        "source": "composite",
        "id": "state-label",
        "paint": {
            "text-color": "hsla(215, 100%, 1%, 0.86)",
            "text-halo-color": "hsla(215, 0%, 100%, 0.86)",
            "text-opacity": 0.5,
            "text-halo-blur": 1
        },
        "source-layer": "place_label"
    }, {
        "minzoom": 1,
        "layout": {
            "icon-image": "",
            "text-field": ["coalesce", ["get", "name_zh-Hans"],
                ["get", "name"]
            ],
            "text-line-height": 1.1,
            "text-max-width": 6,
            "text-font": ["DIN Pro Medium", "Arial Unicode MS Regular"],
            "text-radial-offset": ["step", ["zoom"], 0.6, 8, 0],
            "text-size": ["interpolate", ["cubic-bezier", 0.2, 0, 0.7, 1],
                ["zoom"], 1, ["step", ["get", "symbolrank"], 18.7, 4, 15.299999999999999, 5, 13.6], 9, ["step", ["get", "symbolrank"], 37.4, 4, 32.3, 5, 28.9]
            ]
        },
        "metadata": {
            "mapbox:featureComponent": "place-labels",
            "mapbox:group": "Place labels, place-labels"
        },
        "maxzoom": 10,
        "filter": ["all", ["match", ["get", "class"],
                ["country", "disputed_country"],
                ["match", ["get", "worldview"],
                    ["all", "CN"], true, false
                ], false
            ],
            ["step", ["pitch"], true, 50, ["<", ["distance-from-center"], 3], 60, ["<", ["distance-from-center"], 4], 70, ["<", ["distance-from-center"], 5]]
        ],
        "type": "symbol",
        "source": "composite",
        "id": "country-label",
        "paint": {
            "icon-opacity": ["step", ["zoom"],
                ["case", ["has", "text_anchor"], 1, 0], 7, 0
            ],
            "text-color": "hsla(63, 0%, 94%, 0.86)",
            "text-halo-color": "hsla(215, 3%, 22%, 0.86)",
            "text-halo-width": 1.5,
            "text-halo-blur": 0.75
        },
        "source-layer": "place_label"
    }, {
        "minzoom": 0.75,
        "layout": {
            "text-field": ["step", ["zoom"],
                ["coalesce", ["get", "name_zh-Hans"],
                    ["get", "name"]
                ], 22, ["coalesce", ["get", "name_zh-Hans"],
                    ["get", "name"]
                ]
            ],
            "text-line-height": 1.1,
            "text-max-width": 6,
            "text-font": ["DIN Pro Medium", "Arial Unicode MS Regular"],
            "text-size": ["interpolate", ["exponential", 0.5],
                ["zoom"], 0, 10, 2.5, 15
            ],
            "text-transform": "uppercase",
            "text-letter-spacing": 0.05
        },
        "metadata": {
            "mapbox:featureComponent": "place-labels",
            "mapbox:group": "Place labels, place-labels"
        },
        "maxzoom": 3,
        "filter": ["==", ["get", "class"], "continent"],
        "type": "symbol",
        "source": "composite",
        "id": "continent-label",
        "paint": {
            "text-color": "hsla(215, 0%, 15%, 0.86)",
            "text-halo-color": "hsla(215, 4%, 82%, 0.86)",
            "text-halo-width": 1.5,
            "text-opacity": ["interpolate", ["linear"],
                ["zoom"], 0, 0.8, 1.5, 0.5, 2.5, 0
            ]
        },
        "source-layer": "natural_label"
    }],
    "created": "2024-06-01T09:15:03.411Z",
    "modified": "2024-06-13T04:49:42.227Z",
    "id": "clwvwdd2r01d301nyayhc6p6g",
    "owner": "notheorem",
    "visibility": "private",
    "protected": false,
    "draft": false
}

*/

#endif /* nav_mb_json_h */
