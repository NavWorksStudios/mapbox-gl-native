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
                           
                           // 开发测试
                           "mapbox://styles/notheorem/clxonzrtz00k701qq00fj6rnh" // dark
//                           "mapbox://styles/notheorem/clxoo5n7100lh01qw47x0fdhe" // light
                           
                           
                           // 正式 0 - 10
//                           "mapbox://styles/notheorem/clxk5j3qw00av01qq93udh6bm"
//                           "mapbox://styles/notheorem/clxolet6000jx01qq4un34jep"
//                           "mapbox://styles/notheorem/clxo6t3l800l501qma3lk7kku"
//                           "mapbox://styles/notheorem/clxo6u08m00l601qme55bahbs"
//                           "mapbox://styles/notheorem/clxo6ym3j00k501r2dgm7d26j"
//                           "mapbox://styles/notheorem/clxo70o4q00l701qmekyz04x5"
//                           "mapbox://styles/notheorem/clxo72llu00j101qqgeb55xlw"
//                           "mapbox://styles/notheorem/clxo77xld004501r0ae6s94fz"
//                           "mapbox://styles/notheorem/clxo7bjja00kn01qrdm5k6o1a"
//                           "mapbox://styles/notheorem/clxo7dspj00ko01qrhny81uem"
//                           "mapbox://styles/notheorem/clxo7gf6000l301pf067zcqu9"
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


bool layerHasLineHeight(const std::string& layerId) {
    return layerId.find("bridge") != std::string::npos;
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
                _ratio = fmin(_ratio + fmax((1. - _ratio) * 0.1, 0.01), 1.);
            }
        } else {
            if (_ratio > 0.) {
                _ratio = fmax(_ratio - fmax((_ratio - 0.) * 0.025, 0.0025), 0.);
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
