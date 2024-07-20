#include <mbgl/style/layer_impl.hpp>

namespace mbgl {
namespace style {

Layer::Impl::Impl(nav::stringid layerID, nav::stringid sourceID)
    : id(std::move(layerID)),
      source(std::move(sourceID)) {
}

void Layer::Impl::populateFontStack(std::set<FontStack>&) const {}

} // namespace style
} // namespace mbgl
