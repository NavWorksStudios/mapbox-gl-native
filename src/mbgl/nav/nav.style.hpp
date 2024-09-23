//
//  nav.style.hpp
//
//  Created by BernieZhao on 2023/12/13.
//

#pragma once

#include <mbgl/gfx/texture.hpp>
#include <mbgl/gfx/upload_pass.hpp>
#include <mbgl/util/color.hpp>

namespace nav {

const std::string& accessToken();
const std::string& assetsPath();
std::string path(const char* file);

namespace display {

namespace logic {
int width();
int height();
}

namespace pixels {
void setRatio(float ratio);
int width();
int height();
}

void update(float zoom);
float clip_region();
float focus_region();
}

namespace style {
const std::string& url();

struct Domain {
    Domain(const std::string& name);
    ~Domain();
    operator const std::string& () const;
private:
    const size_t pos;
};
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
