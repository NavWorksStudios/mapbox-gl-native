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

const std::string& accessToken() {
    static std::string token = "pk.eyJ1Ijoibm90aGVvcmVtIiwiYSI6ImNsb3Y0cjVoaTBxY20yamxsb3pvNHI3MW0ifQ.ZEemHSq9LP1ScpWw6w9Z0g";
    return token;
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


struct Hsla {
    float h=0., s=0., l=0., a=0.;

    Hsla() = default;

    Hsla(float h, float s, float l, float a=1.) : h(h), s(s), l(l), a(a) {
        assert(h >= 0. && h <= 360.);
        assert(s >= 0. && s <= 1.);
        assert(l >= 0. && l <= 1.);
        assert(a >= 0. && a <= 1.);
    }
    
    Hsla(const mbgl::Color& color) {
        *this << color;
    }
    
    void operator >> (mbgl::Color& rgba) const {
        float& r = rgba.r;
        float& g = rgba.g;
        float& b = rgba.b;
     
        if (s == 0) {
            r = g = b = l; //  achromatic
        } else {
            float q = l < 0.5 ? l * (1. + s) : l + s - l * s;
            float p = 2. * l - q;
            r = hue_to_rgb(p, q, h + 1./3.);
            g = hue_to_rgb(p, q, h);
            b = hue_to_rgb(p, q, h - 1./3.);
        }
        
        rgba.a = a;
    }
    
    void operator << (const mbgl::Color& rgba) {
        const float& r = rgba.r;
        const float& g = rgba.g;
        const float& b = rgba.b;
        
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
        
        a = rgba.a;
    }
    
    void smoothto(const Hsla& to) {
        h = value_smooth_to(h, to.h);
        s = value_smooth_to(s, to.s);
        l = value_smooth_to(l, to.l);
        a = value_smooth_to(a, to.a);
    }
    
};

static const Hsla SAMPLE_CENTER(292./255., .5, .5, 1.);

class Color {
    Hsla hsla;
    mbgl::Color rgba;

public:
    struct Stylizer {
        float hCoef=0, sCoef=0, lCoef=0, aCoef=0;
        
        Stylizer() = default;

        Stylizer(const Hsla& base, const Hsla& sample) {
            hCoef = base.h - sample.h;
            sCoef = sample.s - base.s;
            lCoef = logf((sample.l - .1) / .8) / logf(base.l);
            aCoef = sample.a;
        }
        
        void stylize(const Hsla& base, Hsla& color) const {
            color.h = fmod(base.h + hCoef, 360.);          // h [0,360]
            color.s = fmin(fmax(base.s + sCoef, 0.), 1.);  // s [0,1]
            color.l = pow(base.l, lCoef) * .8 + .1;        // l [0,1]
            color.a = fmin(fmax(base.a * aCoef, 0.), 1.);  // a [0,1]
        }
    };

    void smoothto(const Color& color) {
        hsla.smoothto(color.hsla);
        hsla >> rgba;
    }
    
    void stylize(const Hsla& base, const Stylizer& stylizer) {
        stylizer.stylize(base, hsla);
        hsla >> rgba;
    }

    inline const mbgl::Color& rgb() const { return rgba; }

};


class GradientColor : Color {
    Color stylizedColor;
    Color::Stylizer stylizer;

public:
    GradientColor() = default;
    GradientColor(const Color::Stylizer& stylizer) : stylizer(stylizer) { }
    
    inline void operator = (const Hsla& base) {
        stylizedColor.stylize(base, stylizer);
    }
    
    inline void update() { smoothto(stylizedColor); }
    inline operator const mbgl::Color& () const { return rgb(); }
};


std::map<int32_t, GradientColor> paletteColors;

static std::atomic<int> needUpdate = { 0 } ;

bool update() {
    if (needUpdate > 0) {
        needUpdate--;
        for (auto it : paletteColors) {
            it.second.update();
        }
        return true;
    }
    return false;
}

void setColorBase(const mbgl::Color& color) {
    const Hsla base(color);

    for (auto& it : paletteColors) {
        it.second = base;
    }

    needUpdate = 100;
}

const mbgl::Color& getColor(const mbgl::Color& color) {
    if (color.isHsl) {
        const int32_t key = static_cast<int32_t>((color.r + color.g + color.b + color.a) * 100000000);
        auto it = paletteColors.find(key);
        if (it == paletteColors.end()) {
            it = paletteColors.insert( {key, {Color::Stylizer(SAMPLE_CENTER, Hsla(color))}} ).first;
        }
        return it->second;
    } else {
        return color;
    }
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
