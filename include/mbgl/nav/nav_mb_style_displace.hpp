//
//  nav_mb_style_displace.hpp
//
//  Created by BernieZhao on 2023/12/13.
//

#pragma once

#include <mbgl/style/layer.hpp>

#include <mbgl/style/layers/background_layer.hpp>
#include <mbgl/style/layers/fill_layer.hpp>

namespace nav {
namespace mb {
namespace layer {

const char* const ID_NAV_LAND = "nav:land";
const char* const ID_NAV_3DLAND = "nav:3d-land";

inline void displaceStyle(const std::string& id, std::unique_ptr<::mbgl::style::Layer>& layer) {
    if (id == "water") {
        auto fillLayer = static_cast<::mbgl::style::FillLayer*>(layer.get());
        mbgl::style::PropertyValue<mbgl::Color> color = fillLayer->getFillColor();
        mbgl::style::PropertyValue<mbgl::Color> new_fill_color = mbgl::Color(0.203,0.325,0.396,1);
        fillLayer->setFillColor(new_fill_color);
    } else if (id == ID_NAV_LAND) {
        auto fillLayer = static_cast<::mbgl::style::FillLayer*>(layer.get());
        mbgl::style::PropertyValue<mbgl::Color> new_fill_color = mbgl::Color(1,0,1,1);
        fillLayer->setFillColor(new_fill_color);
    }
}

}
}
}
