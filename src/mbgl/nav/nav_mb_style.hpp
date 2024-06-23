//
//  nav_mb_style.hpp
//
//  Created by BernieZhao on 2023/12/13.
//

#pragma once

#include <mbgl/style/image.hpp>
#include <mbgl/gfx/texture.hpp>
#include <mbgl/gfx/upload_pass.hpp>
#include <mbgl/util/color.hpp>

namespace nav {
namespace style {

const std::string& url();

void loadAllImage(const std::string& path);
void releaseAllImage();

void uploadTexture(mbgl::gfx::UploadPass& uploadPass);
mbgl::gfx::TextureResource& getTexture(const std::string& name);

bool layerHasLineHeight(const std::string& layerId);

namespace rendertime {
double value();
}

namespace spotlight {
float value();
}

namespace landscape {
float value();
}

enum ViewMode : int8_t {
    Landscape,
    Normal,
    Spotlight,
};

void setViewMode(ViewMode mode);

namespace monochrome {
struct Hsl {
    float h=0, s=0, l=0, a=0;
    Hsl() = default;
    Hsl(const mbgl::Color& rgb);
    mbgl::Color rgb();
    bool operator == (const Hsl& hsl);
    void smoothto(const Hsl& to);
};
void setColorBase(const Hsl& base);
const std::vector<Hsl>& getColor(const std::string& layerId);
}

void update();

}
}
