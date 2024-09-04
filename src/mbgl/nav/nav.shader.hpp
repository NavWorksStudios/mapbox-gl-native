//
//  nav.shader.hpp
//
//  Created by BernieZhao on 2024/9/1.
//

#pragma once

// https://zhuanlan.zhihu.com/p/547180466


namespace nav {

namespace shader {

const char* vs_vert();
const char* geometry_frag();
const char* SSAO_frag();
const char* SSAOBlur_frag();
const char* lighting_frag();

}

}
