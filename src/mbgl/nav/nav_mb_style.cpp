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
                           
                           // 开发测试
//                           "mapbox://styles/notheorem/clxonzrtz00k701qq00fj6rnh" // dark
//                           "mapbox://styles/notheorem/clxoo5n7100lh01qw47x0fdhe" // light
                           
                           
                           // 正式 0 - 10
                           "mapbox://styles/notheorem/clxk5j3qw00av01qq93udh6bm" // 暗蓝
//                           "mapbox://styles/notheorem/clxo7gf6000l301pf067zcqu9" // 蓝
//                           "mapbox://styles/notheorem/clxolet6000jx01qq4un34jep" // 红紫
//                           "mapbox://styles/notheorem/clxo72llu00j101qqgeb55xlw" // 紫
//                           "mapbox://styles/notheorem/clxo6t3l800l501qma3lk7kku" // 玫红
//                           "mapbox://styles/notheorem/clxo6u08m00l601qme55bahbs" // 土黄
//                           "mapbox://styles/notheorem/clxo6ym3j00k501r2dgm7d26j" // 芥末绿
//                           "mapbox://styles/notheorem/clxo70o4q00l701qmekyz04x5" // 深绿
//                           "mapbox://styles/notheorem/clxo77xld004501r0ae6s94fz" // 黄橙
                           
                           
//                           "mapbox://styles/notheorem/clxo7bjja00kn01qrdm5k6o1a"
//                           "mapbox://styles/notheorem/clxo7dspj00ko01qrhny81uem"
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

double time_seconds;

void update() {
    const auto now = std::chrono::steady_clock::now();
    time_seconds = std::chrono::duration_cast<std::chrono::duration<double>>(now.time_since_epoch()).count();
}

double value() {
    return time_seconds;
}

}

struct ToggleValue {
    bool _enabled = false;
    float _ratio = .5;

    void update() {
        if (_enabled) {
            if (_ratio < 1.) {
                _ratio = fmin(_ratio + fmax((1. - _ratio) * 0.1, 0.01), 1.);
            }
        } else {
            if (_ratio > 0.) {
                _ratio = fmax(_ratio - fmax((_ratio - 0.) * 0.025, 0.0025), 0.);
            }
        }
        
//        nav::log::i("ToggleValue", "%f", _ratio);
    }

    void enable() { _enabled = true; }
    void disable() { _enabled = false; }
    operator float () { return _ratio; }
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

namespace monochrome {

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

Hsl::Hsl(const mbgl::Color& rgb) {
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

mbgl::Color Hsl::rgb() {
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

bool Hsl::operator == (const Hsl& hsl) {
    return h == hsl.h && s == hsl.s && l == hsl.l && a == hsl.a;
}

void Hsl::smoothto(const Hsl& to) {
    h = value_smooth_to(h, to.h);
    s = value_smooth_to(s, to.s);
    l = value_smooth_to(l, to.l);
    a = value_smooth_to(a, to.a);
}

struct StylableColorEntity {
    bool needUpdate = false;
    std::vector<Hsl> colors;
    std::vector<Hsl> smoothToColors;
    
    virtual ~StylableColorEntity() {}
    
    virtual void stylize(const Hsl& base) = 0;
    
    void update() {
        assert(colors.size() == smoothToColors.size());
        
        if (needUpdate) {
            for (size_t i=0; i<colors.size(); i++) {
                auto& from = colors[i];
                auto& to = smoothToColors[i];
                from.smoothto(to);
            }
            
            needUpdate = !(colors[0] == smoothToColors[0]);
        }
    }
};

template <int H, int S, int L, int A> 
struct HslOffset {
    static mbgl::Color stylize(const Hsl& base) {
        
    }
};

template <class ...HslOffsets> struct StylableColor : public StylableColorEntity {
    void stylize(const Hsl& base) override {
        smoothToColors.clear();
        needUpdate = true;
    }
};

std::map<std::string, std::shared_ptr<StylableColorEntity>> colorMap = {

    
    { "land",                                                std::make_shared<StylableColor< HslOffset<0,0,0,0>, HslOffset<0,0,0,0> >>() },
    { "landcover",                                           std::make_shared<StylableColor< HslOffset<0,0,0,0> >>() },
    { "national-park",                                       std::make_shared<StylableColor< HslOffset<0,0,0,0> >>() },
    { "landuse",                                             std::make_shared<StylableColor< HslOffset<0,0,0,0> >>() },
    { "hillshade",                                           std::make_shared<StylableColor< HslOffset<0,0,0,0>, 
        HslOffset<0,0,0,0>, HslOffset<0,0,0,0>, HslOffset<0,0,0,0> >>() },
    
    
    { "waterway",                                            std::make_shared<StylableColor< HslOffset<0,0,0,0> >>() },
    { "water",                                               std::make_shared<StylableColor< HslOffset<0,0,0,0> >>() },
    { "water-depth",                                         std::make_shared<StylableColor< 
        HslOffset<0,0,0,0>, HslOffset<0,0,0,0>, HslOffset<0,0,0,0>, HslOffset<0,0,0,0>, HslOffset<0,0,0,0>, HslOffset<0,0,0,0> >>() },
    
    
//    { "land-structure-polygon",                              std::make_shared<StylableColor< HslOffset<0,0,0,0> >>() },
//    { "land-structure-line",                                 std::make_shared<StylableColor< HslOffset<0,0,0,0> >>() },
    
    
//    { "aeroway-polygon",                                     std::make_shared<StylableColor< HslOffset<0,0,0,0> >>() },
//    { "aeroway-line",                                        std::make_shared<StylableColor< HslOffset<0,0,0,0> >>() },
    
    
//    { "tunnel-path-trail",                                   std::make_shared<StylableColor< HslOffset<0,0,0,0> >>() },
//    { "tunnel-path-cycleway-piste",                          std::make_shared<StylableColor< HslOffset<0,0,0,0> >>() },
    { "tunnel-path",                                         std::make_shared<StylableColor< HslOffset<0,0,0,0> >>() },
    { "tunnel-steps",                                        std::make_shared<StylableColor< HslOffset<0,0,0,0> >>() },
    { "tunnel-pedestrian",                                   std::make_shared<StylableColor< HslOffset<0,0,0,0> >>() },
    
    
    { "tunnel-minor-case-navigation",                        std::make_shared<StylableColor< HslOffset<0,0,0,0> >>() },
    { "tunnel-street-case-navigation",                       std::make_shared<StylableColor< HslOffset<0,0,0,0> >>() },
    { "tunnel-secondary-tertiary-case-navigation",           std::make_shared<StylableColor< HslOffset<0,0,0,0> >>() },
    { "tunnel-primary-case-navigation",                      std::make_shared<StylableColor< HslOffset<0,0,0,0> >>() },
    { "tunnel-major-link-case-navigation",                   std::make_shared<StylableColor< HslOffset<0,0,0,0> >>() },
    { "tunnel-motorway-trunk-case-navigation",               std::make_shared<StylableColor< HslOffset<0,0,0,0> >>() },
    { "tunnel-construction-navigation",                      std::make_shared<StylableColor< HslOffset<0,0,0,0> >>() },
    { "tunnel-minor-navigation",                             std::make_shared<StylableColor< HslOffset<0,0,0,0> >>() },
    { "tunnel-major-link-navigation",                        std::make_shared<StylableColor< HslOffset<0,0,0,0> >>() },
    { "tunnel-street-navigation",                            std::make_shared<StylableColor< HslOffset<0,0,0,0> >>() },
//    { "tunnel-street-low-navigation",                        std::make_shared<StylableColor< HslOffset<0,0,0,0> >>() },
    { "tunnel-secondary-tertiary-navigation",                std::make_shared<StylableColor< HslOffset<0,0,0,0> >>() },
    { "tunnel-primary-navigation",                           std::make_shared<StylableColor< HslOffset<0,0,0,0> >>() },
    { "tunnel-motorway-trunk-navigation",                    std::make_shared<StylableColor< HslOffset<0,0,0,0> >>() },
//    { "tunnel-oneway-arrow-blue-navigation",                 std::make_shared<StylableColor< HslOffset<0,0,0,0> >>() },
//    { "tunnel-oneway-arrow-white-navigation",                std::make_shared<StylableColor< HslOffset<0,0,0,0> >>() },
    
    
//    { "road-path-trail",                                     std::make_shared<StylableColor< HslOffset<0,0,0,0> >>() },
//    { "road-path-cycleway-piste",                            std::make_shared<StylableColor< HslOffset<0,0,0,0> >>() },
    { "road-path",                                           std::make_shared<StylableColor< HslOffset<0,0,0,0> >>() },
    { "road-steps",                                          std::make_shared<StylableColor< HslOffset<0,0,0,0> >>() },
    { "road-pedestrian",                                     std::make_shared<StylableColor< HslOffset<0,0,0,0> >>() },
    
    
    { "turning-feature-outline-navigation",                  std::make_shared<StylableColor< HslOffset<0,0,0,0>, HslOffset<0,0,0,0> >>() },
    { "road-minor-case-navigation",                          std::make_shared<StylableColor< HslOffset<0,0,0,0> >>() },
    { "road-street-case-navigation",                         std::make_shared<StylableColor< HslOffset<0,0,0,0> >>() },
    { "road-secondary-tertiary-case-navigation",             std::make_shared<StylableColor< HslOffset<0,0,0,0> >>() },
    { "road-primary-case-navigation",                        std::make_shared<StylableColor< HslOffset<0,0,0,0> >>() },
    { "road-major-link-case-navigation",                     std::make_shared<StylableColor< HslOffset<0,0,0,0> >>() },
    { "road-motorway-trunk-case-navigation",                 std::make_shared<StylableColor< HslOffset<0,0,0,0> >>() },
//    { "road-construction-navigation",                        std::make_shared<StylableColor< HslOffset<0,0,0,0> >>() },
    { "road-minor-navigation",                               std::make_shared<StylableColor< HslOffset<0,0,0,0> >>() },
    { "road-major-link-navigation",                          std::make_shared<StylableColor< HslOffset<0,0,0,0> >>() },
    { "road-street-navigation",                              std::make_shared<StylableColor< HslOffset<0,0,0,0> >>() },
//    { "road-street-low-navigation",                          std::make_shared<StylableColor< HslOffset<0,0,0,0> >>() },
    { "road-secondary-tertiary-navigation",                  std::make_shared<StylableColor< HslOffset<0,0,0,0> >>() },
    { "road-primary-navigation",                             std::make_shared<StylableColor< HslOffset<0,0,0,0> >>() },
    { "road-motorway-trunk-case-low-navigation",             std::make_shared<StylableColor< HslOffset<0,0,0,0> >>() },
    { "road-motorway-trunk-navigation",                      std::make_shared<StylableColor< HslOffset<0,0,0,0> >>() },
//    { "level-crossing-navigation",                           std::make_shared<StylableColor< HslOffset<0,0,0,0> >>() },
//    { "road-oneway-arrow-blue-navigation",                   std::make_shared<StylableColor< HslOffset<0,0,0,0> >>() },
//    { "road-oneway-arrow-white-navigation",                  std::make_shared<StylableColor< HslOffset<0,0,0,0> >>() },
    { "turning-feature-navigation",                          std::make_shared<StylableColor< HslOffset<0,0,0,0>, HslOffset<0,0,0,0> >>() },
//    { "crosswalks",                                          std::make_shared<StylableColor< HslOffset<0,0,0,0> >>() },
    
    
//    { "road-rail-bg-white",                                  std::make_shared<StylableColor< HslOffset<0,0,0,0> >>() },
//    { "road-rail",                                           std::make_shared<StylableColor< HslOffset<0,0,0,0> >>() },
//    { "road-rail-tracks",                                    std::make_shared<StylableColor< HslOffset<0,0,0,0> >>() },
    
    
//    { "bridge-path-trail",                                   std::make_shared<StylableColor< HslOffset<0,0,0,0> >>() },
//    { "bridge-path-cycleway-piste",                          std::make_shared<StylableColor< HslOffset<0,0,0,0> >>() },
    { "bridge-path",                                         std::make_shared<StylableColor< HslOffset<0,0,0,0> >>() },
    { "bridge-steps",                                        std::make_shared<StylableColor< HslOffset<0,0,0,0> >>() },
    { "bridge-pedestrian",                                   std::make_shared<StylableColor< HslOffset<0,0,0,0> >>() },
    
    
    { "bridge-minor-case-navigation",                        std::make_shared<StylableColor< HslOffset<0,0,0,0> >>() },
    { "bridge-street-case-navigation",                       std::make_shared<StylableColor< HslOffset<0,0,0,0> >>() },
    { "bridge-secondary-tertiary-case-navigation",           std::make_shared<StylableColor< HslOffset<0,0,0,0> >>() },
    { "bridge-primary-case-navigation",                      std::make_shared<StylableColor< HslOffset<0,0,0,0> >>() },
    { "bridge-major-link-case-navigation",                   std::make_shared<StylableColor< HslOffset<0,0,0,0> >>() },
    { "bridge-motorway-trunk-case-navigation",               std::make_shared<StylableColor< HslOffset<0,0,0,0> >>() },
//    { "bridge-construction-navigation",                      std::make_shared<StylableColor< HslOffset<0,0,0,0> >>() },
    { "bridge-minor-navigation",                             std::make_shared<StylableColor< HslOffset<0,0,0,0> >>() },
    { "bridge-major-link-navigation",                        std::make_shared<StylableColor< HslOffset<0,0,0,0> >>() },
    { "bridge-street-navigation",                            std::make_shared<StylableColor< HslOffset<0,0,0,0> >>() },
//    { "bridge-street-low-navigation",                        std::make_shared<StylableColor< HslOffset<0,0,0,0> >>() },
    { "bridge-secondary-tertiary-navigation",                std::make_shared<StylableColor< HslOffset<0,0,0,0> >>() },
    { "bridge-primary-navigation",                           std::make_shared<StylableColor< HslOffset<0,0,0,0> >>() },
    { "bridge-motorway-trunk-navigation",                    std::make_shared<StylableColor< HslOffset<0,0,0,0> >>() },
    { "bridge-major-link-2-case-navigation",                 std::make_shared<StylableColor< HslOffset<0,0,0,0> >>() },
    { "bridge-motorway-trunk-2-case-navigation",             std::make_shared<StylableColor< HslOffset<0,0,0,0> >>() },
    { "bridge-major-link-2-navigation",                      std::make_shared<StylableColor< HslOffset<0,0,0,0> >>() },
    { "bridge-motorway-trunk-2-navigation",                  std::make_shared<StylableColor< HslOffset<0,0,0,0> >>() },
//    { "bridge-oneway-arrow-blue-navigation",                 std::make_shared<StylableColor< HslOffset<0,0,0,0> >>() },
//    { "bridge-oneway-arrow-white-navigation",                std::make_shared<StylableColor< HslOffset<0,0,0,0> >>() },
    
    
//    { "bridge-rail-bg-white",                                std::make_shared<StylableColor< HslOffset<0,0,0,0> >>() },
//    { "bridge-rail",                                         std::make_shared<StylableColor< HslOffset<0,0,0,0> >>() },
//    { "bridge-rail-tracks",                                  std::make_shared<StylableColor< HslOffset<0,0,0,0> >>() },
    
    
    { "building-extrusion",                                  std::make_shared<StylableColor< HslOffset<0,0,0,0> >>() },
    
    
    { "admin-2-boundary-bg",                                 std::make_shared<StylableColor< HslOffset<0,0,0,0> >>() },
    { "admin-1-boundary-bg",                                 std::make_shared<StylableColor< HslOffset<0,0,0,0> >>() },
    { "admin-0-boundary-bg",                                 std::make_shared<StylableColor< HslOffset<0,0,0,0> >>() },
    { "admin-2-boundary",                                    std::make_shared<StylableColor< HslOffset<0,0,0,0> >>() },
    { "admin-1-boundary",                                    std::make_shared<StylableColor< HslOffset<0,0,0,0> >>() },
    { "admin-0-boundary",                                    std::make_shared<StylableColor< HslOffset<0,0,0,0> >>() },
    { "admin-0-boundary-disputed",                           std::make_shared<StylableColor< HslOffset<0,0,0,0> >>() },
    
    
    { "building-entrance",                                   std::make_shared<StylableColor< HslOffset<0,0,0,0>, HslOffset<0,0,0,0> >>() },
    { "building-number-label",                               std::make_shared<StylableColor< HslOffset<0,0,0,0>, HslOffset<0,0,0,0> >>() },
    { "block-number-label",                                  std::make_shared<StylableColor< HslOffset<0,0,0,0>, HslOffset<0,0,0,0> >>() },
    
    
    { "road-intersection",                                   std::make_shared<StylableColor< HslOffset<0,0,0,0> >>() },
//    { "traffic-signal-navigation",                           std::make_shared<StylableColor< HslOffset<0,0,0,0> >>() },
    { "road-label-navigation",                               std::make_shared<StylableColor< HslOffset<0,0,0,0> >>() },
//    { "road-number-shield-navigation",                       std::make_shared<StylableColor< HslOffset<0,0,0,0> >>() },
//    { "road-exit-shield-navigation",                         std::make_shared<StylableColor< HslOffset<0,0,0,0> >>() },
    
    
    { "waterway-label",                                      std::make_shared<StylableColor< HslOffset<0,0,0,0> >>() },
    { "natural-line-label",                                  std::make_shared<StylableColor< HslOffset<0,0,0,0>, HslOffset<0,0,0,0> >>() },
    { "natural-point-label",                                 std::make_shared<StylableColor< HslOffset<0,0,0,0>, HslOffset<0,0,0,0> >>() },
    { "water-line-label",                                    std::make_shared<StylableColor< HslOffset<0,0,0,0> >>() },
    { "water-point-label",                                   std::make_shared<StylableColor< HslOffset<0,0,0,0> >>() },
    
    
    { "poi-label",                                           std::make_shared<StylableColor< HslOffset<0,0,0,0>, HslOffset<0,0,0,0> >>() },
    
    
    { "airport-label",                                       std::make_shared<StylableColor< HslOffset<0,0,0,0>, HslOffset<0,0,0,0> >>() },
    
    
    { "settlement-subdivision-label",                        std::make_shared<StylableColor< HslOffset<0,0,0,0>, HslOffset<0,0,0,0> >>() },
    { "settlement-minor-label",                              std::make_shared<StylableColor< HslOffset<0,0,0,0>, HslOffset<0,0,0,0> >>() },
    { "settlement-major-label",                              std::make_shared<StylableColor< HslOffset<0,0,0,0>, HslOffset<0,0,0,0> >>() },
    { "state-label",                                         std::make_shared<StylableColor< HslOffset<0,0,0,0>, HslOffset<0,0,0,0> >>() },
    { "country-label",                                       std::make_shared<StylableColor< HslOffset<0,0,0,0>, HslOffset<0,0,0,0> >>() },
    { "continent-label",                                     std::make_shared<StylableColor< HslOffset<0,0,0,0>, HslOffset<0,0,0,0> >>() },
};

void setColorBase(const Hsl& base) {
    for (auto it : colorMap) {
        it.second->stylize(base);
    }
}

void update() {
    for (auto it : colorMap) {
        it.second->update();
    }
}

const std::vector<Hsl>& getColor(const std::string& layerId) {
    static std::vector<Hsl> stub;
    auto it = colorMap.find(layerId);
    if (it != colorMap.end()) return it->second->colors;
    else return stub;
}

}


void update() {
    rendertime::update();
    spotlight::toggle.update();
    landscape::toggle.update();
    monochrome::update();
}


}
}
