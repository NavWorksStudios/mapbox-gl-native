//
//  nav_mb_style_displace.hpp
//
//  Created by BernieZhao on 2023/12/13.
//

#pragma once

#include <mbgl/style/layer.hpp>

namespace nav {
namespace mb {

void displaceStyle(const std::string& id, std::unique_ptr<::mbgl::style::Layer>& layer);

}
}
