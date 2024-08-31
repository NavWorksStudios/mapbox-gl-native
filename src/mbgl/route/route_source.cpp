#include <mbgl/style/layer.hpp>
#include <mbgl/route/route_line_layer_manager.hpp>
#include <mbgl/route/route_source.hpp>

namespace mbgl {

using namespace style;

RouteSource::RouteSource()
    : Source(makeMutable<Impl>()) {
}

RouteSource::Impl::Impl()
    : Source::Impl(SourceType::Route, RouteLineLayerManager::RouteSourceID) {
}

const RouteSource::Impl& RouteSource::impl() const {
    return static_cast<const Impl&>(*baseImpl);
}

void RouteSource::loadDescription(FileSource&) {
    loaded = true;
}

optional<std::string> RouteSource::Impl::getAttribution() const {
    return {};
}

bool RouteSource::supportsLayerType(const mbgl::style::LayerTypeInfo* info) const {
    return !std::strcmp(info->type, "line") || !std::strcmp(info->type, "symbol") || !std::strcmp(info->type, "fill");
}

Mutable<Source::Impl> RouteSource::createMutable() const noexcept {
    return staticMutableCast<Source::Impl>(makeMutable<Impl>(impl()));
}

} // namespace mbgl
