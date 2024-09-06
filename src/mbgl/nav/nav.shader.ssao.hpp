//
//  nav.shader.ssao.hpp
//
//  Created by BernieZhao on 2024/9/6.
//

#pragma once

// https://zhuanlan.zhihu.com/p/547180466


namespace nav {
namespace shader {
namespace ssao {

const char* phong_vert();
const char* phong_frag();

const char* ssao_vert();
const char* ssao_frag();

const char* blur_vert();
const char* blur_frag();

}
}
}
