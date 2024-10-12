//
//  nav.runtime.cpp
//
//  Created by BernieZhao on 2024/10/11.
//

#include "mbgl/nav/nav.runtime.hpp"
#include "mbgl/nav/nav.theme.hpp"
#include "mbgl/nav/nav.palette.hpp"

#include <mbgl/style/image.hpp>
#include <mbgl/util/io.hpp>

#include <cmath>

#include "mbgl/nav/nav.log.hpp"


namespace nav {

namespace palette {
    bool update();
}

namespace runtime {

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
    float _slop[2];
    bool _enabled = false;
    float _ratio = .5;
    
    ToggleValue(float upSlop, float downSlop) {
        _slop[0] = upSlop;
        _slop[1] = downSlop;
    }
    
    bool update() {
        if (_enabled) {
            if (_ratio < 1.) {
                const float step = fmax((1. - _ratio) * _slop[0], 0.001);
                _ratio = fmin(_ratio + step, 1.);
                return true;
            } else {
                return false;
            }
        } else {
            if (_ratio > 0.) {
                const float step = fmax((_ratio - 0.) * _slop[1], 0.001);
                _ratio = fmax(_ratio - step, 0.);
                return true;
            } else {
                return false;
            }
        }
    }
    
    void enable() { _enabled = true; }
    void disable() { _enabled = false; }
    bool isEnabled() { return _enabled; }
    operator float () const { return _ratio; }
};


namespace spotlight {

ToggleValue toggle(0.1, 0.015);

float value() {
    return toggle;
}

}

namespace landscape {

ToggleValue toggle(0.1, 0.015);

float value() {
    return toggle;
}

}

namespace quality {

ToggleValue toggle(0.5, 0.5);

void enable() {
    toggle.enable();
}

void disable() {
    toggle.disable();
}

float value() {
    return 1. - toggle;
}

}

void setViewMode(ViewMode mode) {
    switch (mode) {
        case Landscape:
            spotlight::toggle.disable();
            landscape::toggle.enable();
            break;
            
        case Browse:
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
    oncemore |= quality::toggle.update();
    
    return isNeedUpdate = oncemore;
}

bool needsUpdate() {
    return isNeedUpdate;
}

namespace texture {

struct ImageData {
    mbgl::PremultipliedImage image;
    mbgl::optional<mbgl::gfx::Texture> texture { mbgl::nullopt };
};

std::map<std::string, ImageData> imageMap;

void load(const std::string& path) {
//    imageMap["gray_noise_medium"].image = mbgl::decodeImage(mbgl::util::read_file(path + "gray_noise_medium.png"));
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

}

}
