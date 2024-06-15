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

namespace nav {
namespace mb {

mbgl::Color land_color() { return { 216/255., 225/255., 238/255., 1.0f }; };

void displaceStyle(const std::string& id, std::unique_ptr<::mbgl::style::Layer>& layer) {
//    if (id == "water") {
//        auto fillLayer = static_cast<::mbgl::style::FillLayer*>(layer.get());
//        fillLayer->setFillColor(mbgl::Color(0/255.,255/255.,0/255.,1));
//        fillLayer->setFillBase(-3);
//    } 
//    
//    else 
        
//    if (id == "land") {
//        auto backgroundLayer = static_cast<::mbgl::style::BackgroundLayer*>(layer.get());
//        backgroundLayer->setBackgroundColor(mbgl::Color(255/255.,0/255.,255/255.,1));
//    }
//    
//    else 
//        
//    if (layer->getSourceLayer() == "road") {
//        const std::string type(layer->getTypeInfo()->type);
//        if (type == "line") {
//            auto lineLayer = static_cast<::mbgl::style::LineLayer*>(layer.get());
//            
//            if (id.find("road") == 0) {
//                lineLayer->setLineColor(mbgl::Color(40/255.,47/255.,71/255.,1));
//            }
//            
//            else if (id.find("tunnel") == 0) {
//                lineLayer->setLineColor(mbgl::Color(40/255.,47/255.,255/255.,1));
//                lineLayer->setLineOpacity(0.3);
//            }
//            
//            else if (id.find("bridge") == 0) {
//                lineLayer->setLineColor(mbgl::Color(255/255.,47/255.,71/255.,1));
//                lineLayer->setLineOpacity(0.8);
//            }
//        }
//    }
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
    float _ratio = 0.;

    void update() {
        if (_enabled && _ratio < 1.) {
            _ratio = fmin(_ratio + fmax((1. - _ratio) * 0.2, 0.01), 1.);
        } else if (_ratio > 0.) {
            _ratio = fmax(_ratio - fmax((_ratio - 0.) * 0.05, 0.01), 0.);
        }
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
