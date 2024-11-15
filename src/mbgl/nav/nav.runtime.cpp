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
#include <OpenGL/gl.h>


namespace nav {

namespace render {
GLuint genTexture(GLint internalformat, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels=nullptr);
}

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


namespace sunlight {

std::array<float,3> _pos = { 1, 1, 1 };

const std::array<float,3>& pos() {
    return _pos;
}

void setPos(const std::array<float,3>& pos) {
    _pos = { -pos[0], -pos[1], pos[2] };
}

}

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
    std::string path;
    mbgl::optional<mbgl::gfx::Texture> texture;
};

std::string root;
std::unordered_map<std::string, ImageData> imageMap;
bool needUpload = true;

int32_t logo_texture = 0;
mbgl::Size logo_size;

void load(const std::string& path) {
    root = path;

    imageMap["dummy"].path = "dummy.png";
    
    imageMap["water"].path = "water.jpg";
    imageMap["water.normal"].path = "water.normal.jpg";
    imageMap["water.reflection"].path = "water.reflection.jpg";
    
    imageMap["grass.1"].path = "grass.1.png";
    imageMap["grass.1.normal"].path = "grass.1.normal.png";
    
    imageMap["grass.2"].path = "grass.2.jpg";
    imageMap["grass.2.normal"].path = "grass.2.normal.jpg";
    
    imageMap["grass.3"].path = "grass.3.jpg";
    imageMap["grass.3.normal"].path = "grass.3.normal.jpg";
}

void release() {
    imageMap.clear();
}

void upload(mbgl::gfx::UploadPass& uploadPass) {
    if (needUpload) {
        needUpload = !needUpload;
        
        for (auto& it : imageMap) {
            auto path = root + it.second.path;
            auto data = std::move(mbgl::util::read_file(path));
            mbgl::PremultipliedImage image = mbgl::decodeImage(data);
            it.second.texture = uploadPass.createTexture(image);
        }
        
        {
            auto path = root + "logo.png";
            auto data = std::move(mbgl::util::read_file(path));
            mbgl::PremultipliedImage image = mbgl::decodeImage(data);
            logo_texture = render::genTexture(GL_RGBA, image.size.width, image.size.height,
                                              GL_RGBA, GL_UNSIGNED_BYTE, (const GLvoid*) image.data.get());
            logo_size = image.size;
        }
    }
}

mbgl::gfx::Texture& get(const std::string& name) {
    if (name.length() > 0) {
        return *imageMap[name].texture;
    } else {
        return *imageMap["dummy"].texture;
    }
}

std::tuple<mbgl::Size, int32_t> logo() {
    return { logo_size, logo_texture };
}

}

}

}
