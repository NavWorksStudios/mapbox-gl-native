//
//  nav_mb_palette.cpp
//
//  Created by BernieZhao on 2024/7/9.
//

#include "mbgl/nav/nav_mb_palette.hpp"
#include "mbgl/nav/nav_log.hpp"


namespace nav {

namespace palette {

float hue_to_rgb(float p, float q, float t) {
    if (t < 0.) t += 1.;
    if (t > 1.) t -= 1.;
    if (t * 6. < 1.) return p + (q - p) * 6. * t;
    if (t * 2. < 1.) return q;
    if (t * 3. < 2.) return p + (q - p) * (2./3. - t) * 6.;
    return p;
}

template <int MAX> float mix(float a, float b, const float ratio) {
    float dis = fabs(a - b);
    if (dis < MAX * 0.002) {
        return b;
    } else if (dis > MAX * 0.5) {
        if (a > b) {
            // ... max,min ...
            //   |         |
            //   a    ->   b
            //   a -> b + MAX
            b += MAX;
        } else {
            // ... max,min ...
            //   |         |
            //   b   <-    a
            //   a -> b - MAX
            b -= MAX;
        }
    }
    
    float value = a * (1. - ratio) + b * ratio;
    if (value > MAX) value -= MAX;
    else if (value < 0) value += MAX;
    return value;
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
};

class GradientColor : public Hsla, public mbgl::Color {
    bool isNeedsupdate = true;
    Hsla target;

public:
    GradientColor(const Hsla& color) : Hsla(color), target(color) {
        *this >> (mbgl::Color&) *this;
    }

    inline void smoothTo(const Hsla& color) {
        isNeedsupdate = true;
        target = color;
        *this >> (mbgl::Color&) *this;
    }

    inline bool update() {
        mix(target, .02);
        *this >> (mbgl::Color&) *this;
        
        isNeedsupdate = (Hsla::h != target.h ||
                         Hsla::s != target.s ||
                         Hsla::l != target.l ||
                         Hsla::a != target.a);

        return isNeedsupdate;
    }

    inline bool needsUpdate() const { return isNeedsupdate; }
};

GradientColor themeBaseColor = GradientColor({ 0., .5, .5, 1. });

void setColorBase(const mbgl::Color& color) {
    themeBaseColor.smoothTo(color);
}

const mbgl::Color& getColorBase() {
    return themeBaseColor;
}

bool enableShaderPalette(const std::string& id) {
    static std::map<std::string, bool> layerIds = {
        { "water-depth", true },
        { "hillshade", true },
    };

    return layerIds.find(id) != layerIds.end();
}

class Stylizer {
    bool fixed = false;
    float hcoef=0, scoef=0, lcoef=0, acoef=0;

public:
    Stylizer() = default;

    Stylizer(const Hsla& color, bool fixed) : fixed(fixed) {
        if (fixed) {
            hcoef = color.h;
            scoef = color.s;
            lcoef = color.l;
            acoef = color.a;
        } else {
            static const Hsla SAMPLE_CENTER = { 200., .5, .5, 1. };
            
            hcoef = color.h - SAMPLE_CENTER.h;
            scoef = color.s - SAMPLE_CENTER.s;
            lcoef = log2(color.l) / log2(SAMPLE_CENTER.l);
            acoef = color.a / SAMPLE_CENTER.a;
            
            assert(!std::isnan(hcoef));
            assert(!std::isnan(scoef));
            assert(!std::isnan(lcoef));
            assert(!std::isnan(acoef));
        }
    }
    
    Hsla operator () (const Hsla& baseColor) {
        if (fixed) {
            return Hsla(hcoef, scoef, lcoef, acoef);
        } else {
            return Hsla(
                fmod(baseColor.h + hcoef + 360., 360.),     // h [0,360]
                fmin(fmax(baseColor.s + scoef, 0.), 1.),    // s [0,1]
                pow(baseColor.l, lcoef),                    // l [0,1]
                fmin(fmax(baseColor.a * acoef, 0.), 1.)     // a [0,1]
            );
        }
    }
};

class ColorBinding {
    std::string uri;
    Stylizer stylizer;
    Binding callback;
    
public:
    ColorBinding() = default;
    ColorBinding(const std::string& uri, const Stylizer& stylizer, const Binding& callback) :
    uri(uri), stylizer(stylizer), callback(callback) { }
    
    void notify(const Hsla& baseColor) {
        Hsla hsla = stylizer(baseColor);
        mbgl::Color color;
        hsla >> color;
        callback(color);
    }
};

std::vector<ColorBinding> paletteBindings;
bool somebodyNeedsUpdate = false;

void bind(const std::string& uri, const mbgl::Color& color, const Binding& callback) {
    nav::log::i("Palette", "bind uri %s", uri.c_str());

    static const auto prefixColor = [] (const std::string& uri, const Hsla& color) {
        Hsla hsla = color;
        
        if (uri.find("water-depth") != std::string::npos) {
            hsla.s = 0;
        } else if(uri.find("hillshade") != std::string::npos) {
            hsla.s = 0;
        } else if (uri.find("building-extrusion") != std::string::npos) {

        } else if (uri.find("case") == std::string::npos) {
            if (uri.find("path") != std::string::npos ||
                uri.find("steps") != std::string::npos ||
                uri.find("pedestrian") != std::string::npos) {

            } else if (uri.find("road") != std::string::npos) {
                hsla.s = .2;
                hsla.l *= hsla.l > .5 ? .7 : 1.3;
            } else if (uri.find("bridge") != std::string::npos) {
                hsla.s = 0.1;
                hsla.l *= hsla.l > .5 ? .6 : 1.4;
            } else if (uri.find("tunnel") != std::string::npos) {
                hsla.l *= hsla.l > .5 ? .9 : 1.1;
            }
        }

        return hsla;
    };

    const Hsla hsla = prefixColor(uri, color);
    
    static const auto isFixed = [] (const std::string& uri, const mbgl::Color& color) {
        if (uri.find("water-depth") != std::string::npos || uri.find("hillshade") != std::string::npos) {
            return true;
        } else {
            return false;
        }
    };

    const bool fixed = isFixed(uri, color);
    ColorBinding binding(uri, Stylizer(hsla, fixed), callback);
    paletteBindings.emplace_back(binding);
    
    somebodyNeedsUpdate = true;
}

bool update() {
    if (somebodyNeedsUpdate || themeBaseColor.needsUpdate()) {
        somebodyNeedsUpdate = false;
        themeBaseColor.update();

        for (auto& it : paletteBindings) {
            it.notify(themeBaseColor);
        }
    }
    
    return themeBaseColor.needsUpdate();
}

bool demo() {
    static int counter = 0;
    if (counter++ > 50) {
        counter = 0;

        static float h = 0;
        static float s = 0;
        static float l = 0;

        h += 18.;
        s += .1;
        l += .1;

        Hsla color;
        color.h = fmod(h, 360.);
        color.s = .0 + .9 * fabs(fmod(s, 1.) - .5) / .5;
        color.l = .2 + .6 * fabs(fmod(l, 2.) - 1.) / 1.;
        color.a = 1.;

        themeBaseColor.smoothTo(color);
    }
    
    return true;
}

}

}
