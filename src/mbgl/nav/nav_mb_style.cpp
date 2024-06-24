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

#include <math.h>

namespace nav {
namespace style {

const std::string& url() {
    static std::string url(
                           
    // 主题模式
//    "mapbox://styles/notheorem/clxonzrtz00k701qq00fj6rnh" // dark
//    "mapbox://styles/notheorem/clxrac32800o901qw94ryfkdz" // light

    // 色彩模式
//    "mapbox://styles/notheorem/clxo7gf6000l301pf067zcqu9" // 蓝
//    "mapbox://styles/notheorem/clxk5j3qw00av01qq93udh6bm" // 暗蓝
//    "mapbox://styles/notheorem/clxo72llu00j101qqgeb55xlw" // 紫
//    "mapbox://styles/notheorem/clxolet6000jx01qq4un34jep" // 红紫
//    "mapbox://styles/notheorem/clxo6t3l800l501qma3lk7kku" // 玫红
//    "mapbox://styles/notheorem/clxo70o4q00l701qmekyz04x5" // 绿
//    "mapbox://styles/notheorem/clxo6ym3j00k501r2dgm7d26j" // 芥末绿
    "mapbox://styles/notheorem/clxo77xld004501r0ae6s94fz" // 黄橙
//    "mapbox://styles/notheorem/clxo6u08m00l601qme55bahbs" // 土黄
//    "mapbox://styles/notheorem/clxo7bjja00kn01qrdm5k6o1a" // 白
//    "mapbox://styles/notheorem/clxo7dspj00ko01qrhny81uem" // 黑
    );
    
    return url;
}

namespace texture {

struct ImageData {
    mbgl::PremultipliedImage image;
    mbgl::optional<mbgl::gfx::Texture> texture { mbgl::nullopt };
};

std::map<std::string, ImageData> imageMap;

void load(const std::string& path) {
    imageMap["matcap.0.png"].image = mbgl::decodeImage(mbgl::util::read_file(path + "matcap.0.png"));
    imageMap["matcap.1.png"].image = mbgl::decodeImage(mbgl::util::read_file(path + "matcap.1.png"));
    imageMap["matcap.2.png"].image = mbgl::decodeImage(mbgl::util::read_file(path + "matcap.2.png"));
    imageMap["matcap.3.png"].image = mbgl::decodeImage(mbgl::util::read_file(path + "matcap.3.png"));
    imageMap["matcap.4.png"].image = mbgl::decodeImage(mbgl::util::read_file(path + "matcap.4.png"));
}

void release() {
    imageMap.clear();
}

void upload(mbgl::gfx::UploadPass& uploadPass) {
    for (auto& i : imageMap) {
        if (!i.second.texture) {
            i.second.texture = uploadPass.createTexture(i.second.image);
        }
    }
}

mbgl::gfx::TextureResource& get(const std::string& name) {
    return imageMap[name].texture->getResource();
}

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
                _ratio = fmax(_ratio - fmax((_ratio - 0.) * 0.015, 0.0015), 0.);
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
    float h=0., s=0., l=0., a=0.;

    Hsl() = default;

    Hsl(float h, float s, float l, float a=1.) : h(h), s(s), l(l), a(a) {
        assert(h >= 0. && h <= 360.);
        assert(s >= 0. && s <= 1.);
        assert(l >= 0. && l <= 1.);
        assert(a >= 0. && a <= 1.);
    }

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

struct StyledHslSample {
    float hCoef=0, sCoef=0, lCoef=0, aCoef=0;
    StyledHslSample(float h, float s, float l, float a=1.) : hCoef(h), sCoef(s), lCoef(l), aCoef(a) { }
};

struct HslConstrain {
    float hCoef=0, sCoef=0, lCoef=0, aCoef=0;
    
    HslConstrain() = default;
    
    HslConstrain(const StyledHslSample& standardSample) { // center (292., .5, .5, 1.)
        hCoef = 292. - standardSample.hCoef;
        sCoef = standardSample.sCoef - .5;
        lCoef = logf((standardSample.lCoef - .1) / .8) / logf(0.5);
        aCoef = standardSample.aCoef;
    }
    
    Hsl stylize(const Hsl& base) const {
        Hsl color;
        color.h = fmod(base.h + hCoef, 360.);          // h [0,360]
        color.s = fmin(fmax(base.s + sCoef, 0.), 1.);  // s [0,1]
        color.l = pow(base.l, lCoef) * .8 + .1;        // l [0,1]
        color.a = fmin(fmax(base.a * aCoef, 0.), 1.);  // a [0,1]
        return color;
    }
};

struct StyleNotSupport {

};


class GradientColor {
    Hsl color;

    struct Stylable {
        enum Mode : uint8_t { stable, dynamic, notSupport };
        Mode mode = Mode::stable;
        Hsl color;
        HslConstrain constain;

        Stylable(const Hsl& color) : mode(Mode::stable), color(color) { }
        Stylable(const StyledHslSample& standardSample) : mode(Mode::dynamic), constain(standardSample) { }
        Stylable(const StyleNotSupport&) : mode(Mode::notSupport), color(0,1,1) { }
        
        operator const Hsl& () const { return color; }

        void operator = (const Hsl& base) {
            switch (mode) {
                case dynamic:
                    color = constain.stylize(base);
                    break;
                default:
                    break;
            }
        }
    } stylableColor;

public:
    GradientColor(const Hsl& color) : stylableColor(color) { }
    GradientColor(const StyledHslSample& standardSample) : stylableColor(standardSample) { }
    GradientColor(const StyleNotSupport& notSupport) : stylableColor(notSupport) { }

    inline void operator = (const Hsl& base) { stylableColor = base; }
    inline void update() { color.smoothto(stylableColor); }
    inline operator const Hsl& () const { return color; }
    inline operator mbgl::Color () const { return color.rgb(); }
};


std::map<std::string, GradientColor> palleteColors = {

    // land & water - land
    
    { "land/background-color/0",                             GradientColor(StyledHslSample(292, .5, .5)) },
    { "land/background-color/1",                             GradientColor(StyledHslSample(292, .49, .35)) },
    { "landcover/fill-color",                                GradientColor(StyledHslSample(292, .5, .3)) },
    { "national-park/fill-color",                            GradientColor(StyledHslSample(292, .42, .32)) },
    { "landuse/fill-color",                                  GradientColor(StyledHslSample(292, .42, .32)) },
    { "hillshade/fill-color/0/0",                            GradientColor(StyledHslSample(292, .45, .47, 0.06)) },
    { "hillshade/fill-color/0/1",                            GradientColor(StyledHslSample(292, .49, .43, 0.04)) },
    { "hillshade/fill-color/1/0",                            GradientColor(StyledHslSample(292, .45, .47, 0)) },
    { "hillshade/fill-color/1/1",                            GradientColor(StyledHslSample(292, .49, .43, 0)) },
    
    // land & water - water
    
    { "waterway/line-color",                                 GradientColor(StyledHslSample(292, .45, .23)) },
    { "water/fill-color",                                    GradientColor(StyledHslSample(292, .45, .33)) },
    { "water-depth/fill-color/0/0",                          GradientColor(StyledHslSample(292, .45, .28, 0.35)) },
    { "water-depth/fill-color/0/1",                          GradientColor(StyledHslSample(292, .45, .27, 0.95)) },
    { "water-depth/fill-color/0/2",                          GradientColor(StyledHslSample(292, .45, .22, 0.95)) },
    { "water-depth/fill-color/1/0",                          GradientColor(StyledHslSample(292, .45, .28, 0)) },
    { "water-depth/fill-color/1/1",                          GradientColor(StyledHslSample(292, .45, .27, 0)) },
    { "water-depth/fill-color/1/2",                          GradientColor(StyledHslSample(292, .45, .22, 0)) },
    
    // land & water - built
    
    { "land-structure-polygon/fill-color/0",                 GradientColor(StyledHslSample(292, .5, .5, 0)) },
    { "land-structure-polygon/fill-color/1",                 GradientColor(StyledHslSample(292, .49, .35, 0)) },
    { "land-structure-line/line-color/0",                    GradientColor(StyledHslSample(292, .5, .5, 0)) },
    { "land-structure-line/line-color/1",                    GradientColor(StyledHslSample(292, .49, .35, 0)) },
    
    // transit - built
    
    { "aeroway-polygon/fill-color/",                         GradientColor(StyledHslSample(292, .49, .44)) },
    { "aeroway-line/line-color",                             GradientColor(StyledHslSample(292, .49, .44)) },
    
    // walking cycling etc. - tunnels
    
    { "tunnel-path-trail/line-color",                        GradientColor(StyledHslSample(292, .44, .3)) },
    { "tunnel-path-cycleway-piste/line-color",               GradientColor(StyledHslSample(292, .44, .3)) },
    { "tunnel-path/line-color",                              GradientColor(StyledHslSample(292, .44, .3)) },
    { "tunnel-steps/line-color",                             GradientColor(StyledHslSample(292, .44, .3)) },
    { "tunnel-pedestrian/line-color",                        GradientColor(StyledHslSample(292, .44, .3)) },
    
    // road network - tunnels
    
    { "tunnel-minor-case-navigation/line-color",             GradientColor(StyledHslSample(292, .46, .46)) },
    { "tunnel-street-case-navigation/line-color",            GradientColor(StyledHslSample(292, .46, .46)) },
    { "tunnel-secondary-tertiary-case-navigation/line-color",GradientColor(StyledHslSample(292, .46, .46)) },
    { "tunnel-primary-case-navigation/line-color",           GradientColor(StyledHslSample(292, .46, .46)) },
    { "tunnel-major-link-case-navigation/line-color",        GradientColor(StyledHslSample(292, .46, .46)) },
    { "tunnel-motorway-trunk-case-navigation/line-color",    GradientColor(StyledHslSample(292, .46, .46)) },
    { "tunnel-construction-navigation/line-color",           GradientColor(StyledHslSample(292, .48, .32)) },
    { "tunnel-minor-navigation/line-color",                  GradientColor(StyledHslSample(292, .48, .32)) },
    { "tunnel-major-link-navigation/line-color",             GradientColor(StyledHslSample(292, .48, .32)) },
    { "tunnel-street-navigation/line-color",                 GradientColor(StyledHslSample(292, .48, .32)) },
    { "tunnel-street-low-navigation",                        GradientColor(StyledHslSample(292, .48, .32)) },
    { "tunnel-secondary-tertiary-navigation/line-color",     GradientColor(StyledHslSample(292, .48, .32)) },
    { "tunnel-primary-navigation/line-color",                GradientColor(StyledHslSample(292, .48, .32)) },
    { "tunnel-motorway-trunk-navigation/line-color",         GradientColor(StyledHslSample(292, .48, .32)) },
    { "tunnel-oneway-arrow-blue-navigation",                 GradientColor(StyleNotSupport()) },
    { "tunnel-oneway-arrow-white-navigation",                GradientColor(StyleNotSupport()) },
    
    // walking cycling etc. - surface

    { "road-path-trail/line-color",                          GradientColor(StyledHslSample(292, .49, .43)) },
    { "road-path-cycleway-piste/line-color",                 GradientColor(StyledHslSample(292, .49, .43)) },
    { "road-path/line-color",                                GradientColor(StyledHslSample(292, .49, .43)) },
    { "road-steps/line-color",                               GradientColor(StyledHslSample(292, .49, .43)) },
    { "road-pedestrian/line-color",                          GradientColor(StyledHslSample(292, .49, .43)) },
    
    // road network - surface

    { "turning-feature-outline-navigation/circle-color",     GradientColor(StyledHslSample(292, .49, .43)) },
    { "turning-feature-outline-navigation/circle-stroke-color",GradientColor(StyledHslSample(292, .43, .32)) },
    { "road-minor-case-navigation/line-color",               GradientColor(StyledHslSample(292, .43, .32)) },
    { "road-street-case-navigation/line-color",              GradientColor(StyledHslSample(292, .43, .32)) },
    { "road-secondary-tertiary-case-navigation/line-color",  GradientColor(StyledHslSample(292, .43, .32)) },
    { "road-primary-case-navigation/line-color",             GradientColor(StyledHslSample(292, .43, .32)) },
    { "road-major-link-case-navigation/line-color",          GradientColor(StyledHslSample(292, .43, .32)) },
    { "road-motorway-trunk-case-navigation/line-color",      GradientColor(StyledHslSample(292, .43, .32)) },
    { "road-construction-navigation/line-color",             GradientColor(StyledHslSample(292, .49, .43)) },
    { "road-minor-navigation/line-color",                    GradientColor(StyledHslSample(292, .49, .43)) },
    { "road-major-link-navigation/line-color",               GradientColor(StyledHslSample(292, .49, .43)) },
    { "road-street-navigation/line-color",                   GradientColor(StyledHslSample(292, .49, .43)) },
    { "road-street-low-navigation/line-color",               GradientColor(StyledHslSample(292, .49, .43)) },
    { "road-secondary-tertiary-navigation/line-color",       GradientColor(StyledHslSample(292, .49, .43)) },
    { "road-primary-navigation/line-color",                  GradientColor(StyledHslSample(292, .49, .43)) },
    { "road-motorway-trunk-case-low-navigation/line-color",  GradientColor(StyledHslSample(292, .43, .32)) },
    { "road-motorway-trunk-navigation/line-color",           GradientColor(StyledHslSample(292, .49, .43)) },
    { "level-crossing-navigation",                           GradientColor(StyleNotSupport()) },
    { "road-oneway-arrow-blue-navigation",                   GradientColor(StyleNotSupport()) },
    { "road-oneway-arrow-white-navigation",                  GradientColor(StyleNotSupport()) },
    { "turning-feature-navigation/circle-color",             GradientColor(StyledHslSample(292, .49, .43)) },
    { "crosswalks",                                          GradientColor(StyleNotSupport()) },
    
    // transit - surface
    
    { "road-rail-bg-white/line-color/0",                     GradientColor(Hsl(0, 0, 1)) },
    { "road-rail-bg-white/line-color/1",                     GradientColor(Hsl(0, 0, 1)) },
    { "road-rail/line-color/0",                              GradientColor(StyledHslSample(292, .43, .32)) },
    { "road-rail/line-color/1",                              GradientColor(StyledHslSample(292, .44, .3)) },
    { "road-rail-tracks/line-color",                         GradientColor(StyledHslSample(292, .44, .3)) },
    
    // walking cycling etc. - barrier / bridges
    
    { "bridge-path-trail/line-color",                        GradientColor(StyledHslSample(292, .49, .43)) },
    { "bridge-path-cycleway-piste/line-color",               GradientColor(StyledHslSample(292, .49, .43)) },
    { "bridge-path/line-color",                              GradientColor(StyledHslSample(292, .49, .43)) },
    { "bridge-steps/line-color",                             GradientColor(StyledHslSample(292, .49, .43)) },
    { "bridge-pedestrian/line-color",                        GradientColor(StyledHslSample(292, .49, .43)) },
    
    // road network - bridges
    
    { "bridge-minor-case-navigation/line-color",             GradientColor(StyledHslSample(292, .43, .32)) },
    { "bridge-street-case-navigation/line-color",            GradientColor(StyledHslSample(292, .43, .32)) },
    { "bridge-secondary-tertiary-case-navigation/line-color",GradientColor(StyledHslSample(292, .43, .32)) },
    { "bridge-primary-case-navigation/line-color",           GradientColor(StyledHslSample(292, .43, .32)) },
    { "bridge-major-link-case-navigation/line-color",        GradientColor(StyledHslSample(292, .43, .32)) },
    { "bridge-motorway-trunk-case-navigation/line-color",    GradientColor(StyledHslSample(292, .43, .32)) },
    { "bridge-construction-navigation/line-color",           GradientColor(StyledHslSample(292, .49, .43)) },
    { "bridge-minor-navigation/line-color",                  GradientColor(StyledHslSample(292, .49, .43)) },
    { "bridge-major-link-navigation/line-color",             GradientColor(StyledHslSample(292, .49, .43)) },
    { "bridge-street-navigation/line-color",                 GradientColor(StyledHslSample(292, .49, .43)) },
    { "bridge-street-low-navigation",                        GradientColor(StyledHslSample(292, .49, .43)) },
    { "bridge-secondary-tertiary-navigation/line-color",     GradientColor(StyledHslSample(292, .49, .43)) },
    { "bridge-primary-navigation/line-color",                GradientColor(StyledHslSample(292, .49, .43)) },
    { "bridge-motorway-trunk-navigation/line-color",         GradientColor(StyledHslSample(292, .49, .43)) },
    { "bridge-major-link-2-case-navigation/line-color",      GradientColor(StyledHslSample(292, .43, .32)) },
    { "bridge-motorway-trunk-2-case-navigation/line-color",  GradientColor(StyledHslSample(292, .43, .32)) },
    { "bridge-major-link-2-navigation/line-color",           GradientColor(StyledHslSample(292, .49, .43)) },
    { "bridge-motorway-trunk-2-navigation/line-color",       GradientColor(StyledHslSample(292, .49, .43)) },
    { "bridge-oneway-arrow-blue-navigation",                 GradientColor(StyleNotSupport()) },
    { "bridge-oneway-arrow-white-navigation",                GradientColor(StyleNotSupport()) },
    
    // transit - bridges
    
    { "bridge-rail-bg-white/line-color/0",                   GradientColor(Hsl(0, 0, 1)) },
    { "bridge-rail-bg-white/line-color/1",                   GradientColor(Hsl(0, 0, 1)) },
    { "bridge-rail/line-color",                              GradientColor(StyledHslSample(292, .44, .3)) },
    { "bridge-rail-tracks/line-color",                       GradientColor(StyledHslSample(292, .44, .3)) },
    
    // buildings - extruded
    
    { "building-extrusion/fill-extrusion-color",             GradientColor(StyledHslSample(292, .49, .3)) },
    
    // Adminstrative boundaries - admin
    
    { "admin-2-boundary-bg/line-color",                      GradientColor(StyledHslSample(292, .44, .3)) },
    { "admin-1-boundary-bg/line-color",                      GradientColor(StyledHslSample(292, .44, .3)) },
    { "admin-0-boundary-bg/line-color",                      GradientColor(StyledHslSample(292, .44, .3)) },
    
    { "admin-2-boundary/line-color",                         GradientColor(StyledHslSample(292, .49, .58)) },
    { "admin-1-boundary/line-color",                         GradientColor(StyledHslSample(292, .49, .58)) },
    
    { "admin-0-boundary/line-color",                         GradientColor(StyledHslSample(292, .44, .59)) },
    { "admin-0-boundary-disputed/line-color",                GradientColor(StyledHslSample(292, .44, .59)) },
    
    // buildings - building labels
    
    { "building-entrance/text-color",                        GradientColor(StyledHslSample(292, .38, .72)) },
    { "building-entrance/text-halo-color",                   GradientColor(StyledHslSample(292, .43, .38)) },
    
    { "building-number-label/text-color",                    GradientColor(StyledHslSample(292, .38, .72)) },
    { "building-number-label/text-halo-color",               GradientColor(StyledHslSample(292, .43, .38)) },
    
    { "block-number-label/text-color",                       GradientColor(StyledHslSample(292, .49, .73)) },
    { "block-number-label/text-halo-color",                  GradientColor(StyledHslSample(292, .48, .24)) },
    
    // road network - road lables
    
    { "road-intersection/text-color",                        GradientColor(StyledHslSample(230, .57, .64)) },
    { "traffic-signal-navigation",                           GradientColor(StyleNotSupport()) },
    
    { "road-label-navigation/text-color",                    GradientColor(StyledHslSample(292, .49, .86)) },
    { "road-label-navigation/text-halo-color",               GradientColor(StyledHslSample(292, .48, .24)) },
    
    { "road-number-shield-navigation",                       GradientColor(StyleNotSupport()) },
    { "road-exit-shield-navigation",                         GradientColor(StyleNotSupport()) },
    
    // natural features - natrual labels
    
    { "waterway-label/text-color",                           GradientColor(StyledHslSample(292, .48, .54)) },
    { "waterway-label/text-halo-color",                      GradientColor(StyledHslSample(292, .48, .24, .5)) },
    
    { "natural-line-label/text-color",                       GradientColor(StyledHslSample(292, .49, .73)) },
    { "natural-line-label/text-halo-color",                  GradientColor(StyledHslSample(292, .48, .24)) },
    
    { "natural-point-label/text-color",                      GradientColor(StyledHslSample(292, .49, .73)) },
    { "natural-point-label/text-halo-color",                 GradientColor(StyledHslSample(292, .48, .24)) },
    
    { "water-line-label/text-color",                         GradientColor(StyledHslSample(292, .48, .54)) },
    { "water-line-label/text-halo-color",                    GradientColor(StyledHslSample(292, .48, .24, .5)) },
    
    { "water-point-label/text-color",                        GradientColor(StyledHslSample(292, .48, .54)) },
    { "water-point-label/text-halo-color",                   GradientColor(StyledHslSample(292, .48, .24, .5)) },
    
    // Point of interest labels - poi labels
    
    { "poi-label/text-color",                                GradientColor(StyledHslSample(292, .49, .8)) },
    { "poi-label/text-halo-color",                           GradientColor(StyledHslSample(292, .48, .24)) },
    
    // transit - transit lables
    
    { "airport-label/text-color",                            GradientColor(StyledHslSample(292, .49, .86)) },
    { "airport-label/text-halo-color",                       GradientColor(StyledHslSample(292, .48, .24)) },
    
    // place labels - place labels
    
    { "settlement-subdivision-label/text-color",             GradientColor(StyledHslSample(292, .49, .74)) },
    { "settlement-subdivision-label/text-halo-color",        GradientColor(StyledHslSample(292, .48, .24)) },
    
    { "settlement-minor-label/text-color/0",                 GradientColor(StyledHslSample(292, .49, .86)) },
    { "settlement-minor-label/text-color/1",                 GradientColor(StyledHslSample(292, .49, .73)) },
    { "settlement-minor-label/text-color/2",                 GradientColor(StyledHslSample(292, .49, .67)) },
    { "settlement-minor-label/text-halo-color",              GradientColor(StyledHslSample(292, .48, .24)) },
    
    { "settlement-major-label/text-color/0",                 GradientColor(StyledHslSample(292, .49, .86)) },
    { "settlement-major-label/text-color/1",                 GradientColor(StyledHslSample(292, .49, .73)) },
    { "settlement-major-label/text-color/2",                 GradientColor(StyledHslSample(292, .49, .67)) },
    { "settlement-major-label/text-halo-color",              GradientColor(StyledHslSample(292, .48, .24)) },
    
    { "state-label/text-color",                              GradientColor(StyledHslSample(292, .49, .86)) },
    { "state-label/text-halo-color",                         GradientColor(StyledHslSample(292, .48, .24)) },
    
    { "country-label/text-color",                            GradientColor(StyledHslSample(292, .49, .59)) },
    { "country-label/text-halo-color",                       GradientColor(StyledHslSample(292, .48, .24)) },
    
    { "continent-label/text-color",                          GradientColor(StyledHslSample(292, .49, .59)) },
    { "continent-label/text-halo-color",                     GradientColor(StyledHslSample(292, .48, .24)) },

};

static std::atomic<int> needUpdate = { 0 } ;

bool update() {
    if (needUpdate > 0) {
        needUpdate--;
        for (auto it : palette::palleteColors) {
            it.second.update();
        }
        return true;
    }
    return false;
}

void setColorBase(const mbgl::Color& color) {
    const Hsl base(color);
    for (auto& it : palette::palleteColors) {
        it.second = base;
    }
    needUpdate = 100;
}

mbgl::Color getColor(const std::string& uri) {
    static mbgl::Color stub;
    auto it = palette::palleteColors.find(uri);
    if (it != palette::palleteColors.end()) return it->second;
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
