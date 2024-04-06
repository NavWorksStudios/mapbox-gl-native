//
//  nav_mb_style.cpp
//
//  Created by BernieZhao on 2023/12/13.
//

#include "mbgl/nav/nav_mb_style.hpp"
#include "mbgl/nav/nav_mb_layer.hpp"

#include <mbgl/style/layers/background_layer.hpp>
#include <mbgl/style/layers/fill_layer.hpp>
#include <mbgl/style/layers/fill_extrusion_layer.hpp>

namespace nav {
namespace mb {

void displaceStyle(const std::string& id, std::unique_ptr<::mbgl::style::Layer>& layer) {
    if (id == "water") {
        auto fillLayer = static_cast<::mbgl::style::FillLayer*>(layer.get());
        fillLayer->setFillColor(mbgl::Color(0.203,0.325,0.396,1));
    } else if (id == ID_NAV_LAND) {
        auto fillLayer = static_cast<::mbgl::style::FillLayer*>(layer.get());
        mbgl::style::PropertyValue<mbgl::Color> color = mbgl::Color(1,1,1,1);
        fillLayer->setFillColor(color);
    }
}

}
}
