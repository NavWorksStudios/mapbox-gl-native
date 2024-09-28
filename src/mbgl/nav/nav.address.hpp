//
//  nav.address.hpp
//
//  Created by BernieZhao on 2024/9/25.
//

#pragma once

#include <mbgl/map/camera.hpp>

namespace nav {

namespace place {
    const mbgl::CameraOptions& current();
    const mbgl::CameraOptions& next();
};

}
