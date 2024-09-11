//
//  nav.ssao.hpp
//
//  Created by BernieZhao on 2024/9/1.
//

#pragma once

// https://zhuanlan.zhihu.com/p/547180466

#include <functional>
#include <gl_headers.hpp>

namespace nav {
namespace ssao {
namespace v2 {
    void draw(std::function<void()> renderScene);
}
}
}
