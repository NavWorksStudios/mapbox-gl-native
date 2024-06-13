//
//  nav_mb_style_displace.hpp
//
//  Created by BernieZhao on 2023/12/13.
//

#pragma once

#include <mbgl/util/color.hpp>
#include <mbgl/style/layer.hpp>
#include <mbgl/style/image.hpp>
#include <mbgl/gfx/texture.hpp>
#include <mbgl/gfx/upload_pass.hpp>

namespace nav {
namespace mb {

mbgl::Color land_color();

void displaceStyle(const std::string& id, std::unique_ptr<::mbgl::style::Layer>& layer);

void loadAllImage(const std::string& path);
void releaseAllImage();

void uploadTexture(mbgl::gfx::UploadPass& uploadPass);
mbgl::gfx::TextureResource& getTexture(const std::string& name);

bool layerHasLineHeight(const std::string& layerId);

namespace rendertime {
double value();
}

namespace spotlight {
void enable(bool enabled);
float value();
}

void update();

}
}
