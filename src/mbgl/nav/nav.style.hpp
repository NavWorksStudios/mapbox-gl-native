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
void set(int width, int height);
}

namespace pixels {
int width();
int height();
void setRatio(float ratio);
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

}
