//
//  nav_mb_style.cpp
//
//  Created by BernieZhao on 2023/12/13.
//

#include "mbgl/nav/nav_mb_style.hpp"
#include "mbgl/nav/nav_mb_palette.hpp"

#include <mbgl/style/image.hpp>
#include <mbgl/util/io.hpp>

#include <cmath>

#include "mbgl/nav/nav_log.hpp"


namespace nav {

namespace palette {
bool update();
}

namespace style {

const std::string& url() {
    static std::string url(
        // 主题模式
//        "mapbox://styles/notheorem/clxu5ehnm00ro01qqhhim0d4f" // dark
//        "mapbox://styles/notheorem/clxrac32800o901qw94ryfkdz" // light

        // 色彩模式
        "mapbox://styles/navworks/clxx105i700yr01po4zbn2jc1"
    );

    return url;
}

const std::string& accessToken() {
    static std::string token = "pk.eyJ1IjoibmF2d29ya3MiLCJhIjoiY2x5M3U3MG96MDF5ZjJsb2lsMXZsczUxayJ9.Mnts5N9BU9OBsRb8fHauBQ";
    return token;
}

namespace display {

float clipping = 0.;

void update(float zoom) {
    static float region = height * atan(70/M_PI) * .1;
    clipping = region * pow(2., zoom);
}

float clip_region() {
    return clipping;
}

float focus_region() {
    static float region = (pow(width, 2) + pow(height,2));
    return region;
}

}

namespace texture {

struct ImageData {
    mbgl::PremultipliedImage image;
    mbgl::optional<mbgl::gfx::Texture> texture { mbgl::nullopt };
};

std::map<std::string, ImageData> imageMap;

void load(const std::string& path) {
    imageMap["gray_noise_medium"].image = mbgl::decodeImage(mbgl::util::read_file(path + "gray_noise_medium.png"));
}

void release() {
    imageMap.clear();
}

void upload(mbgl::gfx::UploadPass& uploadPass) {
    for (auto& i : imageMap) {
        if (!i.second.texture) {
            i.second.texture = uploadPass.createTexture(i.second.image);
        }
    }
}

mbgl::gfx::TextureResource& get(const std::string& name) {
    return imageMap[name].texture->getResource();
}

}

namespace rendertime {

struct Time {
    double time_seconds;

    void update() {
        const auto now = std::chrono::steady_clock::now();
        time_seconds = std::chrono::duration_cast<std::chrono::duration<double>>(now.time_since_epoch()).count();
    }
    
    operator double () const { return time_seconds; }
};

Time timestamp;

double value() {
    return timestamp;
}

}


struct ToggleValue {
    bool _enabled = false;
    float _ratio = .5;

    bool update() {
        if (_enabled) {
            if (_ratio < 1.) {
                _ratio = fmin(_ratio + fmax((1. - _ratio) * 0.1, 0.01), 1.);
                return true;
            }
            return false;
        } else {
            if (_ratio > 0.) {
                _ratio = fmax(_ratio - fmax((_ratio - 0.) * 0.015, 0.0015), 0.);
                return true;
            }
            return false;
        }
    }

    void enable() { _enabled = true; }
    void disable() { _enabled = false; }
    operator float () const { return _ratio; }
};


namespace spotlight {

ToggleValue toggle;

float value() {
    return toggle;
}

}

namespace landscape {

ToggleValue toggle;

float value() {
    return toggle;
}

}

void setViewMode(ViewMode mode) {
    switch (mode) {
        case Landscape:
            spotlight::toggle.disable();
            landscape::toggle.enable();
            break;
            
        case Normal:
            spotlight::toggle.disable();
            landscape::toggle.disable();
            break;
            
        case Spotlight:
            spotlight::toggle.enable();
            landscape::toggle.disable();
            break;
            
        default:
            break;
    }
}

bool isNeedUpdate = true;

bool update() {
    rendertime::timestamp.update();

    bool oncemore = false;

    oncemore |= spotlight::toggle.update();
    oncemore |= landscape::toggle.update();
    oncemore |= nav::palette::update();
    
    return isNeedUpdate = oncemore;
}

bool needsUpdate() {
    return isNeedUpdate;
}


}
}
