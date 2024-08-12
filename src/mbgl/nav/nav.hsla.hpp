//
//  nav.hsla.hpp
//
//  Created by BernieZhao on 2024/8/5.
//

#pragma once

#include <mbgl/util/color.hpp>

namespace nav {

struct Hsla {
    float h=0., s=0., l=0., a=0.;

    Hsla() = default;
    Hsla(float h, float s, float l, float a=1.);
    Hsla(const mbgl::Color& color);

    void operator >> (mbgl::Color& rgba) const;
    void operator << (const mbgl::Color& rgba);
    void mix(const Hsla& to, float ratio);
    bool operator == (const Hsla& other) const;
};

}
