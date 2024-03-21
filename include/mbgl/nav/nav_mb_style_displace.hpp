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

void displaceStyle(std::unique_ptr<::mbgl::style::Layer>& layer) {
    const auto id = layer->getID();
    if (id == "land") {
        auto backgroundLayer = static_cast<::mbgl::style::BackgroundLayer*>(layer.get());
        auto color = backgroundLayer->getBackgroundColor();
        backgroundLayer->setBackgroundColor(color);
    } else if (id == "water") {
        auto fillLayer = static_cast<::mbgl::style::FillLayer*>(layer.get());
        mbgl::style::PropertyValue<mbgl::Color> color = fillLayer->getFillColor();
        mbgl::style::PropertyValue<mbgl::Color> c = mbgl::Color(1, 1, 1, 1);
        fillLayer->setFillColor(c);
    }
}

}

}
