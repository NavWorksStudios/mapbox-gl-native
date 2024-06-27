#include <mbgl/style/layers/raster_layer_impl.hpp>

namespace mbgl {
namespace style {

bool RasterLayer::Impl::hasLayoutDifference(const Layer::Impl&) const {
    return false;
}

void RasterLayer::Impl::bindPaintColorToPalette() const {
    
}

} // namespace style
} // namespace mbgl
