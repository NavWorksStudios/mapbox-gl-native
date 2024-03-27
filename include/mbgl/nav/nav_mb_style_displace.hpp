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

static void displaceStyle(std::unique_ptr<::mbgl::style::Layer>& layer) {
    const auto id = layer->getID();
    if (id == "land") {
        auto backgroundLayer = static_cast<::mbgl::style::BackgroundLayer*>(layer.get());
        auto bk_color = backgroundLayer->getBackgroundColor();
        backgroundLayer->setBackgroundColor(bk_color);
    } else if (id == "water") {
        auto fillLayer = static_cast<::mbgl::style::FillLayer*>(layer.get());
//        mbgl::style::PropertyValue<mbgl::Color> color = fillLayer->getFillColor();
        mbgl::style::PropertyValue<mbgl::Color> new_fill_color = mbgl::Color(0.203,0.325,0.396,1);
        fillLayer->setFillColor(new_fill_color);
    }
}

}

}
