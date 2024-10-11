//
//  nav.runtime.hpp
//
//  Created by BernieZhao on 2024/10/11.
//

#pragma once

#include <mbgl/gfx/texture.hpp>
#include <mbgl/gfx/upload_pass.hpp>
#include <mbgl/util/color.hpp>

namespace nav {

namespace runtime {

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
Browse,
Spotlight,
};

void setViewMode(ViewMode mode);

bool update();
bool needsUpdate();

namespace texture {
void load(const std::string& path);
void release();
void upload(mbgl::gfx::UploadPass& uploadPass);
mbgl::gfx::TextureResource& get(const std::string& name);
}

}

}
