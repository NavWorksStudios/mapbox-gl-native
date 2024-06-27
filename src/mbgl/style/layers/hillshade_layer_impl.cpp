#include <mbgl/style/layers/hillshade_layer_impl.hpp>

namespace mbgl {
namespace style {

bool HillshadeLayer::Impl::hasLayoutDifference(const Layer::Impl&) const {
    return false;
}

void HillshadeLayer::Impl::bindPaintColorToPalette() const {
    bindToPalette(paint.get<HillshadeAccentColor>().value);
    bindToPalette(paint.get<HillshadeHighlightColor>().value);
    bindToPalette(paint.get<HillshadeShadowColor>().value);
}

} // namespace style
} // namespace mbgl
