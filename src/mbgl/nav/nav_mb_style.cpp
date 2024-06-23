//
//  nav_mb_style.cpp
//
//  Created by BernieZhao on 2023/12/13.
//

#include "mbgl/nav/nav_mb_style.hpp"
#include "mbgl/nav/nav_mb_layer.hpp"

#include <mbgl/style/layers/background_layer.hpp>
#include <mbgl/style/layers/fill_layer.hpp>
#include <mbgl/style/layers/line_layer.hpp>
#include <mbgl/style/layers/fill_extrusion_layer.hpp>
#include <mbgl/util/io.hpp>

#include "mbgl/nav/nav_log.hpp"

namespace nav {
namespace style {

const std::string& url() {
    static std::string url(
                           
                           // 主题模式
//                           "mapbox://styles/notheorem/clxonzrtz00k701qq00fj6rnh" // dark
//                           "mapbox://styles/notheorem/clxrac32800o901qw94ryfkdz" // light

                           // 色彩模式
//                           "mapbox://styles/notheorem/clxo7gf6000l301pf067zcqu9" // 蓝
//                           "mapbox://styles/notheorem/clxk5j3qw00av01qq93udh6bm" // 暗蓝
//                           "mapbox://styles/notheorem/clxo72llu00j101qqgeb55xlw" // 紫
//                           "mapbox://styles/notheorem/clxolet6000jx01qq4un34jep" // 红紫
//                           "mapbox://styles/notheorem/clxo6t3l800l501qma3lk7kku" // 玫红
//                           "mapbox://styles/notheorem/clxo70o4q00l701qmekyz04x5" // 绿
//                           "mapbox://styles/notheorem/clxo6ym3j00k501r2dgm7d26j" // 芥末绿
//                           "mapbox://styles/notheorem/clxo77xld004501r0ae6s94fz" // 黄橙
//                           "mapbox://styles/notheorem/clxo6u08m00l601qme55bahbs" // 土黄
                           "mapbox://styles/notheorem/clxo7bjja00kn01qrdm5k6o1a" // 白
//                           "mapbox://styles/notheorem/clxo7dspj00ko01qrhny81uem" // 黑
    );
    
    return url;
}

struct ImageData {
    mbgl::PremultipliedImage image;
    mbgl::optional<mbgl::gfx::Texture> texture { mbgl::nullopt };
};

std::map<std::string, ImageData> imageMap;

void loadAllImage(const std::string& path) {
    imageMap["matcap.0.png"].image = mbgl::decodeImage(mbgl::util::read_file(path + "matcap.0.png"));
    imageMap["matcap.1.png"].image = mbgl::decodeImage(mbgl::util::read_file(path + "matcap.1.png"));
    imageMap["matcap.2.png"].image = mbgl::decodeImage(mbgl::util::read_file(path + "matcap.2.png"));
    imageMap["matcap.3.png"].image = mbgl::decodeImage(mbgl::util::read_file(path + "matcap.3.png"));
    imageMap["matcap.4.png"].image = mbgl::decodeImage(mbgl::util::read_file(path + "matcap.4.png"));
}

void releaseAllImage() {
    imageMap.clear();
}

void uploadTexture(mbgl::gfx::UploadPass& uploadPass) {
    for (auto& i : imageMap) {
        if (!i.second.texture) {
            i.second.texture = uploadPass.createTexture(i.second.image);
        }
    }
}

mbgl::gfx::TextureResource& getTexture(const std::string& name) {
    return imageMap[name].texture->getResource();
}


bool layerHasLineHeight(const std::string& layerId) {
    return layerId.find("bridge") != std::string::npos;
}




namespace rendertime {

struct Time {
    double time_seconds;

    void update() {
        const auto now = std::chrono::steady_clock::now();
        time_seconds = std::chrono::duration_cast<std::chrono::duration<double>>(now.time_since_epoch()).count();
    }
    
    operator double () const { return time_seconds; }
};

Time timestamp;

double value() {
    return timestamp;
}

}

struct ToggleValue {
    bool _enabled = false;
    float _ratio = .5;

    bool update() {
        if (_enabled) {
            if (_ratio < 1.) {
                _ratio = fmin(_ratio + fmax((1. - _ratio) * 0.1, 0.01), 1.);
                return true;
            }
            return false;
        } else {
            if (_ratio > 0.) {
                _ratio = fmax(_ratio - fmax((_ratio - 0.) * 0.025, 0.0025), 0.);
                return true;
            }
            return false;
        }
    }

    void enable() { _enabled = true; }
    void disable() { _enabled = false; }
    operator float () const { return _ratio; }
};

namespace spotlight {

ToggleValue toggle;

float value() {
    return toggle;
}

}

namespace landscape {

ToggleValue toggle;

float value() {
    return toggle;
}

}

void setViewMode(ViewMode mode) {
    switch (mode) {
        case Landscape:
            spotlight::toggle.disable();
            landscape::toggle.enable();
            break;
            
        case Normal:
            spotlight::toggle.disable();
            landscape::toggle.disable();
            break;
            
        case Spotlight:
            spotlight::toggle.enable();
            landscape::toggle.disable();
            break;
            
        default:
            break;
    }
}

namespace palette {

float hue_to_rgb(float p, float q, float t) {
    if (t < 0.) t += 1.;
    if (t > 1.) t -= 1.;
    if (t * 6. < 1.) return p + (q - p) * 6. * t;
    if (t * 2. < 1.) return q;
    if (t * 3. < 2.) return p + (q - p) * (2./3. - t) * 6.;
    return p;
}

float value_smooth_to(float from, float to) {
    if (from > to) {
        return fmax(from - fmax((from - to) * 0.1, 0.02), to);
    } else if (from < to) {
        return fmin(from + fmax((to - from) * 0.1, 0.02), to);
    } else {
        return to;
    }
}

struct Hsl {
    float h=0, s=0, l=0, a=0;

    Hsl() = default;

    Hsl(const mbgl::Color& rgb) {
        const float& r = rgb.r;
        const float& g = rgb.g;
        const float& b = rgb.b;
        
        double maxVal = std::max(r, std::max(g, b));
        double minVal = std::min(r, std::min(g, b));
        double delta = maxVal - minVal;
     
        l = (maxVal + minVal) / 2.0;
        if (delta == 0) {
            h = 0; // 无色彩
            s = 0; // 灰色
        } else {
            if (l < 0.5) {
                s = delta / (maxVal + minVal);
            } else {
                s = delta / (2 - maxVal - minVal);
            }
     
            if (r == maxVal) {
                h = (g - b) / delta; // between 0 and 6
            } else if (g == maxVal) {
                h = 2 + (b - r) / delta; // between 2 and 8
            } else {
                h = 4 + (r - g) / delta; // between 4 and 10
            }
            h *= 60; // Convert to degrees
            if (h < 0) {
                h += 360;
            }
        }
    }

    mbgl::Color rgb() const {
        float r, g, b;
     
        if (s == 0) {
            r = g = b = l; //  achromatic
        } else {
            float q = l < 0.5 ? l * (1. + s) : l + s - l * s;
            float p = 2. * l - q;
            r = hue_to_rgb(p, q, h + 1./3.);
            g = hue_to_rgb(p, q, h);
            b = hue_to_rgb(p, q, h - 1./3.);
        }
     
        return {r, g, b, a};
    }

    void smoothto(const Hsl& to) {
        h = value_smooth_to(h, to.h);
        s = value_smooth_to(s, to.s);
        l = value_smooth_to(l, to.l);
        a = value_smooth_to(a, to.a);
    }
};

struct StylableColor {
    const int8_t hEffect, sEffect, lEffect, aEffect;
    Hsl color;
    
    StylableColor(int8_t h, int8_t s, int8_t l, int8_t a) :
    hEffect(h), sEffect(s), lEffect(l), aEffect(a) { }

    void operator = (const Hsl& base) const {
        
    }
    
    operator const Hsl& () { return color; }
};


class GradientColor {
    Hsl color;
    StylableColor stylableColor;

public:
    GradientColor(const StylableColor& color) : stylableColor(color) {
        
    }
    
    void update() {
        color.smoothto(stylableColor);
    }
    
    void operator = (const Hsl& base) { stylableColor = base; }
    operator const Hsl& () { return color; }
    operator mbgl::Color () { return color.rgb(); }
};



std::map<std::string, GradientColor> palleteMap = {

    { "land/background-color/interpolate/0",                 StylableColor( { 0,0,0,0 } ) },
    { "land/background-color/interpolate/1",                 StylableColor( { 0,0,0,0 } ) },
    
    { "landcover/fill-color",                                StylableColor( { 0,0,0,0 } ) },
    { "national-park/fill-color",                            StylableColor( { 0,0,0,0 } ) },
    { "landuse/fill-color",                                  StylableColor( { 0,0,0,0 } ) },

    { "hillshade/fill-color/interpolate/0/0",                StylableColor( { 0,0,0,0 } ) },
    { "hillshade/fill-color/interpolate/0/1",                StylableColor( { 0,0,0,0 } ) },
    { "hillshade/fill-color/interpolate/1/0",                StylableColor( { 0,0,0,0 } ) },
    { "hillshade/fill-color/interpolate/1/1",                StylableColor( { 0,0,0,0 } ) },
    
    { "waterway/line-color",                                 StylableColor( { 0,0,0,0 } ) },
    { "water/fill-color",                                    StylableColor( { 0,0,0,0 } ) },
    
    { "water-depth/fill-color/interpolate/0/0",              StylableColor( { 0,0,0,0 } ) },
    { "water-depth/fill-color/interpolate/0/1",              StylableColor( { 0,0,0,0 } ) },
    { "water-depth/fill-color/interpolate/0/2",              StylableColor( { 0,0,0,0 } ) },
    { "water-depth/fill-color/interpolate/1/0",              StylableColor( { 0,0,0,0 } ) },
    { "water-depth/fill-color/interpolate/1/1",              StylableColor( { 0,0,0,0 } ) },
    { "water-depth/fill-color/interpolate/1/2",              StylableColor( { 0,0,0,0 } ) },
    
//    { "land-structure-polygon",                              StylableColor( { 0,0,0,0 } ) },
//    { "land-structure-line",                                 StylableColor( { 0,0,0,0 } ) },
    
//    { "aeroway-polygon",                                     StylableColor( { 0,0,0,0 } ) },
//    { "aeroway-line",                                        StylableColor( { 0,0,0,0 } ) },
    
//    { "tunnel-path-trail",                                   StylableColor( { 0,0,0,0 } ) },
//    { "tunnel-path-cycleway-piste",                          StylableColor( { 0,0,0,0 } ) },

    { "tunnel-path/line-color",                              StylableColor( { 0,0,0,0 } ) },
    { "tunnel-steps/line-color",                             StylableColor( { 0,0,0,0 } ) },
    { "tunnel-pedestrian/line-color",                        StylableColor( { 0,0,0,0 } ) },
    
    { "tunnel-minor-case-navigation/line-color",             StylableColor( { 0,0,0,0 } ) },
    { "tunnel-street-case-navigation/line-color",            StylableColor( { 0,0,0,0 } ) },
    { "tunnel-secondary-tertiary-case-navigation/line-color",StylableColor( { 0,0,0,0 } ) },
    { "tunnel-primary-case-navigation/line-color",           StylableColor( { 0,0,0,0 } ) },
    { "tunnel-major-link-case-navigation/line-color",        StylableColor( { 0,0,0,0 } ) },
    { "tunnel-motorway-trunk-case-navigation/line-color",    StylableColor( { 0,0,0,0 } ) },
    { "tunnel-construction-navigation/line-color",           StylableColor( { 0,0,0,0 } ) },
    { "tunnel-minor-navigation/line-color",                  StylableColor( { 0,0,0,0 } ) },
    { "tunnel-major-link-navigation/line-color",             StylableColor( { 0,0,0,0 } ) },
    { "tunnel-street-navigation/line-color",                 StylableColor( { 0,0,0,0 } ) },
    
//    { "tunnel-street-low-navigation",                        StylableColor( { 0,0,0,0 } ) },
    
    { "tunnel-secondary-tertiary-navigation/line-color",     StylableColor( { 0,0,0,0 } ) },
    { "tunnel-primary-navigation/line-color",                StylableColor( { 0,0,0,0 } ) },
    { "tunnel-motorway-trunk-navigation/line-color",         StylableColor( { 0,0,0,0 } ) },
    
//    { "tunnel-oneway-arrow-blue-navigation",                 StylableColor( { 0,0,0,0 } ) },
//    { "tunnel-oneway-arrow-white-navigation",                StylableColor( { 0,0,0,0 } ) },
    
//    { "road-path-trail",                                     StylableColor( { 0,0,0,0 } ) },
//    { "road-path-cycleway-piste",                            StylableColor( { 0,0,0,0 } ) },
    
    { "road-path/line-color",                                StylableColor( { 0,0,0,0 } ) },
    { "road-steps/line-color",                               StylableColor( { 0,0,0,0 } ) },
    { "road-pedestrian/line-color",                          StylableColor( { 0,0,0,0 } ) },
        
    { "turning-feature-outline-navigation/circle-color",     StylableColor( { 0,0,0,0 } ) },
    { "turning-feature-outline-navigation/circle-stroke-color",StylableColor( { 0,0,0,0 } ) },
    { "road-minor-case-navigation/line-color",               StylableColor( { 0,0,0,0 } ) },
    { "road-street-case-navigation/line-color",              StylableColor( { 0,0,0,0 } ) },
    { "road-secondary-tertiary-case-navigation/line-color",  StylableColor( { 0,0,0,0 } ) },
    { "road-primary-case-navigation/line-color",             StylableColor( { 0,0,0,0 } ) },
    { "road-major-link-case-navigation/line-color",          StylableColor( { 0,0,0,0 } ) },
    { "road-motorway-trunk-case-navigation/line-color",      StylableColor( { 0,0,0,0 } ) },
    
//    { "road-construction-navigation",                        StylableColor( { 0,0,0,0 } ) },
    
    { "road-minor-navigation/line-color",                    StylableColor( { 0,0,0,0 } ) },
    { "road-major-link-navigation/line-color",               StylableColor( { 0,0,0,0 } ) },
    { "road-street-navigation/line-color",                   StylableColor( { 0,0,0,0 } ) },
    
//    { "road-street-low-navigation",                          StylableColor( { 0,0,0,0 } ) },
    
    { "road-secondary-tertiary-navigation/line-color",       StylableColor( { 0,0,0,0 } ) },
    { "road-primary-navigation/line-color",                  StylableColor( { 0,0,0,0 } ) },
    { "road-motorway-trunk-case-low-navigation/line-color",  StylableColor( { 0,0,0,0 } ) },
    { "road-motorway-trunk-navigation/line-color",           StylableColor( { 0,0,0,0 } ) },
    
//    { "level-crossing-navigation",                           StylableColor( { 0,0,0,0 } ) },
//    { "road-oneway-arrow-blue-navigation",                   StylableColor( { 0,0,0,0 } ) },
//    { "road-oneway-arrow-white-navigation",                  StylableColor( { 0,0,0,0 } ) },
    
    { "turning-feature-navigation/circle-color",             StylableColor( { 0,0,0,0 } ) },
    
//    { "crosswalks",                                          StylableColor( { 0,0,0,0 } ) },
    
//    { "road-rail-bg-white",                                  StylableColor( { 0,0,0,0 } ) },
//    { "road-rail",                                           StylableColor( { 0,0,0,0 } ) },
//    { "road-rail-tracks",                                    StylableColor( { 0,0,0,0 } ) },
    
//    { "bridge-path-trail",                                   StylableColor( { 0,0,0,0 } ) },
//    { "bridge-path-cycleway-piste",                          StylableColor( { 0,0,0,0 } ) },
    
    { "bridge-path/line-color",                              StylableColor( { 0,0,0,0 } ) },
    { "bridge-steps/line-color",                             StylableColor( { 0,0,0,0 } ) },
    { "bridge-pedestrian/line-color",                        StylableColor( { 0,0,0,0 } ) },
    
    { "bridge-minor-case-navigation/line-color",             StylableColor( { 0,0,0,0 } ) },
    { "bridge-street-case-navigation/line-color",            StylableColor( { 0,0,0,0 } ) },
    { "bridge-secondary-tertiary-case-navigation/line-color",StylableColor( { 0,0,0,0 } ) },
    { "bridge-primary-case-navigation/line-color",           StylableColor( { 0,0,0,0 } ) },
    { "bridge-major-link-case-navigation/line-color",        StylableColor( { 0,0,0,0 } ) },
    { "bridge-motorway-trunk-case-navigation/line-color",    StylableColor( { 0,0,0,0 } ) },
    
//    { "bridge-construction-navigation",                      StylableColor( { 0,0,0,0 } ) },
    
    { "bridge-minor-navigation/line-color",                  StylableColor( { 0,0,0,0 } ) },
    { "bridge-major-link-navigation/line-color",             StylableColor( { 0,0,0,0 } ) },
    { "bridge-street-navigation/line-color",                 StylableColor( { 0,0,0,0 } ) },
    
//    { "bridge-street-low-navigation",                        StylableColor( { 0,0,0,0 } ) },
    
    { "bridge-secondary-tertiary-navigation/line-color",     StylableColor( { 0,0,0,0 } ) },
    { "bridge-primary-navigation/line-color",                StylableColor( { 0,0,0,0 } ) },
    { "bridge-motorway-trunk-navigation/line-color",         StylableColor( { 0,0,0,0 } ) },
    { "bridge-major-link-2-case-navigation/line-color",      StylableColor( { 0,0,0,0 } ) },
    { "bridge-motorway-trunk-2-case-navigation/line-color",  StylableColor( { 0,0,0,0 } ) },
    { "bridge-major-link-2-navigation/line-color",           StylableColor( { 0,0,0,0 } ) },
    { "bridge-motorway-trunk-2-navigation/line-color",       StylableColor( { 0,0,0,0 } ) },
    
//    { "bridge-oneway-arrow-blue-navigation",                 StylableColor( { 0,0,0,0 } ) },
//    { "bridge-oneway-arrow-white-navigation",                StylableColor( { 0,0,0,0 } ) },
    
//    { "bridge-rail-bg-white",                                StylableColor( { 0,0,0,0 } ) },
//    { "bridge-rail",                                         StylableColor( { 0,0,0,0 } ) },
//    { "bridge-rail-tracks",                                  StylableColor( { 0,0,0,0 } ) },
    
    { "building-extrusion/fill-extrusion-color",             StylableColor( { 0,0,0,0 } ) },
    
    { "admin-2-boundary-bg/line-color",                      StylableColor( { 0,0,0,0 } ) },
    { "admin-1-boundary-bg/line-color",                      StylableColor( { 0,0,0,0 } ) },
    { "admin-0-boundary-bg/line-color",                      StylableColor( { 0,0,0,0 } ) },
    { "admin-2-boundary/line-color",                         StylableColor( { 0,0,0,0 } ) },
    { "admin-1-boundary/line-color",                         StylableColor( { 0,0,0,0 } ) },
    { "admin-0-boundary/line-color",                         StylableColor( { 0,0,0,0 } ) },
    { "admin-0-boundary-disputed/line-color",                StylableColor( { 0,0,0,0 } ) },
    
    { "building-entrance/text-color",                        StylableColor( { 0,0,0,0 } ) },
    { "building-entrance/text-halo-color",                   StylableColor( { 0,0,0,0 } ) },
    { "building-number-label/text-color",                    StylableColor( { 0,0,0,0 } ) },
    { "building-number-label/text-halo-color",               StylableColor( { 0,0,0,0 } ) },
    { "block-number-label/text-color",                       StylableColor( { 0,0,0,0 } ) },
    { "block-number-label/text-halo-color",                  StylableColor( { 0,0,0,0 } ) },
    
    { "road-intersection/text-color",                        StylableColor( { 0,0,0,0 } ) },
    
//    { "traffic-signal-navigation",                           StylableColor( { 0,0,0,0 } ) },
    
    { "road-label-navigation/text-color",                    StylableColor( { 0,0,0,0 } ) },
    { "road-label-navigation/text-halo-color",               StylableColor( { 0,0,0,0 } ) },
    
//    { "road-number-shield-navigation",                       StylableColor( { 0,0,0,0 } ) },
//    { "road-exit-shield-navigation",                         StylableColor( { 0,0,0,0 } ) },
    
    { "waterway-label/text-color",                           StylableColor( { 0,0,0,0 } ) },
    { "waterway-label/text-halo-color",                      StylableColor( { 0,0,0,0 } ) },
    { "natural-line-label/text-halo-color",                  StylableColor( { 0,0,0,0 } ) },
    { "natural-line-label/text-color",                       StylableColor( { 0,0,0,0 } ) },
    { "natural-point-label/text-color",                      StylableColor( { 0,0,0,0 } ) },
    { "natural-point-label/text-halo-color",                 StylableColor( { 0,0,0,0 } ) },
    { "water-line-label/text-color",                         StylableColor( { 0,0,0,0 } ) },
    { "water-line-label/text-halo-color",                    StylableColor( { 0,0,0,0 } ) },
    { "water-point-label/text-color",                        StylableColor( { 0,0,0,0 } ) },
    { "water-point-label/text-halo-color",                   StylableColor( { 0,0,0,0 } ) },
    
    { "poi-label/text-color",                                StylableColor( { 0,0,0,0 } ) },
    { "poi-label/text-halo-color",                           StylableColor( { 0,0,0,0 } ) },
    
    { "airport-label/text-color",                            StylableColor( { 0,0,0,0 } ) },
    { "airport-label/text-halo-color",                       StylableColor( { 0,0,0,0 } ) },
        
    { "settlement-subdivision-label/text-color",             StylableColor( { 0,0,0,0 } ) },
    { "settlement-subdivision-label/text-halo-color",        StylableColor( { 0,0,0,0 } ) },
    
    { "settlement-minor-label/text-color/step/0",            StylableColor( { 0,0,0,0 } ) },
    { "settlement-minor-label/text-color/step/1",            StylableColor( { 0,0,0,0 } ) },
    { "settlement-minor-label/text-color/step/2",            StylableColor( { 0,0,0,0 } ) },
    { "settlement-minor-label/text-halo-color",              StylableColor( { 0,0,0,0 } ) },
    
    { "settlement-major-label/text-color/step/0",            StylableColor( { 0,0,0,0 } ) },
    { "settlement-major-label/text-color/step/1",            StylableColor( { 0,0,0,0 } ) },
    { "settlement-major-label/text-color/step/2",            StylableColor( { 0,0,0,0 } ) },
    { "settlement-major-label/text-halo-color",              StylableColor( { 0,0,0,0 } ) },
    
    { "state-label/text-color",                              StylableColor( { 0,0,0,0 } ) },
    { "state-label/text-halo-color",                         StylableColor( { 0,0,0,0 } ) },
    
    { "country-label/text-color",                            StylableColor( { 0,0,0,0 } ) },
    { "country-label/text-halo-color",                       StylableColor( { 0,0,0,0 } ) },
    
    { "continent-label/text-color",                          StylableColor( { 0,0,0,0 } ) },
    { "continent-label/text-halo-color",                     StylableColor( { 0,0,0,0 } ) },

};

static std::atomic<int> needUpdate = { 0 } ;

bool update() {
    if (needUpdate > 0) {
        needUpdate--;
        for (auto it : palleteMap) {
            it.second.update();
        }
        return true;
    }
    return false;
}

void setColorBase(const mbgl::Color& color) {
    const Hsl base(color);
    for (auto& it : palleteMap) {
        it.second = base;
    }

    needUpdate = 100;
}

mbgl::Color getColor(const std::string& uri) {
    static mbgl::Color stub;
    auto it = palleteMap.find(uri);
    if (it != palleteMap.end()) return it->second;
    else return stub;
}

}


bool update() {
    bool needUpdate = false;

    rendertime::timestamp.update();
    needUpdate |= spotlight::toggle.update();
    needUpdate |= landscape::toggle.update();
    needUpdate |= palette::update();
    
    return needUpdate;
}


}
}
