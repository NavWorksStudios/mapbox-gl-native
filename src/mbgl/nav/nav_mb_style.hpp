//
//  nav_mb_style.hpp
//
//  Created by BernieZhao on 2023/12/13.
//

#pragma once

#include <mbgl/gfx/texture.hpp>
#include <mbgl/gfx/upload_pass.hpp>
#include <mbgl/util/color.hpp>

namespace nav {

namespace style {

const std::string& url();
const std::string& accessToken();

namespace display {
constexpr size_t width = 2048;
constexpr size_t height = 1080;
void update(float zoom);
float clip_region();
float focus_region();
}

struct Domain {
    Domain(const std::string& name);
    ~Domain();
    operator const std::string& () const;
private:
    const size_t pos;
};

namespace texture {
void load(const std::string& path);
void release();
void upload(mbgl::gfx::UploadPass& uploadPass);
mbgl::gfx::TextureResource& get(const std::string& name);
}

}

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
    Normal,
    Spotlight,
};

void setViewMode(ViewMode mode);

bool update();
bool needsUpdate();

}

}
