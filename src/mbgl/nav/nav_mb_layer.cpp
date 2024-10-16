//
//  nav_mb_layer_filter.hpp
//
//  Created by BernieZhao on 2023/11/21.
//

#include "mbgl/nav/nav_mb_layer.hpp"

#include <map>
#include <mutex>

namespace nav {

namespace mb {

static const std::map<std::string, std::string> layerIds = {
    
    // format : { id, source-layer, /* type */ }
    
#if 1
//        {"land", "", /* background */ },  地球底色，灰黄色
    
    {ID_NAV_LAND, ID_NAV_LAND, /* fill */ }, // nav 陆地底座

    {"landcover", "landcover", /* fill */ }, // 覆盖物，大洲绿色，低级别才显示
    {"national-park", "landuse_overlay", /* fill */ }, // 国家公园，绿地
    
    {"landuse", "landuse", /* fill */ }, // 用地："park", "airport", "cemetery", "glacier", "hospital", "pitch", "sand", "school",
//        {"pitch-outline", "landuse", /* line */ }, // 球场轮廓

//        {"water-shadow", "water", /* fill */ }, // 水边阴影
//        {"waterway", "waterway", /* line */ }, // 航道
    
    {"water", "water", /* fill */ }, // 所有水系，江河湖海
    
//        {"hillshade", "hillshade", /* fill */ }, // 山阴

//        {"land-structure-polygon", "structure", /* fill */ },
//        {"land-structure-line", "structure", /* line */ },
    
//        {"aeroway-polygon", "aeroway", /* fill */ }, // 航路
//        {"aeroway-line", "aeroway", /* line */ }, // 航路
    
//        {"building-outline", "building", /* line */ }, // 楼底座的沟边
//        {"building", "building", /* fill */ }, // 楼底座
#endif
    
#if 1
    // 负(-)高度路。桥洞或者地下隧道
    {"tunnel-street-minor-low", "road", /* line */ },
//    {"tunnel-street-minor-case", "road", /* line */ }, // - - - - - dasharray
//    {"tunnel-primary-secondary-tertiary-case", "road", /* line */ }, // - - - - - dasharray
//    {"tunnel-major-link-case", "road", /* line */ }, // - - - - - dasharray
//    {"tunnel-motorway-trunk-case", "road", /* line */ }, // - - - - - dasharray
//    {"tunnel-construction", "road", /* line */ }, // - - - - - dasharray
    
    {"tunnel-path", "road", /* line */ },
    {"tunnel-steps", "road", /* line */ },
    {"tunnel-major-link", "road", /* line */ },
    {"tunnel-pedestrian", "road", /* line */ },
    {"tunnel-street-minor", "road", /* line */ },
    {"tunnel-primary-secondary-tertiary", "road", /* line */ },
    {"tunnel-oneway-arrow-blue", "road", /* symbol */ },
    {"tunnel-motorway-trunk", "road", /* line */ },
    {"tunnel-oneway-arrow-white", "road", /* symbol */ },
#endif
    
#if 0   // 不开
    {"ferry", "road", /* line */ }, // 轮渡路线，水上，虚线
    {"ferry-auto", "road", /* line */ }, // 摆渡，水上，实线
#endif
    
#if 1
    // 零(0)高度路。平路
    {"road-path-bg", "road", /* line */ }, // 固定宽度道路
    {"road-steps-bg", "road", /* line */ }, // 道路台阶
    {"turning-feature-outline", "road", /* symbol */ }, // 类似环岛的圆形，灰色，被turning-feature压盖
    {"road-pedestrian-case", "road", /* line */ }, // 人行道-case
    {"road-minor-low", "road", /* line */ }, // 次要道路-low

    {"road-street-low", "road", /* line */ }, // 公路街道-low
    {"road-minor-case", "road", /* line */ }, // 次要道路-case
    {"road-street-case", "road", /* line */ }, // 公路街道-case
    {"road-secondary-tertiary-case", "road", /* line */ }, // 公路二级-三级-case
    {"road-primary-case", "road", /* line */ }, // 主要公路-case

    {"road-major-link-case", "road", /* line */ }, // 公路主干道-case
    {"road-motorway-trunk-case", "road", /* line */ }, // 公路机动车道干线-case
    {"road-construction", "road", /* line */ /* - - - - - - - - - - */ }, // 在建道路
    {"road-path", "road", /* line */ /* - - - - - - - - - - */ }, // 小路（灰色虚线）
    {"road-steps", "road", /* line */ /* - - - - - - - - - - */ }, // 道路台阶（灰色细虚线）

    {"road-major-link", "road", /* line */ }, // 公路主干道
//    {"road-pedestrian", "road", /* line */ /* - - - - - - - - - - */ }, // 人行道（灰色虚线）
    {"road-pedestrian-polygon-fill", "road", /* fill */ }, // 人行道多边形填充（白块）
    {"road-pedestrian-polygon-pattern", "road", /* fill */ }, // 人行道多边形图案
    {"road-polygon", "road", /* fill */ }, // 道路多边形
    

    {"road-minor", "road", /* line */ }, // 次要道路
    {"road-street", "road", /* line */ }, // 公路街道
    {"road-secondary-tertiary", "road", /* line */ }, // 公路二级-三级
    {"road-primary", "road", /* line */ }, // 道路初级
    {"road-oneway-arrow-blue", "road", /* symbol */ }, // 道路单向箭头蓝色

    
    {"road-motorway-trunk", "road", /* line */ }, // 公路机动车道干线
    {"road-rail", "road", /* line */ }, // 公路铁路
    {"road-rail-tracks", "road", /* line */ /* - - - - - - - - - - */ }, // 公路轨道
    {"level-crossing", "road", /* symbol */ }, // 平交道口
    {"road-oneway-arrow-white", "road", /* symbol */ }, // 道路单向箭头白色
    {"turning-feature", "road", /* symbol */ }, // 类似环岛的圆形，白色，压盖turning-feature-outline
#endif

    
//        {"golf-hole-line", "road", /* line */ }, // 高尔夫球洞线
    
    
#if 1
    // 正(+)高度路。桥、高架
    {"bridge-path-bg", "road", /* line */ },
    {"bridge-steps-bg", "road", /* line */ },
    {"bridge-pedestrian-case", "road", /* line */ },
    {"bridge-street-minor-low", "road", /* line */ },
    {"bridge-street-minor-case", "road", /* line */ },
    {"bridge-primary-secondary-tertiary-case", "road", /* line */ },
    {"bridge-major-link-case", "road", /* line */ },
    {"bridge-motorway-trunk-case", "road", /* line */ },
//    {"bridge-construction", "road", /* line */ /* - - - - - - - - - - */ },
    
//    {"bridge-path", "road", /* line */ /* - - - - - - - - - - */ },
//    {"bridge-steps", "road", /* line */ /* - - - - - - - - - - */ },
    {"bridge-major-link", "road", /* line */ },
//    {"bridge-pedestrian", "road", /* line */ /* - - - - - - - - - - */ },
    
    {"bridge-street-minor", "road", /* line */ },
    {"bridge-primary-secondary-tertiary", "road", /* line */ },
    
    {"bridge-oneway-arrow-blue", "road", /* symbol */ },
    
    {"bridge-motorway-trunk", "road", /* line */ },
    
    {"bridge-rail", "road", /* line */ },
//    {"bridge-rail-tracks", "road", /* line */ /* - - - - - - - - - - */ },
    
    {"bridge-major-link-2-case", "road", /* line */ },
    {"bridge-motorway-trunk-2-case", "road", /* line */ },
    
    {"bridge-major-link-2", "road", /* line */ },
    {"bridge-motorway-trunk-2", "road", /* line */ },
    
    {"bridge-oneway-arrow-white", "road", /* symbol */ },
#endif

    
//        {"aerialway", "road", /* line */ },
    

#if 1
//    {"admin-1-boundary-bg", "admin", /* line */ /* - - - - - - - - - - */ }, //省界
    {"admin-0-boundary-bg", "admin", /* line */ }, // 国界
//    {"admin-1-boundary", "admin", /* line */ /* - - - - - - - - - - */ }, //省界
    {"admin-0-boundary", "admin", /* line */ }, // 国界
//    {"admin-0-boundary-disputed", "admin", /* line */ /* - - - - - - - - - - */ }, // 争议国界 
#endif
    
#if 0
    {"building-number-label", "housenum_label", /* symbol */ },
    {"road-label", "road", /* symbol */ },
    {"road-number-shield", "road", /* symbol */ },
    {"road-exit-shield", "motorway_junction", /* symbol */ },
//        {"golf-hole-label", "road", /* symbol */ },
//        {"waterway-label", "natural_label", /* symbol */ },
//        {"natural-line-label", "natural_label", /* symbol */ },
//        {"natural-point-label", "natural_label", /* symbol */ },
//        {"water-line-label", "natural_label", /* symbol */ },
//        {"water-point-label", "natural_label", /* symbol */ },
    {"poi-label", "poi_label", /* symbol */ },
    {"transit-label", "transit_stop_label", /* symbol */ },
    {"airport-label", "airport_label", /* symbol */ },
    {"settlement-subdivision-label", "place_label", /* symbol */ },
    {"settlement-label", "place_label", /* symbol */ },
    {"state-label", "place_label", /* symbol */ },
    {"country-label", "place_label", /* symbol */ },
#endif
    

};

bool filterLayerByType(const std::string& id) {
    return layerIds.find(id) != layerIds.end();
}

static std::map<std::string, int> layerRenderIndexs;
static std::mutex l;

int layerRenderIndex(const std::string& id) {
    if (layerRenderIndexs.empty()) {
        std::lock_guard<std::mutex> lock(l);
        if (layerRenderIndexs.empty()) {
            int i = 0;
            for (auto it = layerIds.begin(); it != layerIds.end(); it++) {
                layerRenderIndexs[id] = i++;
            }
        }
    }
    
    return layerRenderIndexs[id];
}

}

}
