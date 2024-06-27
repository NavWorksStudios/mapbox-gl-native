#include <mbgl/style/layers/fill_layer_impl.hpp>

namespace mbgl {
namespace style {

bool FillLayer::Impl::hasLayoutDifference(const Layer::Impl& other) const {
    assert(other.getTypeInfo() == getTypeInfo());
    const auto& impl = static_cast<const style::FillLayer::Impl&>(other);
    return filter != impl.filter || visibility != impl.visibility || layout != impl.layout ||
           paint.get<FillPattern>().value != impl.paint.get<FillPattern>().value ||
           paint.hasDataDrivenPropertyDifference(impl.paint);
}

void FillLayer::Impl::bindPaintColorToPalette() const {
    bindToPalette(paint.get<FillColor>().value);
    bindToPalette(paint.get<FillOutlineColor>().value);
}

} // namespace style
} // namespace mbgl
