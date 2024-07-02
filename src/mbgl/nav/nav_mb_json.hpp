//
//  nav_mb_json.hpp
//  Mapbox GL Native
//
//  Created by BernieZhao on 2023/11/23.
//

#ifndef nav_mb_json_h
#define nav_mb_json_h


namespace nav {
namespace mb {

void printJson(const std::string& json) {
//    printf("style json\n\n%s\n\n\n\n", json.c_str());
}

}
}


/*

{
    "version": 8,
    "name": "颜色主题标准模版",
    "metadata": {
        "mapbox:type": "default",
        "mapbox:variation": "monochrome-sky-v1",
        "mapbox:autocomposite": true,
        "mapbox:sdk-support": {
            "js": "3.5.0",
            "android": "11.5.0",
            "ios": "11.6.0"
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
                "name": "Buildings, building-labels",
                "collapsed": false
            },
            "Transit, surface": {
                "name": "Transit, surface",
                "collapsed": true
            },
            "Land & water, land": {
                "name": "Land & water, land",
                "collapsed": false
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
                "collapsed": false
            },
            "Buildings, built": {
                "name": "Buildings, built",
                "collapsed": true
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
                "collapsed": true
            },
            "Walking, cycling, etc., tunnels": {
                "name": "Walking, cycling, etc., tunnels",
                "collapsed": true
            },
            "Walking, cycling, etc., walking-cycling-labels": {
                "name": "Walking, cycling, etc., walking-cycling-labels",
                "collapsed": true
            },
            "Walking, cycling, etc., surface": {
                "name": "Walking, cycling, etc., surface",
                "collapsed": false
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
    "center": [114.15423248940607, 22.283663380671697],
    "zoom": 20.51340255071003,
    "bearing": 150.01216823514005,
    "pitch": 79.50000000000001,
    "sources": {
        "composite": {
            "url": "mapbox://mapbox.mapbox-streets-v8,mapbox.mapbox-terrain-v2,mapbox.mapbox-bathymetry-v2",
            "type": "vector"
        }
    },
    "sprite": "mapbox://sprites/navworks/clxx105i700yr01po4zbn2jc1/ah15fcb529a9fx7wqmaq3q0o0",
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
                ["zoom"], 9, "hsl(200, 50%, 50%)", 11, "hsl(200, 49%, 35%)"
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
            "fill-color": "hsl(200, 42%, 33%)",
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
            "fill-color": "hsl(200, 42%, 33%)",
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
                ["zoom"], 14, ["match", ["get", "class"], "shadow", "hsla(200, 45%, 45%, 0.06)", "hsla(200, 49%, 44%, 0.04)"], 16, ["match", ["get", "class"], "shadow", "hsla(200, 45%, 45%, 0)", "hsla(200, 49%, 44%, 0)"]
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
            "line-color": "hsl(200, 45%, 28%)",
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
            "fill-color": "hsl(200, 45%, 28%)"
        },
        "metadata": {
            "mapbox:featureComponent": "land-and-water",
            "mapbox:group": "Land & water, water"
        }
    }, {
        "layout": {},
        "metadata": {
            "mapbox:featureComponent": "land-and-water",
            "mapbox:group": "Land & water, water"
        },
        "maxzoom": 8,
        "type": "fill",
        "source": "composite",
        "id": "water-depth",
        "paint": {
            "fill-antialias": false,
            "fill-color": ["interpolate", ["linear"],
                ["zoom"], 6, ["interpolate", ["linear"],
                    ["get", "min_depth"], 0, "hsla(200, 45%, 28%, 0.35)", 200, "hsla(200, 45%, 27%, 0.95)", 7000, "hsla(200, 45%, 22%, 0.95)"
                ], 8, ["interpolate", ["linear"],
                    ["get", "min_depth"], 0, "hsla(200, 45%, 28%, 0)", 200, "hsla(200, 45%, 27%, 0)", 7000, "hsla(200, 45%, 22%, 0)"
                ]
            ]
        },
        "source-layer": "depth"
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
                ["zoom"], 9, "hsl(200, 50%, 50%)", 11, "hsl(200, 49%, 35%)"
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
                ["zoom"], 9, "hsl(200, 50%, 50%)", 11, "hsl(200, 49%, 35%)"
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
            "fill-color": "hsl(200, 49%, 44%)",
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
            "line-color": "hsl(200, 49%, 44%)",
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
        "layout": {},
        "metadata": {
            "mapbox:featureComponent": "buildings",
            "mapbox:group": "Buildings, built"
        },
        "filter": ["all", ["==", ["get", "underground"], "true"],
            ["==", ["geometry-type"], "Polygon"]
        ],
        "type": "fill",
        "source": "composite",
        "id": "building-underground",
        "paint": {
            "fill-color": "hsl(200, 45%, 31%)",
            "fill-opacity": ["interpolate", ["linear"],
                ["zoom"], 15, 0, 16, 0.5
            ]
        },
        "source-layer": "building"
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
            "line-color": "hsl(200, 44%, 30%)",
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
            "line-color": "hsl(200, 44%, 30%)",
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
            "line-color": "hsl(200, 44%, 30%)",
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
            "line-color": "hsl(200, 44%, 30%)",
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
            "line-color": "hsl(200, 44%, 30%)",
            "line-dasharray": [10, 0]
        },
        "metadata": {
            "mapbox:featureComponent": "walking-cycling",
            "mapbox:group": "Walking, cycling, etc., tunnels"
        }
    }, {
        "id": "tunnel-minor-case-navigation",
        "type": "line",
        "source": "composite",
        "source-layer": "road",
        "minzoom": 15,
        "filter": ["all", ["==", ["get", "structure"], "tunnel"],
            ["match", ["get", "class"],
                ["track", "secondary_link", "tertiary_link", "service"], true, false
            ],
            ["==", ["geometry-type"], "LineString"]
        ],
        "paint": {
            "line-width": ["interpolate", ["exponential", 1.2],
                ["zoom"], 15, 0.75, 18, 1.5
            ],
            "line-color": "hsl(200, 46%, 46%)",
            "line-gap-width": ["interpolate", ["exponential", 1.5],
                ["zoom"], 15, ["match", ["get", "class"], "track", 1, 0.5], 18, 10, 22, 100
            ],
            "line-dasharray": [3, 3]
        },
        "metadata": {
            "mapbox:featureComponent": "road-network",
            "mapbox:group": "Road network, tunnels"
        }
    }, {
        "id": "tunnel-street-case-navigation",
        "type": "line",
        "source": "composite",
        "source-layer": "road",
        "minzoom": 14,
        "filter": ["all", ["==", ["get", "structure"], "tunnel"],
            ["match", ["get", "class"],
                ["street", "street_limited", "primary_link"], true, false
            ],
            ["==", ["geometry-type"], "LineString"]
        ],
        "paint": {
            "line-width": ["interpolate", ["exponential", 1.2],
                ["zoom"], 10, 0.5, 18, 2
            ],
            "line-color": "hsl(200, 46%, 46%)",
            "line-gap-width": ["interpolate", ["exponential", 1.5],
                ["zoom"], 14, 2, 18, 20, 22, 200
            ],
            "line-dasharray": [3, 3]
        },
        "metadata": {
            "mapbox:featureComponent": "road-network",
            "mapbox:group": "Road network, tunnels"
        }
    }, {
        "id": "tunnel-secondary-tertiary-case-navigation",
        "type": "line",
        "source": "composite",
        "source-layer": "road",
        "minzoom": 12,
        "filter": ["all", ["==", ["get", "structure"], "tunnel"],
            ["match", ["get", "class"],
                ["secondary", "tertiary"], true, false
            ],
            ["==", ["geometry-type"], "LineString"]
        ],
        "paint": {
            "line-width": ["interpolate", ["exponential", 1.2],
                ["zoom"], 10, 0.75, 18, 2
            ],
            "line-color": "hsl(200, 46%, 46%)",
            "line-gap-width": ["interpolate", ["exponential", 1.5],
                ["zoom"], 10, 0.75, 18, 28, 22, 280
            ],
            "line-dasharray": [3, 3]
        },
        "metadata": {
            "mapbox:featureComponent": "road-network",
            "mapbox:group": "Road network, tunnels"
        }
    }, {
        "id": "tunnel-primary-case-navigation",
        "type": "line",
        "source": "composite",
        "source-layer": "road",
        "minzoom": 10,
        "filter": ["all", ["==", ["get", "structure"], "tunnel"],
            ["==", ["get", "class"], "primary"],
            ["==", ["geometry-type"], "LineString"]
        ],
        "paint": {
            "line-width": ["interpolate", ["exponential", 1.2],
                ["zoom"], 10, 1, 18, 2
            ],
            "line-color": "hsl(200, 46%, 46%)",
            "line-gap-width": ["interpolate", ["exponential", 1.5],
                ["zoom"], 10, 1.125, 18, 32, 22, 200
            ],
            "line-dasharray": [3, 3]
        },
        "metadata": {
            "mapbox:featureComponent": "road-network",
            "mapbox:group": "Road network, tunnels"
        }
    }, {
        "id": "tunnel-major-link-case-navigation",
        "type": "line",
        "source": "composite",
        "source-layer": "road",
        "minzoom": 13,
        "filter": ["all", ["==", ["get", "structure"], "tunnel"],
            ["match", ["get", "class"],
                ["motorway_link", "trunk_link"], true, false
            ],
            ["==", ["geometry-type"], "LineString"]
        ],
        "paint": {
            "line-width": ["interpolate", ["exponential", 1.2],
                ["zoom"], 10, 0.75, 18, 2
            ],
            "line-color": "hsl(200, 46%, 46%)",
            "line-gap-width": ["interpolate", ["exponential", 1.5],
                ["zoom"], 12, 0.75, 14, 2, 18, 20, 22, 200
            ],
            "line-dasharray": [3, 3]
        },
        "metadata": {
            "mapbox:featureComponent": "road-network",
            "mapbox:group": "Road network, tunnels"
        }
    }, {
        "id": "tunnel-motorway-trunk-case-navigation",
        "type": "line",
        "source": "composite",
        "source-layer": "road",
        "minzoom": 13,
        "filter": ["all", ["==", ["get", "structure"], "tunnel"],
            ["match", ["get", "class"],
                ["motorway", "trunk"], true, false
            ],
            ["==", ["geometry-type"], "LineString"]
        ],
        "paint": {
            "line-width": ["interpolate", ["exponential", 1.2],
                ["zoom"], 10, 1, 18, 2
            ],
            "line-color": "hsl(200, 46%, 46%)",
            "line-gap-width": ["interpolate", ["exponential", 1.5],
                ["zoom"], 5, 1.25, 18, 32, 22, 320
            ],
            "line-dasharray": [3, 3]
        },
        "metadata": {
            "mapbox:featureComponent": "road-network",
            "mapbox:group": "Road network, tunnels"
        }
    }, {
        "id": "tunnel-construction-navigation",
        "type": "line",
        "source": "composite",
        "source-layer": "road",
        "minzoom": 14,
        "filter": ["all", ["==", ["get", "structure"], "tunnel"],
            ["==", ["get", "class"], "construction"],
            ["==", ["geometry-type"], "LineString"]
        ],
        "paint": {
            "line-width": ["interpolate", ["exponential", 1.5],
                ["zoom"], 14, 2, 18, 20, 22, 200
            ],
            "line-color": "hsl(200, 48%, 32%)",
            "line-dasharray": ["step", ["zoom"],
                ["literal", [0.4, 0.8]], 15, ["literal", [0.3, 0.6]], 16, ["literal", [0.2, 0.3]], 17, ["literal", [0.2, 0.25]], 18, ["literal", [0.15, 0.15]]
            ]
        },
        "metadata": {
            "mapbox:featureComponent": "road-network",
            "mapbox:group": "Road network, tunnels"
        }
    }, {
        "id": "tunnel-minor-navigation",
        "type": "line",
        "source": "composite",
        "source-layer": "road",
        "minzoom": 15,
        "filter": ["all", ["==", ["get", "structure"], "tunnel"],
            ["match", ["get", "class"],
                ["track", "secondary_link", "tertiary_link", "service"], true, false
            ],
            ["==", ["geometry-type"], "LineString"]
        ],
        "paint": {
            "line-width": ["interpolate", ["exponential", 1.5],
                ["zoom"], 15, ["match", ["get", "class"], "track", 1, 0.5], 18, 10, 22, 100
            ],
            "line-color": "hsl(200, 48%, 32%)"
        },
        "metadata": {
            "mapbox:featureComponent": "road-network",
            "mapbox:group": "Road network, tunnels"
        }
    }, {
        "id": "tunnel-major-link-navigation",
        "type": "line",
        "source": "composite",
        "source-layer": "road",
        "minzoom": 13,
        "filter": ["all", ["==", ["get", "structure"], "tunnel"],
            ["match", ["get", "class"],
                ["motorway_link", "trunk_link"], true, false
            ],
            ["==", ["geometry-type"], "LineString"]
        ],
        "paint": {
            "line-width": ["interpolate", ["exponential", 1.5],
                ["zoom"], 12, 0.75, 14, 2, 18, 20, 22, 200
            ],
            "line-color": "hsl(200, 48%, 32%)"
        },
        "metadata": {
            "mapbox:featureComponent": "road-network",
            "mapbox:group": "Road network, tunnels"
        }
    }, {
        "minzoom": 14,
        "layout": {
            "line-cap": "round",
            "line-join": "round"
        },
        "metadata": {
            "mapbox:featureComponent": "road-network",
            "mapbox:group": "Road network, tunnels"
        },
        "filter": ["all", ["==", ["get", "structure"], "tunnel"],
            ["match", ["get", "class"],
                ["street", "street_limited", "primary_link"], true, false
            ],
            ["==", ["geometry-type"], "LineString"]
        ],
        "type": "line",
        "source": "composite",
        "id": "tunnel-street-navigation",
        "paint": {
            "line-width": ["interpolate", ["exponential", 1.5],
                ["zoom"], 12, 0.5, 14, 2, 18, 20, 22, 200
            ],
            "line-color": "hsl(200, 48%, 32%)"
        },
        "source-layer": "road"
    }, {
        "minzoom": 13,
        "metadata": {
            "mapbox:featureComponent": "road-network",
            "mapbox:group": "Road network, tunnels"
        },
        "maxzoom": 14,
        "filter": ["all", ["==", ["get", "structure"], "tunnel"],
            ["match", ["get", "class"],
                ["street", "street_limited", "primary_link"], true, false
            ],
            ["==", ["geometry-type"], "LineString"]
        ],
        "type": "line",
        "source": "composite",
        "id": "tunnel-street-low-navigation",
        "paint": {
            "line-width": ["interpolate", ["exponential", 1.5],
                ["zoom"], 12, 0.5, 14, 2
            ],
            "line-color": "hsl(200, 48%, 32%)"
        },
        "source-layer": "road"
    }, {
        "id": "tunnel-secondary-tertiary-navigation",
        "type": "line",
        "source": "composite",
        "source-layer": "road",
        "minzoom": 13,
        "filter": ["all", ["==", ["get", "structure"], "tunnel"],
            ["match", ["get", "class"],
                ["secondary", "tertiary"], true, false
            ],
            ["==", ["geometry-type"], "LineString"]
        ],
        "paint": {
            "line-width": ["interpolate", ["exponential", 1.5],
                ["zoom"], 10, 0.75, 18, 28, 22, 280
            ],
            "line-color": "hsl(200, 48%, 32%)"
        },
        "metadata": {
            "mapbox:featureComponent": "road-network",
            "mapbox:group": "Road network, tunnels"
        }
    }, {
        "id": "tunnel-primary-navigation",
        "type": "line",
        "source": "composite",
        "source-layer": "road",
        "minzoom": 13,
        "filter": ["all", ["==", ["get", "structure"], "tunnel"],
            ["==", ["get", "class"], "primary"],
            ["==", ["geometry-type"], "LineString"]
        ],
        "paint": {
            "line-width": ["interpolate", ["exponential", 1.5],
                ["zoom"], 10, 1.125, 18, 32, 22, 200
            ],
            "line-color": "hsl(200, 48%, 32%)"
        },
        "metadata": {
            "mapbox:featureComponent": "road-network",
            "mapbox:group": "Road network, tunnels"
        }
    }, {
        "id": "tunnel-motorway-trunk-navigation",
        "type": "line",
        "source": "composite",
        "source-layer": "road",
        "minzoom": 13,
        "filter": ["all", ["==", ["get", "structure"], "tunnel"],
            ["match", ["get", "class"],
                ["motorway", "trunk"], true, false
            ],
            ["==", ["geometry-type"], "LineString"]
        ],
        "paint": {
            "line-width": ["interpolate", ["exponential", 1.5],
                ["zoom"], 3, 0.75, 18, 32, 22, 320
            ],
            "line-color": "hsl(200, 48%, 32%)"
        },
        "metadata": {
            "mapbox:featureComponent": "road-network",
            "mapbox:group": "Road network, tunnels"
        }
    }, {
        "minzoom": 16,
        "layout": {
            "symbol-placement": "line",
            "symbol-spacing": ["interpolate", ["linear"],
                ["zoom"], 15, 400, 18, 600, 22, 1200
            ],
            "icon-image": "oneway-small",
            "icon-size": ["interpolate", ["exponential", 1.5],
                ["zoom"], 15, ["match", ["get", "class"],
                    ["track", "secondary_link", "tertiary_link", "service"], 0.15, 0.25
                ], 18, ["match", ["get", "class"],
                    ["primary", "secondary", "tertiary"], 0.7, ["street", "street_limited", "primary_link"], 0.5, 0.4
                ]
            ],
            "icon-rotation-alignment": "map",
            "icon-allow-overlap": true,
            "icon-ignore-placement": true
        },
        "metadata": {
            "mapbox:featureComponent": "road-network",
            "mapbox:group": "Road network, tunnels"
        },
        "filter": ["all", ["==", ["get", "structure"], "tunnel"],
            ["==", ["get", "oneway"], "true"],
            ["step", ["zoom"],
                ["match", ["get", "class"],
                    ["primary", "secondary", "street", "street_limited", "tertiary"], true, false
                ], 16, ["match", ["get", "class"],
                    ["primary", "secondary", "tertiary", "street", "street_limited", "primary_link", "secondary_link", "tertiary_link", "service", "track"], true, false
                ]
            ],
            ["step", ["pitch"], true, 50, ["<", ["distance-from-center"], 1], 60, ["<", ["distance-from-center"], 1.5], 70, ["<", ["distance-from-center"], 2]]
        ],
        "type": "symbol",
        "source": "composite",
        "id": "tunnel-oneway-arrow-blue-navigation",
        "paint": {},
        "source-layer": "road"
    }, {
        "minzoom": 16,
        "layout": {
            "symbol-placement": "line",
            "symbol-spacing": ["interpolate", ["linear"],
                ["zoom"], 15, 400, 18, 600, 22, 1200
            ],
            "icon-image": ["step", ["zoom"], "oneway-white-small", 18, "oneway-white-large"],
            "icon-size": ["interpolate", ["exponential", 1.5],
                ["zoom"], 15, ["match", ["get", "class"],
                    ["motorway", "trunk"], 0.45, 0.3
                ], 20, ["match", ["get", "class"],
                    ["motorway", "trunk"], 0.6, 0.4
                ]
            ],
            "icon-rotation-alignment": "map",
            "icon-allow-overlap": true,
            "icon-ignore-placement": true
        },
        "metadata": {
            "mapbox:featureComponent": "road-network",
            "mapbox:group": "Road network, tunnels"
        },
        "filter": ["all", ["==", ["get", "structure"], "tunnel"],
            ["match", ["get", "class"],
                ["motorway", "motorway_link", "trunk", "trunk_link"], true, false
            ],
            ["==", ["get", "oneway"], "true"],
            ["step", ["pitch"], true, 50, ["<", ["distance-from-center"], 1], 60, ["<", ["distance-from-center"], 1.5], 70, ["<", ["distance-from-center"], 2]]
        ],
        "type": "symbol",
        "source": "composite",
        "id": "tunnel-oneway-arrow-white-navigation",
        "paint": {},
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
            "line-color": "hsl(200, 49%, 44%)",
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
            "line-color": "hsl(200, 49%, 44%)",
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
            "line-color": "hsl(200, 49%, 44%)",
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
            "line-color": "hsl(200, 49%, 44%)",
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
            "line-color": "hsl(200, 49%, 44%)",
            "line-dasharray": [10, 0]
        },
        "source-layer": "road"
    }, {
        "id": "turning-feature-outline-navigation",
        "type": "circle",
        "source": "composite",
        "source-layer": "road",
        "minzoom": 15,
        "filter": ["all", ["match", ["get", "class"],
                ["turning_circle", "turning_loop"], true, false
            ],
            ["==", ["geometry-type"], "Point"]
        ],
        "paint": {
            "circle-radius": ["interpolate", ["exponential", 1.5],
                ["zoom"], 15, 3.2, 16, 6, 18, 17, 22, 170
            ],
            "circle-color": "hsl(200, 49%, 44%)",
            "circle-stroke-width": ["interpolate", ["linear"],
                ["zoom"], 15, 0.8, 16, 1.2, 18, 2
            ],
            "circle-stroke-color": "hsl(200, 43%, 33%)",
            "circle-pitch-alignment": "map"
        },
        "metadata": {
            "mapbox:featureComponent": "road-network",
            "mapbox:group": "Road network, surface"
        }
    }, {
        "minzoom": 15,
        "layout": {
            "line-join": "round",
            "line-cap": "round"
        },
        "metadata": {
            "mapbox:featureComponent": "road-network",
            "mapbox:group": "Road network, surface"
        },
        "filter": ["all", ["match", ["get", "class"],
                ["track", "secondary_link", "tertiary_link", "service"], true, false
            ],
            ["match", ["get", "structure"],
                ["none", "ford"], true, false
            ],
            ["==", ["geometry-type"], "LineString"]
        ],
        "type": "line",
        "source": "composite",
        "id": "road-minor-case-navigation",
        "paint": {
            "line-width": ["interpolate", ["exponential", 1.2],
                ["zoom"], 15, 0.75, 18, 1.5
            ],
            "line-color": "hsl(200, 43%, 33%)",
            "line-gap-width": ["interpolate", ["exponential", 1.5],
                ["zoom"], 15, ["match", ["get", "class"], "track", 1, 0.5], 18, 10, 22, 100
            ]
        },
        "source-layer": "road"
    }, {
        "minzoom": 14,
        "layout": {
            "line-cap": ["step", ["zoom"], "butt", 13, "round"],
            "line-join": ["step", ["zoom"], "miter", 13, "round"]
        },
        "metadata": {
            "mapbox:featureComponent": "road-network",
            "mapbox:group": "Road network, surface"
        },
        "filter": ["all", ["match", ["get", "class"],
                ["street", "street_limited", "primary_link"], true, false
            ],
            ["match", ["get", "structure"],
                ["none", "ford"], true, false
            ],
            ["==", ["geometry-type"], "LineString"]
        ],
        "type": "line",
        "source": "composite",
        "id": "road-street-case-navigation",
        "paint": {
            "line-width": ["interpolate", ["exponential", 1.2],
                ["zoom"], 10, 0.5, 18, 2
            ],
            "line-color": "hsl(200, 43%, 33%)",
            "line-gap-width": ["interpolate", ["exponential", 1.5],
                ["zoom"], 14, 2, 18, 20, 22, 200
            ]
        },
        "source-layer": "road"
    }, {
        "minzoom": 12,
        "layout": {
            "line-cap": ["step", ["zoom"], "butt", 14, "round"],
            "line-join": ["step", ["zoom"], "miter", 14, "round"]
        },
        "metadata": {
            "mapbox:featureComponent": "road-network",
            "mapbox:group": "Road network, surface"
        },
        "filter": ["all", ["match", ["get", "class"],
                ["secondary", "tertiary"], true, false
            ],
            ["match", ["get", "structure"],
                ["none", "ford"], true, false
            ],
            ["==", ["geometry-type"], "LineString"]
        ],
        "type": "line",
        "source": "composite",
        "id": "road-secondary-tertiary-case-navigation",
        "paint": {
            "line-width": ["interpolate", ["exponential", 1.2],
                ["zoom"], 10, 0.75, 18, 2
            ],
            "line-color": "hsl(200, 43%, 33%)",
            "line-gap-width": ["interpolate", ["exponential", 1.5],
                ["zoom"], 10, 0.75, 18, 28, 22, 280
            ]
        },
        "source-layer": "road"
    }, {
        "minzoom": 12,
        "layout": {
            "line-cap": ["step", ["zoom"], "butt", 14, "round"],
            "line-join": ["step", ["zoom"], "miter", 14, "round"]
        },
        "metadata": {
            "mapbox:featureComponent": "road-network",
            "mapbox:group": "Road network, surface"
        },
        "filter": ["all", ["==", ["get", "class"], "primary"],
            ["match", ["get", "structure"],
                ["none", "ford"], true, false
            ],
            ["==", ["geometry-type"], "LineString"]
        ],
        "type": "line",
        "source": "composite",
        "id": "road-primary-case-navigation",
        "paint": {
            "line-width": ["interpolate", ["exponential", 1.2],
                ["zoom"], 10, 1, 18, 2
            ],
            "line-color": "hsl(200, 43%, 33%)",
            "line-gap-width": ["interpolate", ["exponential", 1.5],
                ["zoom"], 10, 1.125, 18, 32, 22, 200
            ]
        },
        "source-layer": "road"
    }, {
        "minzoom": 11,
        "layout": {
            "line-cap": ["step", ["zoom"], "butt", 13, "round"],
            "line-join": ["step", ["zoom"], "miter", 13, "round"]
        },
        "metadata": {
            "mapbox:featureComponent": "road-network",
            "mapbox:group": "Road network, surface"
        },
        "filter": ["all", ["match", ["get", "class"],
                ["motorway_link", "trunk_link"], true, false
            ],
            ["match", ["get", "structure"],
                ["none", "ford"], true, false
            ],
            ["==", ["geometry-type"], "LineString"]
        ],
        "type": "line",
        "source": "composite",
        "id": "road-major-link-case-navigation",
        "paint": {
            "line-width": ["interpolate", ["exponential", 1.2],
                ["zoom"], 10, 0.75, 18, 2
            ],
            "line-color": "hsl(200, 43%, 33%)",
            "line-gap-width": ["interpolate", ["exponential", 1.5],
                ["zoom"], 12, 0.75, 14, 2, 18, 20, 22, 200
            ]
        },
        "source-layer": "road"
    }, {
        "minzoom": 13,
        "layout": {
            "line-cap": ["step", ["zoom"], "butt", 14, "round"],
            "line-join": ["step", ["zoom"], "miter", 14, "round"]
        },
        "metadata": {
            "mapbox:featureComponent": "road-network",
            "mapbox:group": "Road network, surface"
        },
        "filter": ["all", ["match", ["get", "class"],
                ["motorway", "trunk"], true, false
            ],
            ["match", ["get", "structure"],
                ["none", "ford"], true, false
            ],
            ["==", ["geometry-type"], "LineString"]
        ],
        "type": "line",
        "source": "composite",
        "id": "road-motorway-trunk-case-navigation",
        "paint": {
            "line-width": ["interpolate", ["exponential", 1.2],
                ["zoom"], 10, 1, 22, 2
            ],
            "line-color": "hsl(200, 43%, 33%)",
            "line-gap-width": ["interpolate", ["exponential", 1.5],
                ["zoom"], 5, 1.25, 18, 32, 22, 320
            ]
        },
        "source-layer": "road"
    }, {
        "id": "road-construction-navigation",
        "type": "line",
        "source": "composite",
        "source-layer": "road",
        "minzoom": 14,
        "filter": ["all", ["==", ["get", "class"], "construction"],
            ["match", ["get", "structure"],
                ["none", "ford"], true, false
            ],
            ["==", ["geometry-type"], "LineString"]
        ],
        "paint": {
            "line-width": ["interpolate", ["exponential", 1.5],
                ["zoom"], 14, 2, 18, 20, 22, 200
            ],
            "line-color": "hsl(200, 49%, 44%)",
            "line-dasharray": ["step", ["zoom"],
                ["literal", [0.4, 0.8]], 15, ["literal", [0.3, 0.6]], 16, ["literal", [0.2, 0.3]], 17, ["literal", [0.2, 0.25]], 18, ["literal", [0.15, 0.15]]
            ]
        },
        "metadata": {
            "mapbox:featureComponent": "road-network",
            "mapbox:group": "Road network, surface"
        }
    }, {
        "minzoom": 15,
        "layout": {
            "line-join": "round",
            "line-cap": "round"
        },
        "metadata": {
            "mapbox:featureComponent": "road-network",
            "mapbox:group": "Road network, surface"
        },
        "filter": ["all", ["match", ["get", "class"],
                ["track", "secondary_link", "tertiary_link", "service"], true, false
            ],
            ["match", ["get", "structure"],
                ["none", "ford"], true, false
            ],
            ["==", ["geometry-type"], "LineString"]
        ],
        "type": "line",
        "source": "composite",
        "id": "road-minor-navigation",
        "paint": {
            "line-width": ["interpolate", ["exponential", 1.5],
                ["zoom"], 15, ["match", ["get", "class"], "track", 1, 0.5], 18, 10, 22, 100
            ],
            "line-color": "hsl(200, 49%, 44%)"
        },
        "source-layer": "road"
    }, {
        "minzoom": 11,
        "layout": {
            "line-cap": ["step", ["zoom"], "butt", 13, "round"],
            "line-join": ["step", ["zoom"], "miter", 13, "round"]
        },
        "metadata": {
            "mapbox:featureComponent": "road-network",
            "mapbox:group": "Road network, surface"
        },
        "filter": ["all", ["match", ["get", "class"],
                ["motorway_link", "trunk_link"], true, false
            ],
            ["match", ["get", "structure"],
                ["none", "ford"], true, false
            ],
            ["==", ["geometry-type"], "LineString"]
        ],
        "type": "line",
        "source": "composite",
        "id": "road-major-link-navigation",
        "paint": {
            "line-width": ["interpolate", ["exponential", 1.5],
                ["zoom"], 12, 0.75, 14, 2, 18, 20, 22, 200
            ],
            "line-color": "hsl(200, 49%, 44%)"
        },
        "source-layer": "road"
    }, {
        "minzoom": 14,
        "layout": {
            "line-cap": "round",
            "line-join": "round"
        },
        "metadata": {
            "mapbox:featureComponent": "road-network",
            "mapbox:group": "Road network, surface"
        },
        "filter": ["all", ["match", ["get", "class"],
                ["street", "street_limited", "primary_link"], true, false
            ],
            ["match", ["get", "structure"],
                ["none", "ford"], true, false
            ],
            ["==", ["geometry-type"], "LineString"]
        ],
        "type": "line",
        "source": "composite",
        "id": "road-street-navigation",
        "paint": {
            "line-width": ["interpolate", ["exponential", 1.5],
                ["zoom"], 12, 0.5, 14, 2, 18, 20, 22, 200
            ],
            "line-color": "hsl(200, 49%, 44%)"
        },
        "source-layer": "road"
    }, {
        "minzoom": 11,
        "layout": {
            "line-cap": ["step", ["zoom"], "butt", 14, "round"],
            "line-join": ["step", ["zoom"], "miter", 14, "round"]
        },
        "metadata": {
            "mapbox:featureComponent": "road-network",
            "mapbox:group": "Road network, surface"
        },
        "maxzoom": 14,
        "filter": ["all", ["match", ["get", "class"],
                ["street", "street_limited", "primary_link"], true, false
            ],
            ["match", ["get", "structure"],
                ["none", "ford"], true, false
            ],
            ["==", ["geometry-type"], "LineString"]
        ],
        "type": "line",
        "source": "composite",
        "id": "road-street-low-navigation",
        "paint": {
            "line-width": ["interpolate", ["exponential", 1.5],
                ["zoom"], 12, 0.5, 14, 2
            ],
            "line-color": "hsl(200, 49%, 44%)"
        },
        "source-layer": "road"
    }, {
        "minzoom": 8,
        "layout": {
            "line-cap": ["step", ["zoom"], "butt", 11, "round"],
            "line-join": ["step", ["zoom"], "miter", 11, "round"]
        },
        "metadata": {
            "mapbox:featureComponent": "road-network",
            "mapbox:group": "Road network, surface"
        },
        "filter": ["all", ["match", ["get", "class"],
                ["secondary", "tertiary"], true, false
            ],
            ["match", ["get", "structure"],
                ["none", "ford"], true, false
            ],
            ["==", ["geometry-type"], "LineString"]
        ],
        "type": "line",
        "source": "composite",
        "id": "road-secondary-tertiary-navigation",
        "paint": {
            "line-width": ["interpolate", ["exponential", 1.5],
                ["zoom"], 10, 0.75, 18, 28, 22, 280
            ],
            "line-color": "hsl(200, 49%, 44%)"
        },
        "source-layer": "road"
    }, {
        "minzoom": 6,
        "layout": {
            "line-cap": ["step", ["zoom"], "butt", 11, "round"],
            "line-join": ["step", ["zoom"], "miter", 11, "round"]
        },
        "metadata": {
            "mapbox:featureComponent": "road-network",
            "mapbox:group": "Road network, surface"
        },
        "filter": ["all", ["==", ["get", "class"], "primary"],
            ["match", ["get", "structure"],
                ["none", "ford"], true, false
            ],
            ["==", ["geometry-type"], "LineString"]
        ],
        "type": "line",
        "source": "composite",
        "id": "road-primary-navigation",
        "paint": {
            "line-width": ["interpolate", ["exponential", 1.5],
                ["zoom"], 10, 1.125, 18, 32, 22, 200
            ],
            "line-color": "hsl(200, 49%, 44%)"
        },
        "source-layer": "road"
    }, {
        "minzoom": 3,
        "layout": {},
        "metadata": {
            "mapbox:featureComponent": "road-network",
            "mapbox:group": "Road network, surface"
        },
        "maxzoom": 13,
        "filter": ["all", ["step", ["zoom"],
                ["match", ["get", "class"],
                    ["motorway", "trunk"], true, false
                ], 5, ["all", ["match", ["get", "class"],
                        ["motorway", "trunk"], true, false
                    ],
                    ["match", ["get", "structure"],
                        ["none", "ford"], true, false
                    ]
                ]
            ],
            ["==", ["geometry-type"], "LineString"]
        ],
        "type": "line",
        "source": "composite",
        "id": "road-motorway-trunk-case-low-navigation",
        "paint": {
            "line-width": ["interpolate", ["exponential", 1.5],
                ["zoom"], 3, 0.2, 10, 1, 18, 2
            ],
            "line-color": "hsl(200, 43%, 33%)",
            "line-gap-width": ["interpolate", ["exponential", 1.5],
                ["zoom"], 3, 0.75, 18, 32, 22, 320
            ],
            "line-opacity": ["interpolate", ["linear"],
                ["zoom"], 3, 0, 3.5, 1
            ]
        },
        "source-layer": "road"
    }, {
        "minzoom": 3,
        "layout": {
            "line-cap": ["step", ["zoom"], "butt", 8, "round"],
            "line-join": ["step", ["zoom"], "miter", 8, "round"]
        },
        "metadata": {
            "mapbox:featureComponent": "road-network",
            "mapbox:group": "Road network, surface"
        },
        "filter": ["all", ["step", ["zoom"],
                ["match", ["get", "class"],
                    ["motorway", "trunk"], true, false
                ], 5, ["all", ["match", ["get", "class"],
                        ["motorway", "trunk"], true, false
                    ],
                    ["match", ["get", "structure"],
                        ["none", "ford"], true, false
                    ]
                ]
            ],
            ["==", ["geometry-type"], "LineString"]
        ],
        "type": "line",
        "source": "composite",
        "id": "road-motorway-trunk-navigation",
        "paint": {
            "line-width": ["interpolate", ["exponential", 1.5],
                ["zoom"], 3, 0.75, 18, 32, 22, 320
            ],
            "line-color": "hsl(200, 49%, 44%)",
            "line-opacity": ["interpolate", ["linear"],
                ["zoom"], 3, 0, 3.5, 1
            ]
        },
        "source-layer": "road"
    }, {
        "minzoom": 16,
        "layout": {
            "icon-image": "level-crossing",
            "icon-size": ["interpolate", ["exponential", 1.5],
                ["zoom"], 16, 0.25, 22, 1
            ],
            "icon-rotation-alignment": "map",
            "icon-allow-overlap": true,
            "icon-ignore-placement": true
        },
        "metadata": {
            "mapbox:featureComponent": "road-network",
            "mapbox:group": "Road network, surface"
        },
        "filter": ["all", ["==", ["get", "class"], "level_crossing"],
            ["step", ["pitch"], true, 50, ["<", ["distance-from-center"], 1], 60, ["<", ["distance-from-center"], 1.5], 70, ["<", ["distance-from-center"], 2]]
        ],
        "type": "symbol",
        "source": "composite",
        "id": "level-crossing-navigation",
        "paint": {},
        "source-layer": "road"
    }, {
        "minzoom": 16,
        "layout": {
            "symbol-placement": "line",
            "symbol-spacing": ["interpolate", ["linear"],
                ["zoom"], 15, 400, 18, 600, 22, 1200
            ],
            "icon-image": "oneway-small",
            "icon-size": ["interpolate", ["exponential", 1.5],
                ["zoom"], 15, ["match", ["get", "class"],
                    ["track", "secondary_link", "tertiary_link", "service"], 0.15, 0.25
                ], 18, ["match", ["get", "class"],
                    ["primary", "secondary", "tertiary"], 0.7, ["street", "street_limited", "primary_link"], 0.5, 0.4
                ]
            ],
            "icon-rotation-alignment": "map",
            "icon-allow-overlap": true,
            "icon-ignore-placement": true
        },
        "metadata": {
            "mapbox:featureComponent": "road-network",
            "mapbox:group": "Road network, surface"
        },
        "filter": ["all", ["==", ["get", "oneway"], "true"],
            ["step", ["zoom"],
                ["match", ["get", "class"],
                    ["primary", "secondary", "tertiary", "street", "street_limited"], true, false
                ], 16, ["match", ["get", "class"],
                    ["primary", "secondary", "tertiary", "street", "street_limited", "primary_link", "secondary_link", "tertiary_link", "service", "track"], true, false
                ]
            ],
            ["match", ["get", "structure"],
                ["none", "ford"], true, false
            ],
            ["step", ["pitch"], true, 50, ["<", ["distance-from-center"], 1], 60, ["<", ["distance-from-center"], 1.5], 70, ["<", ["distance-from-center"], 2]]
        ],
        "type": "symbol",
        "source": "composite",
        "id": "road-oneway-arrow-blue-navigation",
        "paint": {},
        "source-layer": "road"
    }, {
        "minzoom": 16,
        "layout": {
            "symbol-placement": "line",
            "symbol-spacing": ["interpolate", ["linear"],
                ["zoom"], 15, 400, 18, 600, 22, 1200
            ],
            "icon-image": ["step", ["zoom"], "oneway-white-small", 18, "oneway-white-large"],
            "icon-size": ["interpolate", ["exponential", 1.5],
                ["zoom"], 15, ["match", ["get", "class"],
                    ["motorway", "trunk"], 0.45, 0.3
                ], 20, ["match", ["get", "class"],
                    ["motorway", "trunk"], 0.6, 0.4
                ]
            ],
            "icon-rotation-alignment": "map",
            "icon-allow-overlap": true,
            "icon-ignore-placement": true
        },
        "metadata": {
            "mapbox:featureComponent": "road-network",
            "mapbox:group": "Road network, surface"
        },
        "filter": ["all", ["==", ["get", "oneway"], "true"],
            ["match", ["get", "class"],
                ["motorway", "trunk", "motorway_link", "trunk_link"], true, false
            ],
            ["match", ["get", "structure"],
                ["none", "ford"], true, false
            ],
            ["step", ["pitch"], true, 50, ["<", ["distance-from-center"], 1], 60, ["<", ["distance-from-center"], 1.5], 70, ["<", ["distance-from-center"], 2]]
        ],
        "type": "symbol",
        "source": "composite",
        "id": "road-oneway-arrow-white-navigation",
        "paint": {},
        "source-layer": "road"
    }, {
        "id": "turning-feature-navigation",
        "type": "circle",
        "source": "composite",
        "source-layer": "road",
        "minzoom": 15,
        "filter": ["all", ["match", ["get", "class"],
                ["turning_circle", "turning_loop"], true, false
            ],
            ["==", ["geometry-type"], "Point"]
        ],
        "paint": {
            "circle-radius": ["interpolate", ["exponential", 1.5],
                ["zoom"], 15, 3.2, 16, 6, 18, 17, 22, 170
            ],
            "circle-color": "hsl(200, 49%, 44%)",
            "circle-pitch-alignment": "map"
        },
        "metadata": {
            "mapbox:featureComponent": "road-network",
            "mapbox:group": "Road network, surface"
        }
    }, {
        "minzoom": 17,
        "layout": {
            "icon-size": ["interpolate", ["linear"],
                ["zoom"], 16, 0.03, 18, 0.07, 19, 0.17, 22, 0.7
            ],
            "icon-image": ["step", ["zoom"], "crosswalk-small", 18, "crosswalk-large"],
            "icon-rotate": ["get", "direction"],
            "icon-rotation-alignment": "map",
            "icon-allow-overlap": true,
            "icon-ignore-placement": true
        },
        "metadata": {
            "mapbox:featureComponent": "road-network",
            "mapbox:group": "Road network, surface"
        },
        "filter": ["all", ["==", ["get", "type"], "crosswalk"],
            ["==", ["geometry-type"], "Point"],
            ["step", ["pitch"], true, 50, ["<", ["distance-from-center"], 1], 60, ["<", ["distance-from-center"], 1.5], 70, ["<", ["distance-from-center"], 2]]
        ],
        "type": "symbol",
        "source": "composite",
        "id": "crosswalks",
        "paint": {},
        "source-layer": "structure"
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
                ["zoom"], 13, "hsl(200, 43%, 33%)", 17, "hsl(200, 44%, 30%)"
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
            "line-color": "hsl(200, 44%, 30%)",
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
            "line-color": "hsl(200, 49%, 44%)",
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
            "line-color": "hsl(200, 49%, 44%)",
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
            "line-width": ["interpolate", ["exponential", 1.5],
                ["zoom"], 15, 1, 18, 4
            ],
            "line-color": "hsl(200, 49%, 44%)",
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
            "line-color": "hsl(200, 49%, 44%)",
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
            "line-color": "hsl(200, 49%, 44%)",
            "line-dasharray": [10, 0]
        },
        "metadata": {
            "mapbox:featureComponent": "walking-cycling",
            "mapbox:group": "Walking, cycling, etc., barriers-bridges"
        }
    }, {
        "id": "bridge-minor-case-navigation",
        "type": "line",
        "source": "composite",
        "source-layer": "road",
        "minzoom": 15,
        "filter": ["all", ["==", ["get", "structure"], "bridge"],
            ["match", ["get", "class"],
                ["track", "secondary_link", "tertiary_link", "service"], true, false
            ],
            ["==", ["geometry-type"], "LineString"]
        ],
        "paint": {
            "line-width": ["interpolate", ["exponential", 1.2],
                ["zoom"], 15, 0.75, 18, 1.5
            ],
            "line-color": "hsl(200, 43%, 33%)",
            "line-gap-width": ["interpolate", ["exponential", 1.5],
                ["zoom"], 15, ["match", ["get", "class"], "track", 1, 0.5], 18, 10, 22, 100
            ]
        },
        "metadata": {
            "mapbox:featureComponent": "road-network",
            "mapbox:group": "Road network, bridges"
        }
    }, {
        "id": "bridge-street-case-navigation",
        "type": "line",
        "source": "composite",
        "source-layer": "road",
        "minzoom": 14,
        "filter": ["all", ["==", ["get", "structure"], "bridge"],
            ["match", ["get", "class"],
                ["street", "street_limited", "primary_link"], true, false
            ],
            ["==", ["geometry-type"], "LineString"]
        ],
        "paint": {
            "line-width": ["interpolate", ["exponential", 1.2],
                ["zoom"], 10, 0.5, 18, 2
            ],
            "line-color": "hsl(200, 43%, 33%)",
            "line-gap-width": ["interpolate", ["exponential", 1.5],
                ["zoom"], 14, 2, 18, 20, 22, 200
            ]
        },
        "metadata": {
            "mapbox:featureComponent": "road-network",
            "mapbox:group": "Road network, bridges"
        }
    }, {
        "id": "bridge-secondary-tertiary-case-navigation",
        "type": "line",
        "source": "composite",
        "source-layer": "road",
        "minzoom": 12,
        "filter": ["all", ["==", ["get", "structure"], "bridge"],
            ["match", ["get", "class"],
                ["secondary", "tertiary"], true, false
            ],
            ["==", ["geometry-type"], "LineString"]
        ],
        "paint": {
            "line-width": ["interpolate", ["exponential", 1.2],
                ["zoom"], 10, 0.75, 18, 2
            ],
            "line-color": "hsl(200, 43%, 33%)",
            "line-gap-width": ["interpolate", ["exponential", 1.5],
                ["zoom"], 10, 0.75, 18, 28, 22, 280
            ]
        },
        "metadata": {
            "mapbox:featureComponent": "road-network",
            "mapbox:group": "Road network, bridges"
        }
    }, {
        "id": "bridge-primary-case-navigation",
        "type": "line",
        "source": "composite",
        "source-layer": "road",
        "minzoom": 12,
        "filter": ["all", ["==", ["get", "structure"], "bridge"],
            ["==", ["get", "class"], "primary"],
            ["==", ["geometry-type"], "LineString"]
        ],
        "paint": {
            "line-width": ["interpolate", ["exponential", 1.2],
                ["zoom"], 10, 1, 18, 2
            ],
            "line-color": "hsl(200, 43%, 33%)",
            "line-gap-width": ["interpolate", ["exponential", 1.5],
                ["zoom"], 10, 1.125, 18, 32, 22, 200
            ]
        },
        "metadata": {
            "mapbox:featureComponent": "road-network",
            "mapbox:group": "Road network, bridges"
        }
    }, {
        "id": "bridge-major-link-case-navigation",
        "type": "line",
        "source": "composite",
        "source-layer": "road",
        "minzoom": 13,
        "filter": ["all", ["==", ["get", "structure"], "bridge"],
            ["match", ["get", "class"],
                ["motorway_link", "trunk_link"], true, false
            ],
            ["<=", ["get", "layer"], 1],
            ["==", ["geometry-type"], "LineString"]
        ],
        "paint": {
            "line-width": ["interpolate", ["exponential", 1.2],
                ["zoom"], 10, 0.75, 18, 2
            ],
            "line-color": "hsl(200, 43%, 33%)",
            "line-gap-width": ["interpolate", ["exponential", 1.5],
                ["zoom"], 12, 0.75, 14, 2, 18, 20, 22, 200
            ]
        },
        "metadata": {
            "mapbox:featureComponent": "road-network",
            "mapbox:group": "Road network, bridges"
        }
    }, {
        "id": "bridge-motorway-trunk-case-navigation",
        "type": "line",
        "source": "composite",
        "source-layer": "road",
        "minzoom": 13,
        "filter": ["all", ["==", ["get", "structure"], "bridge"],
            ["match", ["get", "class"],
                ["motorway", "trunk"], true, false
            ],
            ["<=", ["get", "layer"], 1],
            ["==", ["geometry-type"], "LineString"]
        ],
        "paint": {
            "line-width": ["interpolate", ["exponential", 1.2],
                ["zoom"], 10, 1, 18, 2
            ],
            "line-color": "hsl(200, 43%, 33%)",
            "line-gap-width": ["interpolate", ["exponential", 1.5],
                ["zoom"], 5, 1.25, 18, 32, 22, 320
            ]
        },
        "metadata": {
            "mapbox:featureComponent": "road-network",
            "mapbox:group": "Road network, bridges"
        }
    }, {
        "id": "bridge-construction-navigation",
        "type": "line",
        "source": "composite",
        "source-layer": "road",
        "minzoom": 14,
        "filter": ["all", ["==", ["get", "structure"], "bridge"],
            ["==", ["get", "class"], "construction"],
            ["==", ["geometry-type"], "LineString"]
        ],
        "paint": {
            "line-width": ["interpolate", ["exponential", 1.5],
                ["zoom"], 14, 2, 18, 20, 22, 200
            ],
            "line-color": "hsl(200, 49%, 44%)",
            "line-dasharray": ["step", ["zoom"],
                ["literal", [0.4, 0.8]], 15, ["literal", [0.3, 0.6]], 16, ["literal", [0.2, 0.3]], 17, ["literal", [0.2, 0.25]], 18, ["literal", [0.15, 0.15]]
            ]
        },
        "metadata": {
            "mapbox:featureComponent": "road-network",
            "mapbox:group": "Road network, bridges"
        }
    }, {
        "minzoom": 15,
        "layout": {
            "line-cap": "round"
        },
        "metadata": {
            "mapbox:featureComponent": "road-network",
            "mapbox:group": "Road network, bridges"
        },
        "filter": ["all", ["==", ["get", "structure"], "bridge"],
            ["match", ["get", "class"],
                ["track", "secondary_link", "tertiary_link", "service"], true, false
            ],
            ["==", ["geometry-type"], "LineString"]
        ],
        "type": "line",
        "source": "composite",
        "id": "bridge-minor-navigation",
        "paint": {
            "line-width": ["interpolate", ["exponential", 1.5],
                ["zoom"], 15, ["match", ["get", "class"], "track", 1, 0.5], 18, 10, 22, 100
            ],
            "line-color": "hsl(200, 49%, 44%)"
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
            ["match", ["get", "class"],
                ["motorway_link", "trunk_link"], true, false
            ],
            ["<=", ["get", "layer"], 1],
            ["==", ["geometry-type"], "LineString"]
        ],
        "type": "line",
        "source": "composite",
        "id": "bridge-major-link-navigation",
        "paint": {
            "line-width": ["interpolate", ["exponential", 1.5],
                ["zoom"], 12, 0.75, 14, 2, 18, 20, 22, 200
            ],
            "line-color": "hsl(200, 49%, 44%)"
        },
        "source-layer": "road"
    }, {
        "minzoom": 14,
        "layout": {
            "line-cap": "round"
        },
        "metadata": {
            "mapbox:featureComponent": "road-network",
            "mapbox:group": "Road network, bridges"
        },
        "filter": ["all", ["==", ["get", "structure"], "bridge"],
            ["match", ["get", "class"],
                ["street", "street_limited", "primary_link"], true, false
            ],
            ["==", ["geometry-type"], "LineString"]
        ],
        "type": "line",
        "source": "composite",
        "id": "bridge-street-navigation",
        "paint": {
            "line-width": ["interpolate", ["exponential", 1.5],
                ["zoom"], 12, 0.5, 14, 2, 18, 20, 22, 200
            ],
            "line-color": "hsl(200, 49%, 44%)"
        },
        "source-layer": "road"
    }, {
        "minzoom": 13,
        "metadata": {
            "mapbox:featureComponent": "road-network",
            "mapbox:group": "Road network, bridges"
        },
        "maxzoom": 14,
        "filter": ["all", ["==", ["get", "structure"], "bridge"],
            ["match", ["get", "class"],
                ["street", "street_limited", "primary_link"], true, false
            ],
            ["==", ["geometry-type"], "LineString"]
        ],
        "type": "line",
        "source": "composite",
        "id": "bridge-street-low-navigation",
        "paint": {
            "line-width": ["interpolate", ["exponential", 1.5],
                ["zoom"], 12, 0.5, 14, 2
            ],
            "line-color": "hsl(200, 49%, 44%)"
        },
        "source-layer": "road"
    }, {
        "minzoom": 13,
        "layout": {
            "line-cap": ["step", ["zoom"], "butt", 11, "round"]
        },
        "metadata": {
            "mapbox:featureComponent": "road-network",
            "mapbox:group": "Road network, bridges"
        },
        "filter": ["all", ["==", ["get", "structure"], "bridge"],
            ["match", ["get", "class"],
                ["secondary", "tertiary"], true, false
            ],
            ["==", ["geometry-type"], "LineString"]
        ],
        "type": "line",
        "source": "composite",
        "id": "bridge-secondary-tertiary-navigation",
        "paint": {
            "line-width": ["interpolate", ["exponential", 1.5],
                ["zoom"], 10, 0.75, 18, 28, 22, 280
            ],
            "line-color": "hsl(200, 49%, 44%)"
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
            ["==", ["get", "class"], "primary"],
            ["==", ["geometry-type"], "LineString"]
        ],
        "type": "line",
        "source": "composite",
        "id": "bridge-primary-navigation",
        "paint": {
            "line-width": ["interpolate", ["exponential", 1.5],
                ["zoom"], 10, 1.125, 18, 32, 22, 200
            ],
            "line-color": "hsl(200, 49%, 44%)"
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
            ["match", ["get", "class"],
                ["motorway", "trunk"], true, false
            ],
            ["<=", ["get", "layer"], 1],
            ["==", ["geometry-type"], "LineString"]
        ],
        "type": "line",
        "source": "composite",
        "id": "bridge-motorway-trunk-navigation",
        "paint": {
            "line-width": ["interpolate", ["exponential", 1.5],
                ["zoom"], 3, 0.75, 18, 32, 22, 320
            ],
            "line-color": "hsl(200, 49%, 44%)"
        },
        "source-layer": "road"
    }, {
        "id": "bridge-major-link-2-case-navigation",
        "type": "line",
        "source": "composite",
        "source-layer": "road",
        "minzoom": 13,
        "filter": ["all", ["==", ["get", "structure"], "bridge"],
            [">=", ["get", "layer"], 2],
            ["match", ["get", "class"],
                ["motorway_link", "trunk_link"], true, false
            ],
            ["==", ["geometry-type"], "LineString"]
        ],
        "paint": {
            "line-width": ["interpolate", ["exponential", 1.2],
                ["zoom"], 10, 0.75, 18, 2
            ],
            "line-color": "hsl(200, 43%, 33%)",
            "line-gap-width": ["interpolate", ["exponential", 1.5],
                ["zoom"], 12, 0.75, 14, 2, 18, 20, 22, 200
            ]
        },
        "metadata": {
            "mapbox:featureComponent": "road-network",
            "mapbox:group": "Road network, bridges"
        }
    }, {
        "id": "bridge-motorway-trunk-2-case-navigation",
        "type": "line",
        "source": "composite",
        "source-layer": "road",
        "minzoom": 13,
        "filter": ["all", ["==", ["get", "structure"], "bridge"],
            [">=", ["get", "layer"], 2],
            ["match", ["get", "class"],
                ["motorway", "trunk"], true, false
            ],
            ["==", ["geometry-type"], "LineString"]
        ],
        "paint": {
            "line-width": ["interpolate", ["exponential", 1.2],
                ["zoom"], 10, 1, 18, 2
            ],
            "line-color": "hsl(200, 43%, 33%)",
            "line-gap-width": ["interpolate", ["exponential", 1.5],
                ["zoom"], 5, 1.25, 18, 32, 22, 320
            ]
        },
        "metadata": {
            "mapbox:featureComponent": "road-network",
            "mapbox:group": "Road network, bridges"
        }
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
            [">=", ["get", "layer"], 2],
            ["match", ["get", "class"],
                ["motorway_link", "trunk_link"], true, false
            ],
            ["==", ["geometry-type"], "LineString"]
        ],
        "type": "line",
        "source": "composite",
        "id": "bridge-major-link-2-navigation",
        "paint": {
            "line-width": ["interpolate", ["exponential", 1.5],
                ["zoom"], 12, 0.75, 14, 2, 18, 20, 22, 200
            ],
            "line-color": "hsl(200, 49%, 44%)"
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
            [">=", ["get", "layer"], 2],
            ["match", ["get", "class"],
                ["motorway", "trunk"], true, false
            ],
            ["==", ["geometry-type"], "LineString"]
        ],
        "type": "line",
        "source": "composite",
        "id": "bridge-motorway-trunk-2-navigation",
        "paint": {
            "line-width": ["interpolate", ["exponential", 1.5],
                ["zoom"], 5, 1.25, 18, 32, 22, 320
            ],
            "line-color": "hsl(200, 49%, 44%)"
        },
        "source-layer": "road"
    }, {
        "minzoom": 16,
        "layout": {
            "symbol-placement": "line",
            "symbol-spacing": ["interpolate", ["linear"],
                ["zoom"], 15, 400, 18, 600, 22, 1200
            ],
            "icon-image": "oneway-small",
            "icon-size": ["interpolate", ["exponential", 1.5],
                ["zoom"], 15, ["match", ["get", "class"],
                    ["track", "secondary_link", "tertiary_link", "service"], 0.15, 0.25
                ], 18, ["match", ["get", "class"],
                    ["primary", "secondary", "tertiary"], 0.7, ["street", "street_limited", "primary_link"], 0.5, 0.4
                ]
            ],
            "icon-rotation-alignment": "map",
            "icon-allow-overlap": true,
            "icon-ignore-placement": true
        },
        "metadata": {
            "mapbox:featureComponent": "road-network",
            "mapbox:group": "Road network, bridges"
        },
        "filter": ["all", ["==", ["get", "structure"], "bridge"],
            ["==", ["get", "oneway"], "true"],
            ["step", ["zoom"],
                ["match", ["get", "class"],
                    ["primary", "secondary", "tertiary", "street", "street_limited"], true, false
                ], 16, ["match", ["get", "class"],
                    ["primary", "secondary", "tertiary", "street", "street_limited", "primary_link", "secondary_link", "tertiary_link", "service", "track"], true, false
                ]
            ],
            ["step", ["pitch"], true, 50, ["<", ["distance-from-center"], 1], 60, ["<", ["distance-from-center"], 1.5], 70, ["<", ["distance-from-center"], 2]]
        ],
        "type": "symbol",
        "source": "composite",
        "id": "bridge-oneway-arrow-blue-navigation",
        "paint": {},
        "source-layer": "road"
    }, {
        "minzoom": 16,
        "layout": {
            "symbol-placement": "line",
            "symbol-spacing": ["interpolate", ["linear"],
                ["zoom"], 15, 400, 18, 600, 22, 1200
            ],
            "icon-image": ["step", ["zoom"], "oneway-white-small", 18, "oneway-white-large"],
            "icon-size": ["interpolate", ["exponential", 1.5],
                ["zoom"], 15, ["match", ["get", "class"],
                    ["motorway", "trunk"], 0.45, 0.3
                ], 20, ["match", ["get", "class"],
                    ["motorway", "trunk"], 0.6, 0.4
                ]
            ],
            "icon-rotation-alignment": "map",
            "icon-allow-overlap": true,
            "icon-ignore-placement": true
        },
        "metadata": {
            "mapbox:featureComponent": "road-network",
            "mapbox:group": "Road network, bridges"
        },
        "filter": ["all", ["==", ["get", "structure"], "bridge"],
            ["match", ["get", "class"],
                ["motorway", "trunk", "motorway_link", "trunk_link"], true, false
            ],
            ["==", ["get", "oneway"], "true"],
            ["step", ["pitch"], true, 50, ["<", ["distance-from-center"], 1], 60, ["<", ["distance-from-center"], 1.5], 70, ["<", ["distance-from-center"], 2]]
        ],
        "type": "symbol",
        "source": "composite",
        "id": "bridge-oneway-arrow-white-navigation",
        "paint": {},
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
            "line-color": "hsl(200, 44%, 30%)",
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
            "line-color": "hsl(200, 44%, 30%)",
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
            "fill-extrusion-color": "hsl(200, 49%, 30%)",
            "fill-extrusion-height": ["get", "height"],
            "fill-extrusion-base": ["get", "min_height"],
            "fill-extrusion-opacity": ["interpolate", ["linear"],
                ["zoom"], 15, 0, 16, 1
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
            "line-color": "hsl(200, 44%, 30%)",
            "line-width": ["interpolate", ["linear"],
                ["zoom"], 3, 4.199999999999999, 12, 8.399999999999999
            ],
            "line-opacity": ["interpolate", ["linear"],
                ["zoom"], 7, 0, 8, 0.5
            ],
            "line-dasharray": [1, 0],
            "line-blur": ["interpolate", ["linear"],
                ["zoom"], 3, 0, 12, 4.199999999999999
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
            "line-color": "hsl(200, 44%, 30%)",
            "line-width": ["interpolate", ["linear"],
                ["zoom"], 3, 4.5, 12, 9
            ],
            "line-opacity": ["interpolate", ["linear"],
                ["zoom"], 7, 0, 8, 0.5
            ],
            "line-dasharray": [1, 0],
            "line-blur": ["interpolate", ["linear"],
                ["zoom"], 3, 0, 12, 4.5
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
        "filter": ["all", ["match", ["get", "disputed"], "true", false, true],
            ["==", ["get", "admin_level"], 0],
            ["==", ["get", "maritime"], "false"],
            ["match", ["get", "worldview"],
                ["all", "CN"], true, false
            ]
        ],
        "paint": {
            "line-width": ["interpolate", ["linear"],
                ["zoom"], 3, 6.8, 12, 13.6
            ],
            "line-color": "hsl(200, 44%, 30%)",
            "line-opacity": ["interpolate", ["linear"],
                ["zoom"], 3, 0, 4, 0.5
            ],
            "line-blur": ["interpolate", ["linear"],
                ["zoom"], 3, 0, 12, 3.4
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
                ["zoom"], 3, 0.42, 12, 1.4
            ],
            "line-opacity": ["interpolate", ["linear"],
                ["zoom"], 5, 0, 6, 1
            ],
            "line-color": "hsl(200, 49%, 59%)"
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
                ["zoom"], 3, 0.44999999999999996, 12, 2.25
            ],
            "line-opacity": ["interpolate", ["linear"],
                ["zoom"], 2, 0, 3, 1
            ],
            "line-color": "hsl(200, 49%, 59%)"
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
            "line-color": "hsl(200, 44%, 59%)",
            "line-width": ["interpolate", ["linear"],
                ["zoom"], 3, 0.85, 12, 3.4
            ],
            "line-dasharray": [10, 0]
        },
        "source-layer": "admin"
    }, {
        "minzoom": 18,
        "layout": {
            "icon-image": "marker",
            "text-field": ["get", "ref"],
            "text-size": 10,
            "text-offset": [0, -0.5],
            "text-font": ["DIN Pro Italic", "Arial Unicode MS Regular"]
        },
        "metadata": {
            "mapbox:featureComponent": "buildings",
            "mapbox:group": "Buildings, building-labels"
        },
        "filter": ["all", ["==", ["get", "class"], "entrance"],
            ["step", ["pitch"], true, 50, ["<", ["distance-from-center"], 1], 60, ["<", ["distance-from-center"], 1.5], 70, ["<", ["distance-from-center"], 2]]
        ],
        "type": "symbol",
        "source": "composite",
        "id": "building-entrance",
        "paint": {
            "text-color": "hsl(200, 38%, 73%)",
            "text-halo-color": "hsl(200, 43%, 39%)",
            "text-halo-width": 1,
            "icon-opacity": 0.4
        },
        "source-layer": "structure"
    }, {
        "minzoom": 17,
        "layout": {
            "text-field": ["get", "house_num"],
            "text-font": ["DIN Pro Italic", "Arial Unicode MS Regular"],
            "text-padding": 4,
            "text-max-width": 7,
            "text-size": 10
        },
        "metadata": {
            "mapbox:featureComponent": "buildings",
            "mapbox:group": "Buildings, building-labels"
        },
        "filter": ["step", ["pitch"], true, 50, ["<", ["distance-from-center"], 1], 60, ["<", ["distance-from-center"], 1.5], 70, ["<", ["distance-from-center"], 2]],
        "type": "symbol",
        "source": "composite",
        "id": "building-number-label",
        "paint": {
            "text-color": "hsl(200, 38%, 73%)",
            "text-halo-color": "hsl(200, 43%, 39%)",
            "text-halo-width": 1
        },
        "source-layer": "housenum_label"
    }, {
        "minzoom": 16,
        "layout": {
            "text-field": ["get", "name"],
            "text-font": ["DIN Pro Italic", "Arial Unicode MS Regular"],
            "text-max-width": 7,
            "text-size": 11
        },
        "metadata": {
            "mapbox:featureComponent": "buildings",
            "mapbox:group": "Buildings, building-labels"
        },
        "filter": ["all", ["==", ["get", "class"], "settlement_subdivision"],
            ["==", ["get", "type"], "block"],
            ["step", ["pitch"], true, 50, ["<", ["distance-from-center"], 1], 60, ["<", ["distance-from-center"], 1.5], 70, ["<", ["distance-from-center"], 2]]
        ],
        "type": "symbol",
        "source": "composite",
        "id": "block-number-label",
        "paint": {
            "text-color": "hsl(200, 49%, 74%)",
            "text-halo-color": "hsl(200, 48%, 27%)",
            "text-halo-width": 0.5,
            "text-halo-blur": 0.5
        },
        "source-layer": "place_label"
    }, {
        "minzoom": 15,
        "layout": {
            "text-field": ["coalesce", ["get", "name_zh-Hans"],
                ["get", "name"]
            ],
            "icon-image": "intersection",
            "icon-text-fit": "both",
            "icon-text-fit-padding": [1, 2, 1, 2],
            "text-size": 12,
            "text-font": ["DIN Pro Bold", "Arial Unicode MS Bold"]
        },
        "metadata": {
            "mapbox:featureComponent": "road-network",
            "mapbox:group": "Road network, road-labels"
        },
        "filter": ["all", ["==", ["get", "class"], "intersection"],
            ["has", "name"],
            ["step", ["pitch"], true, 50, ["<", ["distance-from-center"], 1], 60, ["<", ["distance-from-center"], 1.5], 70, ["<", ["distance-from-center"], 2]]
        ],
        "type": "symbol",
        "source": "composite",
        "id": "road-intersection",
        "paint": {
            "text-color": "hsl(230, 57%, 64%)"
        },
        "source-layer": "road"
    }, {
        "minzoom": 17,
        "layout": {
            "icon-image": "traffic-signal",
            "icon-size": ["interpolate", ["exponential", 1.5],
                ["zoom"], 15, 0.4, 20, 1
            ],
            "icon-allow-overlap": true,
            "symbol-z-elevate": true
        },
        "metadata": {
            "mapbox:featureComponent": "road-network",
            "mapbox:group": "Road network, road-labels"
        },
        "filter": ["all", ["==", ["get", "class"], "traffic_signals"],
            ["step", ["pitch"], true, 50, ["<", ["distance-from-center"], 1], 60, ["<", ["distance-from-center"], 1.5], 70, ["<", ["distance-from-center"], 2]]
        ],
        "type": "symbol",
        "source": "composite",
        "id": "traffic-signal-navigation",
        "paint": {},
        "source-layer": "road"
    }, {
        "minzoom": 13,
        "layout": {
            "text-size": ["interpolate", ["linear"],
                ["zoom"], 10, ["match", ["get", "class"],
                    ["motorway", "trunk", "primary", "secondary", "tertiary"], 10, ["motorway_link", "trunk_link", "primary_link", "secondary_link", "tertiary_link", "street", "street_limited"], 8, 6.5
                ], 18, ["match", ["get", "class"],
                    ["motorway", "trunk", "primary", "secondary", "tertiary"], 16, ["motorway_link", "trunk_link", "primary_link", "secondary_link", "tertiary_link", "street", "street_limited"], 14, 13
                ], 22, ["match", ["get", "class"],
                    ["motorway", "trunk", "primary", "secondary", "tertiary"], 50, ["motorway_link", "trunk_link", "primary_link", "secondary_link", "tertiary_link", "street", "street_limited"], 40, 30
                ]
            ],
            "text-max-angle": 30,
            "symbol-spacing": 200,
            "text-font": ["DIN Pro Regular", "Arial Unicode MS Regular"],
            "symbol-placement": "line",
            "text-padding": 4,
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
            ["step", ["zoom"],
                ["match", ["get", "class"],
                    ["motorway", "trunk", "primary", "secondary", "tertiary"], true, false
                ], 15, ["match", ["get", "class"],
                    ["motorway", "trunk", "primary", "secondary", "tertiary", "street"], true, false
                ], 16, ["match", ["get", "class"],
                    ["motorway", "trunk", "primary", "secondary", "tertiary", "street", "street_limited"], true, false
                ], 17, ["match", ["get", "class"],
                    ["pedestrian", "golf", "ferry", "aerialway", "path"], false, true
                ]
            ],
            ["step", ["pitch"], true, 50, ["<", ["distance-from-center"], 2], 60, ["<", ["distance-from-center"], 2.5], 70, ["<", ["distance-from-center"], 3]]
        ],
        "type": "symbol",
        "source": "composite",
        "id": "road-label-navigation",
        "paint": {
            "text-color": "hsl(200, 49%, 87%)",
            "text-halo-color": "hsl(200, 48%, 27%)",
            "text-halo-width": 1,
            "text-halo-blur": 1
        },
        "source-layer": "road"
    }, {
        "minzoom": 6,
        "layout": {
            "text-size": ["interpolate", ["exponential", 1.5],
                ["zoom"], 6, 14, 22, 26
            ],
            "icon-image": ["case", ["has", "shield_beta"],
                ["coalesce", ["image", ["concat", ["get", "shield_beta"], "-", ["to-string", ["get", "reflen"]]]],
                    ["image", ["concat", ["get", "shield"], "-", ["to-string", ["get", "reflen"]]]],
                    ["image", ["concat", "default-", ["to-string", ["get", "reflen"]]]]
                ],
                ["concat", ["get", "shield"], "-", ["to-string", ["get", "reflen"]]]
            ],
            "icon-rotation-alignment": "viewport",
            "text-max-angle": 38,
            "symbol-spacing": ["interpolate", ["linear"],
                ["zoom"], 11, 400, 14, 600, 16, 800, 22, 1200
            ],
            "text-font": ["DIN Pro Bold", "Arial Unicode MS Bold"],
            "symbol-placement": ["step", ["zoom"], "point", 11, "line"],
            "text-rotation-alignment": "viewport",
            "icon-size": ["interpolate", ["exponential", 1.5],
                ["zoom"], 6, 0.5, 13, 0.5, 22, 1
            ],
            "text-field": ["get", "ref"],
            "text-letter-spacing": 0.05
        },
        "metadata": {
            "mapbox:featureComponent": "road-network",
            "mapbox:group": "Road network, road-labels"
        },
        "filter": ["all", ["case", ["has", "reflen"],
                ["<=", ["get", "reflen"], 6],
                ["has", "shield_beta"]
            ],
            ["match", ["get", "class"],
                ["pedestrian", "service"], false, true
            ],
            ["step", ["zoom"],
                ["==", ["geometry-type"], "Point"], 11, [">", ["get", "len"], 5000], 12, [">", ["get", "len"], 2500], 13, [">", ["get", "len"], 1000], 14, true
            ],
            ["step", ["pitch"], true, 50, ["<", ["distance-from-center"], 2], 60, ["<", ["distance-from-center"], 2.5], 70, ["<", ["distance-from-center"], 3]]
        ],
        "type": "symbol",
        "source": "composite",
        "id": "road-number-shield-navigation",
        "paint": {
            "text-color": ["case", ["has", "shield_beta"],
                ["case", ["all", ["has", "shield_text_color_beta"],
                        ["to-boolean", ["coalesce", ["image", ["concat", ["get", "shield_beta"], "-", ["to-string", ["get", "reflen"]]]], ""]]
                    ],
                    ["match", ["get", "shield_text_color_beta"], "white", "hsl(0, 0%, 100%)", "yellow", "hsl(50, 100%, 70%)", "orange", "hsl(25, 100%, 75%)", "blue", "hsl(230, 57%, 44%)", "red", "hsl(0, 87%, 59%)", "green", "hsl(140, 74%, 37%)", "hsl(230, 18%, 13%)"], "hsl(230, 18%, 13%)"
                ],
                ["match", ["get", "shield_text_color"], "white", "hsl(0, 0%, 100%)", "yellow", "hsl(50, 100%, 70%)", "orange", "hsl(25, 100%, 75%)", "blue", "hsl(230, 57%, 44%)", "red", "hsl(0, 87%, 59%)", "green", "hsl(140, 74%, 37%)", "hsl(230, 18%, 13%)"]
            ]
        },
        "source-layer": "road"
    }, {
        "minzoom": 14,
        "layout": {
            "text-field": ["get", "ref"],
            "text-size": ["interpolate", ["exponential", 1.5],
                ["zoom"], 6, 14, 22, 26
            ],
            "text-font": ["DIN Pro Bold", "Arial Unicode MS Bold"],
            "icon-image": ["concat", "motorway-exit-", ["to-string", ["get", "reflen"]]],
            "icon-size": ["interpolate", ["exponential", 1.5],
                ["zoom"], 6, 0.5, 13, 0.5, 22, 1
            ]
        },
        "metadata": {
            "mapbox:featureComponent": "road-network",
            "mapbox:group": "Road network, road-labels"
        },
        "filter": ["all", ["has", "reflen"],
            ["<=", ["get", "reflen"], 9],
            ["step", ["pitch"], true, 50, ["<", ["distance-from-center"], 1], 60, ["<", ["distance-from-center"], 1.5], 70, ["<", ["distance-from-center"], 2]]
        ],
        "type": "symbol",
        "source": "composite",
        "id": "road-exit-shield-navigation",
        "paint": {
            "text-color": "hsl(0, 0%, 100%)"
        },
        "source-layer": "motorway_junction"
    }, {
        "minzoom": 12,
        "layout": {
            "text-size": ["interpolate", ["linear"],
                ["zoom"], 10, ["match", ["get", "class"], "pedestrian", 9, 6.5], 18, ["match", ["get", "class"], "pedestrian", 14, 13]
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
            "text-color": "hsl(200, 49%, 87%)",
            "text-halo-color": "hsl(200, 48%, 27%)",
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
            "text-field": ["coalesce", ["get", "name_en"],
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
                    ["all", "US"], true, false
                ], false
            ],
            ["step", ["pitch"], true, 50, ["<", ["distance-from-center"], 1], 60, ["<", ["distance-from-center"], 1.5], 70, ["<", ["distance-from-center"], 2]],
            ["==", ["geometry-type"], "LineString"]
        ],
        "type": "symbol",
        "source": "composite",
        "id": "waterway-label",
        "paint": {
            "text-color": "hsl(200, 48%, 55%)",
            "text-halo-color": "hsla(200, 48%, 27%, 0.5)"
        },
        "source-layer": "natural_label"
    }, {
        "minzoom": 4,
        "layout": {
            "text-size": ["step", ["zoom"],
                ["step", ["get", "sizerank"], 18, 5, 12], 17, ["step", ["get", "sizerank"], 18, 13, 12]
            ],
            "text-max-angle": 30,
            "text-field": ["coalesce", ["get", "name_en"],
                ["get", "name"]
            ],
            "text-font": ["DIN Pro Medium", "Arial Unicode MS Regular"],
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
                    ["all", "US"], true, false
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
            "text-halo-color": "hsl(200, 48%, 27%)",
            "text-halo-blur": 0.5,
            "text-color": "hsl(200, 49%, 74%)"
        },
        "source-layer": "natural_label"
    }, {
        "minzoom": 4,
        "layout": {
            "text-size": ["step", ["zoom"],
                ["step", ["get", "sizerank"], 18, 5, 12], 17, ["step", ["get", "sizerank"], 18, 13, 12]
            ],
            "icon-image": "",
            "text-font": ["DIN Pro Medium", "Arial Unicode MS Regular"],
            "text-offset": ["literal", [0, 0]],
            "text-field": ["coalesce", ["get", "name_en"],
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
                    ["all", "US"], true, false
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
            "text-halo-color": "hsl(200, 48%, 27%)",
            "text-halo-width": 0.5,
            "text-halo-blur": 0.5,
            "text-color": "hsl(200, 49%, 74%)"
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
            "text-field": ["coalesce", ["get", "name_en"],
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
                    ["all", "US"], true, false
                ], false
            ],
            ["step", ["pitch"], true, 50, ["<", ["distance-from-center"], 1], 60, ["<", ["distance-from-center"], 1.5], 70, ["<", ["distance-from-center"], 2]],
            ["==", ["geometry-type"], "LineString"]
        ],
        "type": "symbol",
        "source": "composite",
        "id": "water-line-label",
        "paint": {
            "text-color": "hsl(200, 48%, 55%)",
            "text-halo-color": "hsla(200, 48%, 27%, 0.5)"
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
            "text-field": ["coalesce", ["get", "name_en"],
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
                    ["all", "US"], true, false
                ], false
            ],
            ["step", ["pitch"], true, 50, ["<", ["distance-from-center"], 1], 60, ["<", ["distance-from-center"], 1.5], 70, ["<", ["distance-from-center"], 2]],
            ["==", ["geometry-type"], "Point"]
        ],
        "type": "symbol",
        "source": "composite",
        "id": "water-point-label",
        "paint": {
            "text-color": "hsl(200, 48%, 55%)",
            "text-halo-color": "hsla(200, 48%, 27%, 0.5)"
        },
        "source-layer": "natural_label"
    }, {
        "minzoom": 6,
        "layout": {
            "text-size": ["step", ["zoom"],
                ["step", ["get", "sizerank"], 18, 5, 12], 17, ["step", ["get", "sizerank"], 18, 13, 12]
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
                    ["literal", [0, 0]], 5, ["literal", [0, 1.35]]
                ], 17, ["step", ["get", "sizerank"],
                    ["literal", [0, 0]], 13, ["literal", [0, 1.35]]
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
                ["+", ["step", ["zoom"], 0, 16, 1, 17, 2], 1]
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
            "text-halo-color": "hsl(200, 48%, 27%)",
            "text-halo-width": 0.5,
            "text-halo-blur": 0.5,
            "text-color": "hsl(200, 49%, 80%)"
        },
        "source-layer": "poi_label"
    }, {
        "minzoom": 12,
        "layout": {
            "text-size": 12,
            "icon-image": ["get", "network"],
            "text-font": ["DIN Pro Medium", "Arial Unicode MS Regular"],
            "text-justify": ["match", ["get", "stop_type"], "entrance", "left", "center"],
            "text-offset": ["match", ["get", "stop_type"], "entrance", ["literal", [1, 0]],
                ["literal", [0, 1]]
            ],
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
                ], 16, ["all", ["match", ["get", "mode"], "rail", true, "metro_rail", true, "light_rail", true, false],
                    ["!=", ["get", "stop_type"], "entrance"]
                ], 17, ["all", ["match", ["get", "mode"], "rail", true, "metro_rail", true, "light_rail", true, "bus", true, false],
                    ["!=", ["get", "stop_type"], "entrance"]
                ], 19, ["match", ["get", "mode"], "rail", true, "metro_rail", true, "light_rail", true, "bus", true, false]
            ],
            ["step", ["pitch"], true, 50, ["<", ["distance-from-center"], 1], 60, ["<", ["distance-from-center"], 1.5], 70, ["<", ["distance-from-center"], 2]]
        ],
        "type": "symbol",
        "source": "composite",
        "id": "transit-label",
        "paint": {
            "text-halo-color": "hsl(200, 48%, 27%)",
            "text-color": "hsl(200, 49%, 87%)",
            "text-halo-blur": 0.5,
            "text-halo-width": 0.5
        },
        "source-layer": "transit_stop_label"
    }, {
        "minzoom": 8,
        "layout": {
            "text-line-height": 1.1,
            "text-size": ["step", ["get", "sizerank"], 18, 9, 12],
            "icon-image": ["get", "maki"],
            "text-font": ["DIN Pro Medium", "Arial Unicode MS Regular"],
            "text-offset": [0, 1],
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
            "text-color": "hsl(200, 49%, 87%)",
            "text-halo-color": "hsl(200, 48%, 27%)",
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
                ["zoom"], 11, ["match", ["get", "type"], "suburb", 11, 10.5], 15, ["match", ["get", "type"], "suburb", 15, 14]
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
            "text-halo-color": "hsl(200, 48%, 27%)",
            "text-halo-width": 1,
            "text-color": "hsl(200, 49%, 75%)",
            "text-halo-blur": 0.5
        },
        "source-layer": "place_label"
    }, {
        "minzoom": 2,
        "layout": {
            "text-line-height": 1.1,
            "text-size": ["interpolate", ["cubic-bezier", 0.2, 0, 0.9, 1],
                ["zoom"], 3, ["step", ["get", "symbolrank"], 11, 9, 10], 6, ["step", ["get", "symbolrank"], 14, 9, 12, 12, 10], 8, ["step", ["get", "symbolrank"], 16, 9, 14, 12, 12, 15, 10], 13, ["step", ["get", "symbolrank"], 22, 9, 20, 12, 16, 15, 14]
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
        "filter": ["all", ["<=", ["get", "filterrank"], 2],
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
            "text-color": ["step", ["get", "symbolrank"], "hsl(200, 49%, 87%)", 11, "hsl(200, 49%, 74%)", 16, "hsl(200, 49%, 68%)"],
            "text-halo-color": "hsl(200, 48%, 27%)",
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
                ["zoom"], 3, ["step", ["get", "symbolrank"], 13, 6, 11], 6, ["step", ["get", "symbolrank"], 18, 6, 16, 7, 14], 8, ["step", ["get", "symbolrank"], 20, 9, 16, 10, 14], 15, ["step", ["get", "symbolrank"], 24, 9, 20, 12, 16, 15, 14]
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
        "filter": ["all", ["<=", ["get", "filterrank"], 2],
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
            "text-color": ["step", ["get", "symbolrank"], "hsl(200, 49%, 87%)", 11, "hsl(200, 49%, 74%)", 16, "hsl(200, 49%, 68%)"],
            "text-halo-color": "hsl(200, 48%, 27%)",
            "text-halo-width": 1,
            "icon-opacity": ["step", ["zoom"], 1, 8, 0],
            "text-halo-blur": 1
        },
        "source-layer": "place_label"
    }, {
        "minzoom": 3,
        "layout": {
            "text-size": ["interpolate", ["cubic-bezier", 0.85, 0.7, 0.65, 1],
                ["zoom"], 4, ["step", ["get", "symbolrank"], 9, 6, 8, 7, 7], 9, ["step", ["get", "symbolrank"], 21, 6, 16, 7, 14]
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
            "text-color": "hsl(200, 49%, 87%)",
            "text-halo-color": "hsl(200, 48%, 27%)",
            "text-halo-width": 1,
            "text-opacity": 0.5
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
                ["zoom"], 1, ["step", ["get", "symbolrank"], 11, 4, 9, 5, 8], 9, ["step", ["get", "symbolrank"], 22, 4, 19, 5, 17]
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
            "text-color": "hsl(200, 49%, 59%)",
            "text-halo-color": "hsl(200, 48%, 27%)",
            "text-halo-width": 1.25
        },
        "source-layer": "place_label"
    }, {
        "minzoom": 0.75,
        "layout": {
            "text-field": ["coalesce", ["get", "name_zh-Hans"],
                ["get", "name"]
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
            "text-color": "hsl(200, 49%, 59%)",
            "text-halo-color": "hsl(200, 48%, 27%)",
            "text-halo-width": 1.5,
            "text-opacity": ["interpolate", ["linear"],
                ["zoom"], 0, 0.8, 1.5, 0.5, 2.5, 0
            ]
        },
        "source-layer": "natural_label"
    }],
    "created": "2024-06-27T08:52:13.664Z",
    "modified": "2024-07-01T12:48:54.563Z",
    "id": "clxx105i700yr01po4zbn2jc1",
    "owner": "navworks",
    "visibility": "private",
    "protected": false,
    "draft": false
}
 
*/

#endif /* nav_mb_json_h */
