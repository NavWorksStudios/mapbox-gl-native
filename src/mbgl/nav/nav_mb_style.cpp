//
//  nav_mb_style.cpp
//
//  Created by BernieZhao on 2023/12/13.
//

#include "mbgl/nav/nav_mb_style.hpp"
#include "mbgl/nav/nav_mb_layer.hpp"

#include <mbgl/style/layers/background_layer.hpp>
#include <mbgl/style/layers/fill_layer.hpp>
#include <mbgl/style/layers/line_layer.hpp>
#include <mbgl/style/layers/fill_extrusion_layer.hpp>
#include <mbgl/util/io.hpp>

#include "mbgl/nav/nav_log.hpp"

namespace nav {
namespace style {

const std::string& url() {
    static std::string url(
//        "mapbox://styles/notheorem/clxk5j3qw00av01qq93udh6bm"
        "mapbox://styles/notheorem/clwvwdd2r01d301nyayhc6p6g"
    );
    
    return url;
}

struct ImageData {
    mbgl::PremultipliedImage image;
    mbgl::optional<mbgl::gfx::Texture> texture { mbgl::nullopt };
};

std::map<std::string, ImageData> imageMap;

void loadAllImage(const std::string& path) {
    imageMap["matcap.0.png"].image = mbgl::decodeImage(mbgl::util::read_file(path + "matcap.0.png"));
    imageMap["matcap.1.png"].image = mbgl::decodeImage(mbgl::util::read_file(path + "matcap.1.png"));
    imageMap["matcap.2.png"].image = mbgl::decodeImage(mbgl::util::read_file(path + "matcap.2.png"));
    imageMap["matcap.3.png"].image = mbgl::decodeImage(mbgl::util::read_file(path + "matcap.3.png"));
    imageMap["matcap.4.png"].image = mbgl::decodeImage(mbgl::util::read_file(path + "matcap.4.png"));
}

void releaseAllImage() {
    imageMap.clear();
}

void uploadTexture(mbgl::gfx::UploadPass& uploadPass) {
    for (auto& i : imageMap) {
        if (!i.second.texture) {
            i.second.texture = uploadPass.createTexture(i.second.image);
        }
    }
}

mbgl::gfx::TextureResource& getTexture(const std::string& name) {
    return imageMap[name].texture->getResource();
}

std::map<std::string, int16_t> layerHeightMap = {
//    { "tunnel-path-trail", 0 },
//    { "tunnel-path-cycleway-piste", 0 },
//    { "tunnel-path", 0 },
//    { "tunnel-steps", 0 },
//    { "tunnel-pedestrian", 0 },
//    { "tunnel-simple", 0 },
    
//    { "bridge-path-trail", 0 },
//    { "bridge-path-cycleway-piste", 0 },
//    { "bridge-path", 0 },
//    { "bridge-steps", 0 },
//    { "bridge-pedestrian", 0 },
    
    { "bridge-case-simple", 3 },
    { "bridge-simple", 3 },

//    { "bridge-rail", 0 },
//    { "bridge-rail-tracks", 0 },
};

bool layerHasLineHeight(const std::string& layerId) {
    return layerHeightMap.find(layerId) != layerHeightMap.end();
}




namespace rendertime {

double time_seconds;

void update() {
    const auto now = std::chrono::steady_clock::now();
    time_seconds = std::chrono::duration_cast<std::chrono::duration<double>>(now.time_since_epoch()).count();
}

double value() {
    return time_seconds;
}

}

struct ToggleValue {
    bool _enabled = false;
    float _ratio = .5;

    void update() {
        if (_enabled) {
            if (_ratio < 1.) {
                _ratio = fmin(_ratio + fmax((1. - _ratio) * 0.2, 0.02), 1.);
            }
        } else {
            if (_ratio > 0.) {
                _ratio = fmax(_ratio - fmax((_ratio - 0.) * 0.05, 0.005), 0.);
            }
        }
        
//        nav::log::i("ToggleValue", "%f", _ratio);
    }

    void enable() { _enabled = true; }
    void disable() { _enabled = false; }
    operator float () { return _ratio; }
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

void update() {
    rendertime::update();
    spotlight::toggle.update();
    landscape::toggle.update();
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



}
}
