//
//  nav_mb_style.cpp
//
//  Created by BernieZhao on 2023/12/13.
//

#include "mbgl/nav/nav_mb_style.hpp"
#include "mbgl/nav/nav_mb_layer.hpp"

#include <mbgl/style/image.hpp>
#include <mbgl/util/io.hpp>

#include "mbgl/nav/nav_log.hpp"


namespace nav {
namespace style {

const std::string& url() {
    static std::string url(
    // 主题模式
//    "mapbox://styles/notheorem/clxu5ehnm00ro01qqhhim0d4f" // dark
//    "mapbox://styles/notheorem/clxrac32800o901qw94ryfkdz" // light

    // 色彩模式
    "mapbox://styles/notheorem/clxuokujq00fw01r07m42dsl3"
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
            float hh = h / 360.;
            r = hue_to_rgb(p, q, hh + 1./3.);
            g = hue_to_rgb(p, q, hh);
            b = hue_to_rgb(p, q, hh - 1./3.);
        }
        
        rgba.a = a;
        
        assert(r >= 0. && r <= 1.);
        assert(g >= 0. && g <= 1.);
        assert(b >= 0. && b <= 1.);
        assert(a >= 0. && a <= 1.);
    }
    
    void operator << (const mbgl::Color& rgba) {
        const float& r = rgba.r;
        const float& g = rgba.g;
        const float& b = rgba.b;
        
        float maxVal = std::max(r, std::max(g, b));
        float minVal = std::min(r, std::min(g, b));
        float delta = maxVal - minVal;
     
        l = (maxVal + minVal) / 2.0;
        if (std::abs(delta) < std::numeric_limits<float>::epsilon()) {
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
        
        assert(h >= 0. && h <= 360.);
        assert(s >= 0. && s <= 1.);
        assert(l >= 0. && l <= 1.);
        assert(a >= 0. && a <= 1.);
    }
    
    void smoothto(const Hsla& to) {
        h = value_smooth_to(h, to.h);
        s = value_smooth_to(s, to.s);
        l = value_smooth_to(l, to.l);
        a = value_smooth_to(a, to.a);
    }
};

static const Hsla SAMPLE_CENTER = { 292., .5, .5, 1. };

class Stylizer : public Hsla {
    const bool stylizable;
    float hCoef=0, sCoef=0, lCoef=0, aCoef=0;
    
public:
    Stylizer() : stylizable(false) { }

    Stylizer(const Hsla& fixed) : Hsla(fixed), stylizable(false) {
        
    }

    Stylizer(const Hsla& base, const Hsla& sample) : stylizable(true) {
        hCoef = sample.h - SAMPLE_CENTER.h;
        sCoef = sample.s - SAMPLE_CENTER.s;
        lCoef = log2((sample.l - .1) / .8) / log2(SAMPLE_CENTER.l);
        aCoef = sample.a / SAMPLE_CENTER.a;
        
        if (std::isnan(lCoef)) lCoef = 0.5;
        else if (lCoef < 0.) lCoef = 0.;

        stylize(base);
    }
    
    void stylize(const Hsla& baseColor) {
        if (stylizable) {
            h = fmod(baseColor.h + hCoef + 360., 360.);          // h [0,360]
            s = fmin(fmax(baseColor.s + sCoef, 0.), 1.);  // s [0,1]
            l = pow(baseColor.l, lCoef) * .8 + .1;        // l [0,1], result [.1,.9]
            a = fmin(fmax(baseColor.a * aCoef, 0.), 1.);  // a [0,1]
            
            assert(h >= 0. && h <= 360.);
            assert(s >= 0. && s <= 1.);
            assert(l >= 0. && l <= 1.);
            assert(a >= 0. && a <= 1.);
        }
    }
};

class Color {
    Hsla hsla;
    mbgl::Color rgba;

public:
    Color() = default;
    
    void set(const Hsla& color) {
        hsla = color;
        hsla >> rgba;
    }
    
    const mbgl::Color& smoothto(const Hsla& color) {
        hsla.smoothto(color);
        hsla >> rgba;
        return rgba;
    }
    
    inline operator const mbgl::Color& () const { return rgba; }
};

class GradientColor : public Color {
    Stylizer stylizer;
public:
    GradientColor() = default;
    GradientColor(const Stylizer& stylizer) : stylizer(stylizer) {
        set(stylizer);
    }
    inline void stylize(const Hsla& base, bool smooth) {
        stylizer.stylize(base);
        if (!smooth) set(stylizer);
    }
    inline const mbgl::Color& update() {
        return smoothto(stylizer);
    }
};

namespace internal {

inline bool isStyliable(const mbgl::Color& color) {
    return !((int) fmod(color.a * 100000., 1000.) == 678);
}

inline Hsla unwrap(const mbgl::Color& color) {
    Hsla hsla(color);
    if (hsla.a > 0.95) hsla.a = 1.;
    return hsla;
}

}

enum { UPDATE_FRAME = 100 };
std::atomic<int> needUpdate = { UPDATE_FRAME };
//Hsla colorBase = { 292., .92, .49, 1. };
Hsla colorBase = { 0, 1., .6, 1. };

struct ColorBinding {
    void* tag;
    GradientColor color;
    std::function<void(const mbgl::Color& color)> callback;
    ColorBinding() = default;
    ColorBinding(void* tag, const Stylizer& stylizer, const std::function<void(const mbgl::Color& color)>& cb) :
    tag(tag), color(stylizer), callback(cb) {
        callback(color);
    }
};

std::vector<ColorBinding> paletteBindings;

void setColorBase(const Hsla& color, bool smooth) {
    needUpdate = UPDATE_FRAME;
    colorBase = color;
    for (auto& it : paletteBindings) {
        it.color.stylize(colorBase, smooth);
    }
}

void setColorBase(const mbgl::Color& color, bool smooth) {
    needUpdate = UPDATE_FRAME;
    colorBase = color;
    for (auto& it : paletteBindings) {
        it.color.stylize(colorBase, smooth);
    }
}

void bind(void* tag, const mbgl::Color& color, const std::function<void(const mbgl::Color& color)>& callback) {
    if (internal::isStyliable(color)) {
        paletteBindings.emplace_back(tag, Stylizer({colorBase, Hsla(color)}), callback);
    } else {
        paletteBindings.emplace_back(tag, Stylizer({internal::unwrap(color)}), callback);
    }
}

bool update() {
    if (needUpdate > 0) {
        needUpdate--;
        for (auto it : paletteBindings) {
            const auto& color = it.color.update();
            assert(it.callback);
            it.callback(color);
        }
        
        if (needUpdate < 100) {
            colorBase.h += 2.; if (colorBase.h > 360.) colorBase.h = .0;
//            colorBase.s += .01; if (colorBase.s > 1.0) colorBase.s = .1;
//            colorBase.l += .02; if (colorBase.l > 1.0) colorBase.l = .1;
            
            setColorBase(colorBase, false);
            nav::log::i("palette", "basecolor hsla(%f,%f,%f,%f)", colorBase.h, colorBase.s, colorBase.l, colorBase.a);
        }
        
        return true;
    }
    
    return false;
}

bool changed() {
    return (needUpdate > 0);
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
