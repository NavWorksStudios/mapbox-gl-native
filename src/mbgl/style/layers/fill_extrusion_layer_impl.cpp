#include <mbgl/style/layers/fill_extrusion_layer_impl.hpp>

namespace mbgl {
namespace style {

bool FillExtrusionLayer::Impl::hasLayoutDifference(const Layer::Impl& other) const {
    assert(other.getTypeInfo() == getTypeInfo());
    const auto& impl = static_cast<const style::FillExtrusionLayer::Impl&>(other);
    return filter     != impl.filter ||
           visibility != impl.visibility ||
           paint.hasDataDrivenPropertyDifference(impl.paint);
}

void FillExtrusionLayer::Impl::bindPaintColorToPalette() const {
    bindToPalette(paint.get<FillExtrusionColor>().value);
}

} // namespace style
} // namespace mbgl
