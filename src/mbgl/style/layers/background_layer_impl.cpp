#include <mbgl/style/layers/background_layer_impl.hpp>

namespace mbgl {
namespace style {

bool BackgroundLayer::Impl::hasLayoutDifference(const Layer::Impl&) const {
    return false;
}

void BackgroundLayer::Impl::bindPaintColorToPalette() const {
    bindToPalette(paint.get<BackgroundColor>().value);
}

} // namespace style
} // namespace mbgl
