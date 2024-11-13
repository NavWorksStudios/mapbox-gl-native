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

namespace sunlight {
const std::array<float,3>& pos();
void setPos(const std::array<float,3>& dir); // 太阳坐标系和世界坐标系xy值反向
}

namespace spotlight {
float value();
}

namespace landscape {
float value();
}

namespace quality {
void enable();
void disable();
float value(); // (0., 1.)
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
mbgl::gfx::Texture& get(const std::string& name);

std::tuple<mbgl::Size,int32_t> logo();
}

}

}
