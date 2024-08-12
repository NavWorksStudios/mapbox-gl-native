//
//  nav.palette.cpp
//
//  Created by BernieZhao on 2024/7/9.
//

#include "mbgl/nav/nav.palette.hpp"
#include "mbgl/nav/nav.theme.hpp"
#include "mbgl/nav/nav.log.hpp"


namespace nav {

namespace palette {

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
        isNeedsupdate = !((Hsla&)*this == target);
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

bool enableLayerMonoPalette(const std::string& layer) {
    return theme::enableLayerMonoPalette(layer);
}

class Stylizer {
    bool stylible = false;
    float hcoef=0, scoef=0, lcoef=0, acoef=0;

public:
    Stylizer() = default;

    Stylizer(const Hsla& color, bool stylible) : stylible(stylible) {
        if (stylible) {
            static const Hsla SAMPLE_CENTER = { 200., .5, .5, 1. };
            
            hcoef = color.h - SAMPLE_CENTER.h;
            scoef = color.s - SAMPLE_CENTER.s;
            lcoef = log2(color.l) / log2(SAMPLE_CENTER.l);
            acoef = color.a / SAMPLE_CENTER.a;
            
            assert(!std::isnan(hcoef));
            assert(!std::isnan(scoef));
            assert(!std::isnan(lcoef));
            assert(!std::isnan(acoef));
        } else {
            hcoef = color.h;
            scoef = color.s;
            lcoef = color.l;
            acoef = color.a;
        }
    }
    
    Hsla operator () (const Hsla& baseColor) {
        if (stylible) {
            return Hsla(
                fmod(baseColor.h + hcoef + 360., 360.),     // h [0,360]
                fmin(fmax(baseColor.s + scoef, 0.), 1.),    // s [0,1]
                pow(baseColor.l, lcoef),                    // l [0,1]
                fmin(fmax(baseColor.a * acoef, 0.), 1.)     // a [0,1]
            );
        } else {
            return Hsla(hcoef, scoef, lcoef, acoef);
        }
    }
};

class ColorBinding {
    std::string uri;
    Stylizer stylizer;
    Binding callback;
    
public:
    const void* binder;
    
    ColorBinding() = default;
    ColorBinding(const std::string& uri, const Stylizer& stylizer, const void* binder, const Binding& callback) :
    uri(uri), stylizer(stylizer), binder(binder), callback(callback) { }
    
    void notify(const Hsla& baseColor) {
        Hsla hsla = stylizer(baseColor);
        mbgl::Color color;
        hsla >> color;
        callback(color);
    }
};

std::vector<ColorBinding> paletteBindings;

void bind(const std::string& uri, const mbgl::Color& color, const void* binder, const Binding& callback) {
    nav::log::i("Palette", "bind uri %s (%d,%d,%d)", uri.c_str(), (int)(color.r*255), (int)(color.g*255), (int)(color.b*255));
    if (uri.find("mapbox") != std::string::npos) return;

    const std::tuple<Hsla,bool>&& cfg = theme::colorProperty(uri, color);
    ColorBinding binding(uri, Stylizer(std::get<0>(cfg), std::get<1>(cfg)), binder, callback);
    binding.notify(themeBaseColor);
    paletteBindings.emplace_back(binding);
}

void unbind(const void* binder) {
    auto it = paletteBindings.begin();
    while (it != paletteBindings.end()) {
        if (it->binder == binder) {
            paletteBindings.erase(it);
        } else {
            it++;
        }
    }
}

bool demo() {
    if (!theme::needsUpdate()) return false;

    static int counter = 0;
    if (counter++ > 30) {
        counter = 0;

        static float h = 0;
        static float s = 0;
        static float l = 0;

        static auto wave = [] (float v, float min, float max) {
            const float step = max - min;
            return min + fabs(fmod(v, step * 2) - step);

        };

        Hsla color(50, .5, .5);
        color.h = fmod(h += 9., 360.);
        color.s = .5 + .3 * wave(s += .05, 0, 1);
        color.l = .3 + .5 * wave(l += .1, 0, 1);
        themeBaseColor.smoothTo(color);
    }
    
    return true;
}

bool update() {
    if (themeBaseColor.needsUpdate()) {
        themeBaseColor.update();

        for (auto& it : paletteBindings) {
            it.notify(themeBaseColor);
        }
    }
    
    bool needsUpdate = false;
    needsUpdate |= demo();
    needsUpdate |= themeBaseColor.needsUpdate();
    return needsUpdate;
}

}

}
