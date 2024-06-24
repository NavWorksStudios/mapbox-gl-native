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
#include "csscolorparser/csscolorparser.hpp"


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




namespace csscolor {

struct NamedColor { const char *const name; const CSSColorParser::Color color; };
const NamedColor namedColors[] = {
    { "transparent", { 0, 0, 0, 0 } }, { "aliceblue", { 240, 248, 255, 1 } },
    { "antiquewhite", { 250, 235, 215, 1 } }, { "aqua", { 0, 255, 255, 1 } },
    { "aquamarine", { 127, 255, 212, 1 } }, { "azure", { 240, 255, 255, 1 } },
    { "beige", { 245, 245, 220, 1 } }, { "bisque", { 255, 228, 196, 1 } },
    { "black", { 0, 0, 0, 1 } }, { "blanchedalmond", { 255, 235, 205, 1 } },
    { "blue", { 0, 0, 255, 1 } }, { "blueviolet", { 138, 43, 226, 1 } },
    { "brown", { 165, 42, 42, 1 } }, { "burlywood", { 222, 184, 135, 1 } },
    { "cadetblue", { 95, 158, 160, 1 } }, { "chartreuse", { 127, 255, 0, 1 } },
    { "chocolate", { 210, 105, 30, 1 } }, { "coral", { 255, 127, 80, 1 } },
    { "cornflowerblue", { 100, 149, 237, 1 } }, { "cornsilk", { 255, 248, 220, 1 } },
    { "crimson", { 220, 20, 60, 1 } }, { "cyan", { 0, 255, 255, 1 } },
    { "darkblue", { 0, 0, 139, 1 } }, { "darkcyan", { 0, 139, 139, 1 } },
    { "darkgoldenrod", { 184, 134, 11, 1 } }, { "darkgray", { 169, 169, 169, 1 } },
    { "darkgreen", { 0, 100, 0, 1 } }, { "darkgrey", { 169, 169, 169, 1 } },
    { "darkkhaki", { 189, 183, 107, 1 } }, { "darkmagenta", { 139, 0, 139, 1 } },
    { "darkolivegreen", { 85, 107, 47, 1 } }, { "darkorange", { 255, 140, 0, 1 } },
    { "darkorchid", { 153, 50, 204, 1 } }, { "darkred", { 139, 0, 0, 1 } },
    { "darksalmon", { 233, 150, 122, 1 } }, { "darkseagreen", { 143, 188, 143, 1 } },
    { "darkslateblue", { 72, 61, 139, 1 } }, { "darkslategray", { 47, 79, 79, 1 } },
    { "darkslategrey", { 47, 79, 79, 1 } }, { "darkturquoise", { 0, 206, 209, 1 } },
    { "darkviolet", { 148, 0, 211, 1 } }, { "deeppink", { 255, 20, 147, 1 } },
    { "deepskyblue", { 0, 191, 255, 1 } }, { "dimgray", { 105, 105, 105, 1 } },
    { "dimgrey", { 105, 105, 105, 1 } }, { "dodgerblue", { 30, 144, 255, 1 } },
    { "firebrick", { 178, 34, 34, 1 } }, { "floralwhite", { 255, 250, 240, 1 } },
    { "forestgreen", { 34, 139, 34, 1 } }, { "fuchsia", { 255, 0, 255, 1 } },
    { "gainsboro", { 220, 220, 220, 1 } }, { "ghostwhite", { 248, 248, 255, 1 } },
    { "gold", { 255, 215, 0, 1 } }, { "goldenrod", { 218, 165, 32, 1 } },
    { "gray", { 128, 128, 128, 1 } }, { "green", { 0, 128, 0, 1 } },
    { "greenyellow", { 173, 255, 47, 1 } }, { "grey", { 128, 128, 128, 1 } },
    { "honeydew", { 240, 255, 240, 1 } }, { "hotpink", { 255, 105, 180, 1 } },
    { "indianred", { 205, 92, 92, 1 } }, { "indigo", { 75, 0, 130, 1 } },
    { "ivory", { 255, 255, 240, 1 } }, { "khaki", { 240, 230, 140, 1 } },
    { "lavender", { 230, 230, 250, 1 } }, { "lavenderblush", { 255, 240, 245, 1 } },
    { "lawngreen", { 124, 252, 0, 1 } }, { "lemonchiffon", { 255, 250, 205, 1 } },
    { "lightblue", { 173, 216, 230, 1 } }, { "lightcoral", { 240, 128, 128, 1 } },
    { "lightcyan", { 224, 255, 255, 1 } }, { "lightgoldenrodyellow", { 250, 250, 210, 1 } },
    { "lightgray", { 211, 211, 211, 1 } }, { "lightgreen", { 144, 238, 144, 1 } },
    { "lightgrey", { 211, 211, 211, 1 } }, { "lightpink", { 255, 182, 193, 1 } },
    { "lightsalmon", { 255, 160, 122, 1 } }, { "lightseagreen", { 32, 178, 170, 1 } },
    { "lightskyblue", { 135, 206, 250, 1 } }, { "lightslategray", { 119, 136, 153, 1 } },
    { "lightslategrey", { 119, 136, 153, 1 } }, { "lightsteelblue", { 176, 196, 222, 1 } },
    { "lightyellow", { 255, 255, 224, 1 } }, { "lime", { 0, 255, 0, 1 } },
    { "limegreen", { 50, 205, 50, 1 } }, { "linen", { 250, 240, 230, 1 } },
    { "magenta", { 255, 0, 255, 1 } }, { "maroon", { 128, 0, 0, 1 } },
    { "mediumaquamarine", { 102, 205, 170, 1 } }, { "mediumblue", { 0, 0, 205, 1 } },
    { "mediumorchid", { 186, 85, 211, 1 } }, { "mediumpurple", { 147, 112, 219, 1 } },
    { "mediumseagreen", { 60, 179, 113, 1 } }, { "mediumslateblue", { 123, 104, 238, 1 } },
    { "mediumspringgreen", { 0, 250, 154, 1 } }, { "mediumturquoise", { 72, 209, 204, 1 } },
    { "mediumvioletred", { 199, 21, 133, 1 } }, { "midnightblue", { 25, 25, 112, 1 } },
    { "mintcream", { 245, 255, 250, 1 } }, { "mistyrose", { 255, 228, 225, 1 } },
    { "moccasin", { 255, 228, 181, 1 } }, { "navajowhite", { 255, 222, 173, 1 } },
    { "navy", { 0, 0, 128, 1 } }, { "oldlace", { 253, 245, 230, 1 } },
    { "olive", { 128, 128, 0, 1 } }, { "olivedrab", { 107, 142, 35, 1 } },
    { "orange", { 255, 165, 0, 1 } }, { "orangered", { 255, 69, 0, 1 } },
    { "orchid", { 218, 112, 214, 1 } }, { "palegoldenrod", { 238, 232, 170, 1 } },
    { "palegreen", { 152, 251, 152, 1 } }, { "paleturquoise", { 175, 238, 238, 1 } },
    { "palevioletred", { 219, 112, 147, 1 } }, { "papayawhip", { 255, 239, 213, 1 } },
    { "peachpuff", { 255, 218, 185, 1 } }, { "peru", { 205, 133, 63, 1 } },
    { "pink", { 255, 192, 203, 1 } }, { "plum", { 221, 160, 221, 1 } },
    { "powderblue", { 176, 224, 230, 1 } }, { "purple", { 128, 0, 128, 1 } },
    { "red", { 255, 0, 0, 1 } }, { "rosybrown", { 188, 143, 143, 1 } },
    { "royalblue", { 65, 105, 225, 1 } }, { "saddlebrown", { 139, 69, 19, 1 } },
    { "salmon", { 250, 128, 114, 1 } }, { "sandybrown", { 244, 164, 96, 1 } },
    { "seagreen", { 46, 139, 87, 1 } }, { "seashell", { 255, 245, 238, 1 } },
    { "sienna", { 160, 82, 45, 1 } }, { "silver", { 192, 192, 192, 1 } },
    { "skyblue", { 135, 206, 235, 1 } }, { "slateblue", { 106, 90, 205, 1 } },
    { "slategray", { 112, 128, 144, 1 } }, { "slategrey", { 112, 128, 144, 1 } },
    { "snow", { 255, 250, 250, 1 } }, { "springgreen", { 0, 255, 127, 1 } },
    { "steelblue", { 70, 130, 180, 1 } }, { "tan", { 210, 180, 140, 1 } },
    { "teal", { 0, 128, 128, 1 } }, { "thistle", { 216, 191, 216, 1 } },
    { "tomato", { 255, 99, 71, 1 } }, { "turquoise", { 64, 224, 208, 1 } },
    { "violet", { 238, 130, 238, 1 } }, { "wheat", { 245, 222, 179, 1 } },
    { "white", { 255, 255, 255, 1 } }, { "whitesmoke", { 245, 245, 245, 1 } },
    { "yellow", { 255, 255, 0, 1 } }, { "yellowgreen", { 154, 205, 50, 1 } }
};

mbgl::Color convert(const CSSColorParser::Color& color) {
    return mbgl::Color(color.r / 255., color.g / 255., color.b / 255., color.a / 255.);
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

struct StyleFormula {
    float hCoef=0, sCoef=0, lCoef=0, aCoef=0;
    
    StyleFormula() = default;

    StyleFormula(const Hsla& base, const Hsla& sample) {
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

class Color {
    Hsla hsla;
    mbgl::Color rgba;

public:
    Color() = default;
    
    Color(const mbgl::Color& color) : rgba(color) { hsla << rgba; }
    Color(const Hsla& color) : hsla(color) { hsla >> rgba; }

    void smoothto(const Color& color) {
        hsla.smoothto(color.hsla);
        hsla >> rgba;
    }
    
    void stylize(const Hsla& base, const StyleFormula& formula) {
        formula.stylize(base, hsla);
        hsla >> rgba;
    }

    inline const mbgl::Color& rgb() const { return rgba; }

};


class StyleColor {
    Color color;

    struct Style {
        enum Mode : uint8_t { fixed, stylized };
        Mode mode = Mode::fixed;
        Color color;
        StyleFormula formula;

        Style() = default;
        Style(const mbgl::Color& color) : mode(Mode::fixed), color(color) { }
        Style(const StyleFormula& formula) : mode(Mode::stylized), formula(formula) { }
        void operator = (const Hsla& base) {
            if (mode == stylized) color.stylize(base, formula);
        }
    } style;

public:
    StyleColor() = default;
    StyleColor(const mbgl::Color& color) : style(color) { }
    StyleColor(const StyleFormula& formula) : style(formula) { }
    inline void operator = (const Hsla& base) { style = base; }
    inline void update() { color.smoothto(style.color); }
    inline operator const mbgl::Color& () const { return color.rgb(); }
};


std::map<std::string, StyleColor> paletteColors;

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
    Hsla base;
    base << color;

    for (auto& it : paletteColors) {
        it.second = base;
    }

    needUpdate = 100;
}


template <typename T>
uint8_t clamp_css_byte(T i) {  // Clamp to integer 0 .. 255.
    i = ::round(i);  // Seems to be what Chrome does (vs truncation).
    return i < 0 ? 0 : i > 255 ? 255 : uint8_t(i);
}

template <typename T>
float clamp_css_float(T f) {  // Clamp to float 0.0 .. 1.0.
    return f < 0 ? 0 : f > 1 ? 1 : float(f);
}

float parseFloat(const std::string& str) {
    return strtof(str.c_str(), nullptr);
}

int64_t parseInt(const std::string& str, uint8_t base = 10) {
    return strtoll(str.c_str(), nullptr, base);
}

uint8_t parse_css_int(const std::string& str) {  // int or percentage.
    if (str.length() && str.back() == '%') {
        return clamp_css_byte(parseFloat(str) / 100.0f * 255.0f);
    } else {
        return clamp_css_byte(parseInt(str));
    }
}

float parse_css_float(const std::string& str) {  // float or percentage.
    if (str.length() && str.back() == '%') {
        return clamp_css_float(parseFloat(str) / 100.0f);
    } else {
        return clamp_css_float(parseFloat(str));
    }
}

std::vector<std::string> split(const std::string& s, char delim) {
    std::vector<std::string> elems;
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

void parseColorToPallete(const std::string& color) {
    std::string str = color;
    
    // Remove all whitespace, not compliant, but should just be more accepting.
    str.erase(std::remove(str.begin(), str.end(), ' '), str.end());
    
    // Convert to lowercase.
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
    
    for (const auto& namedColor : csscolor::namedColors) {
        if (str == namedColor.name) {
            paletteColors[color] = StyleColor(csscolor::convert(namedColor.color));
        }
    }
    
    // #abc and #abc123 syntax.
    if (str.length() && str.front() == '#') {
        if (str.length() == 4) {
            int64_t iv = parseInt(str.substr(1), 16);  // TODO(deanm): Stricter parsing.
            if (!(iv >= 0 && iv <= 0xfff)) {
                paletteColors[color] = StyleColor(mbgl::Color());
            } else {
                const mbgl::Color value = {
                    static_cast<float>(((iv & 0xf00) >> 4) | ((iv & 0xf00) >> 8)),
                    static_cast<float>((iv & 0xf0) | ((iv & 0xf0) >> 4)),
                    static_cast<float>((iv & 0xf) | ((iv & 0xf) << 4)),
                    1
                };
                paletteColors[color] = StyleColor(value);
            }
        } else if (str.length() == 7) {
            int64_t iv = parseInt(str.substr(1), 16);  // TODO(deanm): Stricter parsing.
            if (!(iv >= 0 && iv <= 0xffffff)) {
                // Covers NaN.
                paletteColors[color] = StyleColor(mbgl::Color());
            } else {
                const mbgl::Color value = {
                    static_cast<float>((iv & 0xff0000) >> 16),
                    static_cast<float>((iv & 0xff00) >> 8),
                    static_cast<float>(iv & 0xff),
                    1
                };
                paletteColors[color] = StyleColor(value);
            }
        }
        
        paletteColors[color] = StyleColor(mbgl::Color());
    }
    
    size_t op = str.find_first_of('('), ep = str.find_first_of(')');
    if (op != std::string::npos && ep + 1 == str.length()) {
        const std::string fname = str.substr(0, op);
        const std::vector<std::string> params = split(str.substr(op + 1, ep - (op + 1)), ',');
        
        float alpha = 1.0f;
        
        if (fname == "rgba" || fname == "rgb") {
            if (fname == "rgba") {
                if (params.size() != 4) {
                    paletteColors[color] = StyleColor(mbgl::Color());
                }
                alpha = parse_css_float(params.back());
            } else {
                if (params.size() != 3) {
                    paletteColors[color] = StyleColor(mbgl::Color());
                }
            }
            
            const mbgl::Color value = {
                static_cast<float>(parse_css_int(params[0])),
                static_cast<float>(parse_css_int(params[1])),
                static_cast<float>(parse_css_int(params[2])),
                alpha
            };
            
            paletteColors[color] = StyleColor(value);
        } else if (fname == "hsla" || fname == "hsl") {
            if (fname == "hsla") {
                if (params.size() != 4) {
                    paletteColors[color] = StyleColor(mbgl::Color());
                }
                alpha = parse_css_float(params.back());
            } else {
                if (params.size() != 3) {
                    paletteColors[color] = StyleColor(mbgl::Color());
                }
            }
            
            float h = parseFloat(params[0]) / 360.0f;
            float i;
            // Normalize the hue to [0..1[
            h = std::modf(h, &i);
            
            // NOTE(deanm): According to the CSS spec s/l should only be
            // percentages, but we don't bother and let float or percentage.
            float s = parse_css_float(params[1]);
            float l = parse_css_float(params[2]);
            paletteColors[color] = StyleColor({ h, s, l, alpha });
        }
    }
    
    paletteColors[color] = StyleColor(mbgl::Color());
}

const mbgl::Color& getColor(const std::string& color) {
    auto it = paletteColors.find(color);

    if (it == paletteColors.end()) {
        parseColorToPallete(color);
        it = paletteColors.find(color);
    }
    
    return it->second;
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
