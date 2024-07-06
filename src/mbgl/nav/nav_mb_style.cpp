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
    "mapbox://styles/navworks/clxx105i700yr01po4zbn2jc1"
    );
    
    return url;
}

const std::string& accessToken() {
    static std::string token = "pk.eyJ1IjoibmF2d29ya3MiLCJhIjoiY2x5M3U3MG96MDF5ZjJsb2lsMXZsczUxayJ9.Mnts5N9BU9OBsRb8fHauBQ";
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

template <int MAX> float mix(float from, float to, const float ratio) {
    constexpr float HALF = MAX * 0.5;
    constexpr float PRECISION = MAX * 0.001;
    const float MIX_RATIO = ratio;
    const float MIX_RATIO_BOTTOM = ratio * .1;
    const float MIX_BOTTOM = MAX * MIX_RATIO_BOTTOM;

    float delta = to - from;
    if (fabs(delta) < PRECISION) return to;
    
    if (fabs(delta) > HALF) {
        if (to < from) { // to is a small value
            // from -> MAX -> to, then delta > 0
            delta = (to + MAX) - from;
        } else { // to is a big value
            // to <- 0 <- from, then delta < 0
            delta = (to - MAX) - from;
        }
    }
    
    if (delta > 0) {
        delta = fmax(delta * MIX_RATIO, MIX_BOTTOM);
        float value = from + delta;
        if (value > MAX) value -= MAX;
        return value;
    } else if (delta < 0) {
        delta = fmin(delta * MIX_RATIO, -MIX_BOTTOM);
        float value = from + delta;
        if (value < 0) value += MAX;
        return value;
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
    
    void mix(const Hsla& to, float ratio) {
        h = palette::mix<360>(h, to.h, ratio);
        s = palette::mix<1>(s, to.s, ratio);
        l = palette::mix<1>(l, to.l, ratio);
        a = palette::mix<1>(a, to.a, ratio);
    }
    
    void smoothto(const Hsla& to) {
        mix(to, .02);
    }
};

static const Hsla SAMPLE_CENTER = { 200., .5, .5, 1. };

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
        lCoef = log2(sample.l) / log2(SAMPLE_CENTER.l);
        aCoef = sample.a / SAMPLE_CENTER.a;
        
        assert(!std::isnan(hCoef));
        assert(!std::isnan(sCoef));
        assert(!std::isnan(lCoef));
        assert(!std::isnan(aCoef));

        stylize(base);
    }
    
    void stylize(const Hsla& baseColor) {
        if (stylizable) {
            h = fmod(baseColor.h + hCoef + 360., 360.);     // h [0,360]
            s = fmin(fmax(baseColor.s + sCoef, 0.), 1.);    // s [0,1]
            l = pow(baseColor.l, lCoef);                    // l [0,1]
            a = fmin(fmax(baseColor.a * aCoef, 0.), 1.);    // a [0,1]
            
            assert(h >= 0. && h <= 360.);
            assert(s >= 0. && s <= 1.);
            assert(l >= 0. && l <= 1.);
            assert(a >= 0. && a <= 1.);
        }
    }
};

class Color {
public:
    Hsla hsla;
    mbgl::Color rgba;

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
public:
    Stylizer stylizer;

    GradientColor() = default;
    GradientColor(const Stylizer& stylizer) : stylizer(stylizer) {
        set(stylizer);
    }
    inline void stylize(const Hsla& base) {
        stylizer.stylize(base);
    }
    inline const mbgl::Color& update() {
        smoothto(stylizer);
        return *this;
    }
};

enum { UPDATE_FRAME = 100 };
std::atomic<int> needUpdate = { UPDATE_FRAME };
//Hsla colorBase = { 292., .92, .49, 1. };
Hsla colorBase = { 0, .7, .9, 1. };

struct ColorBinding {
    std::string uri;
    GradientColor color;
    std::function<void(const mbgl::Color& color)> callback;
    ColorBinding() = default;
    ColorBinding(const std::string& uri, const Stylizer& stylizer, const std::function<void(const mbgl::Color& color)>& cb) :
    uri(uri), color(stylizer), callback(cb) {
        callback(color);
    }
};

std::vector<ColorBinding> paletteBindings;

void setColorBase(const Hsla& color) {
    needUpdate = UPDATE_FRAME;
    colorBase = color;
    for (auto& it : paletteBindings) {
        it.color.stylize(colorBase);
    }
}

void setColorBase(const mbgl::Color& color) {
    needUpdate = UPDATE_FRAME;
    colorBase = color;
    for (auto& it : paletteBindings) {
        it.color.stylize(colorBase);
    }
}


namespace internal {

inline bool isStyliable(const mbgl::Color& color) {
    return !((int) fmod(color.a * 100000., 1000.) == 678);
}

inline Hsla unwrap(const mbgl::Color& color) {
    Hsla hsla(color);
    if (hsla.a > 0.95) hsla.a = 1.;
    return hsla;
}

Hsla lightenAllRoads(const std::string& uri, const Hsla& color) {
    Hsla hsla = color;
    
    if (uri.find("water") != std::string::npos) {

    } else if (uri.find("building-extrusion") != std::string::npos) {

    } else if (uri.find("case") == std::string::npos) {
        if (uri.find("path") != std::string::npos ||
            uri.find("steps") != std::string::npos ||
            uri.find("pedestrian") != std::string::npos) {

        } else if (uri.find("road") != std::string::npos) {
            hsla.s = 0.3    ;
            hsla.l *= hsla.l > 0.5 ? 0.7 : 1.2;
        } else if (uri.find("bridge") != std::string::npos) {
            hsla.s = 0.2;
            hsla.l *= hsla.l > 0.5 ? 0.6 : 1.3;
        } else if (uri.find("tunnel") != std::string::npos) {
            hsla.l *= hsla.l > 0.5 ? 0.9 : 1.1;
        }
    }

    return hsla;
}

}

void bind(const std::string& uri, const mbgl::Color& color, const std::function<void(const mbgl::Color& color)>& callback) {
    if (internal::isStyliable(color)) {
        Hsla hsla = internal::lightenAllRoads(uri, Hsla(color));
        paletteBindings.emplace_back(uri, Stylizer({colorBase, hsla}), callback);
    } else {
        Hsla hsla = internal::lightenAllRoads(uri, {internal::unwrap(color)});
        paletteBindings.emplace_back(uri, Stylizer(hsla), callback);
    }

}

bool update() {
    if (needUpdate > 0) {
        needUpdate--;
        
        if (needUpdate < 90) {
            static float h = 0;
            static float s = 0;
            static float l = 0;
            
            h += 1.;
            s += .02;
            l += .02;
            
            colorBase.h = fmod(h, 360.);
            colorBase.s = .2 + fabs(fmod(s, 1.8) - .9) * .8;
            colorBase.l = .4 + fabs(fmod(l, 1.) - .5) * .6;
            
            setColorBase(colorBase);
        }
        
        for (auto& it : paletteBindings) {
            it.callback(it.color.update());
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
