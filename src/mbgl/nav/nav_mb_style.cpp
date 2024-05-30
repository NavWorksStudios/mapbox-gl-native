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

namespace nav {
namespace mb {

mbgl::Color land_color() { return { 216/255., 225/255., 238/255., 1.0f }; };

void displaceStyle(const std::string& id, std::unique_ptr<::mbgl::style::Layer>& layer) {
    if (id == "water") {
        auto fillLayer = static_cast<::mbgl::style::FillLayer*>(layer.get());
        
        fillLayer->setFillColor(mbgl::Color(83/255.,120/255.,148/255.,1));
        fillLayer->setFillBase(-3);
    }
    
    else if (id == ID_NAV_LAND) {
        auto fillLayer = static_cast<::mbgl::style::FillLayer*>(layer.get());
        
        fillLayer->setFillColor(land_color());
    }
    
    else if (layer->getSourceLayer() == "road") {
        const std::string type(layer->getTypeInfo()->type);
        if (type == "line") {
            auto lineLayer = static_cast<::mbgl::style::LineLayer*>(layer.get());
            
            if (id.find("road") == 0) {
                lineLayer->setLineColor(mbgl::Color(40/255.,47/255.,71/255.,1));
            }
            
            else if (id.find("tunnel") == 0) {
                lineLayer->setLineColor(mbgl::Color(40/255.,47/255.,255/255.,1));
                lineLayer->setLineOpacity(0.3);
            }
            
            else if (id.find("bridge") == 0) {
                lineLayer->setLineColor(mbgl::Color(255/255.,47/255.,71/255.,1));
                lineLayer->setLineOpacity(0.8);
            }
        }
    }
}

}
}
