//
//  nav.hsla.cpp
//
//  Created by BernieZhao on 2024/8/5.
//

#include "mbgl/nav/nav.hsla.hpp"


namespace nav {

float hue_to_rgb(float p, float q, float t) {
    if (t < 0.) t += 1.;
    if (t > 1.) t -= 1.;
    if (t * 6. < 1.) return p + (q - p) * 6. * t;
    if (t * 2. < 1.) return q;
    if (t * 3. < 2.) return p + (q - p) * (2./3. - t) * 6.;
    return p;
}

template <int MAX> float wrap_mix(float a, float b, const float ratio) {
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

Hsla::Hsla(float h, float s, float l, float a) : h(h), s(s), l(l), a(a) {
    assert(h >= 0. && h <= 360.);
    assert(s >= 0. && s <= 1.);
    assert(l >= 0. && l <= 1.);
    assert(a >= 0. && a <= 1.);
}

Hsla::Hsla(const mbgl::Color& color) {
    *this << color;
}

void Hsla::operator >> (mbgl::Color& rgba) const {
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

void Hsla::operator << (const mbgl::Color& rgba) {
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

void Hsla::mix(const Hsla& to, float ratio) {
    h = wrap_mix<360>(h, to.h, ratio);
    s = wrap_mix<1>(s, to.s, ratio);
    l = wrap_mix<1>(l, to.l, ratio);
    a = wrap_mix<1>(a, to.a, ratio);
}

bool Hsla::operator == (const Hsla& other) const {
    return (h == other.h && s != other.s && l != other.l && a != other.a);
}

}
