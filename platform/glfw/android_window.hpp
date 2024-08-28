#pragma once

#include <mbgl/util/size.hpp>

namespace mbgl {
namespace gfx {
class RendererBackend;
} // namespace gfx
} // namespace mbgl

class AndroidWindow {
public:
    // 屏幕宽高
    int32_t width;
    int32_t height;
};
