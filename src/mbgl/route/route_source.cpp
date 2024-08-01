#include <mbgl/annotation/annotation_manager.hpp>
#include <mbgl/annotation/annotation_source.hpp>
#include <mbgl/style/layer.hpp>
//#include <mbgl/route/annotation_manager.hpp>
#include <mbgl/route/route_source.hpp>

namespace mbgl {

using namespace style;

RouteSource::RouteSource()
    : Source(makeMutable<Impl>()) {
}

RouteSource::Impl::Impl()
    : Source::Impl(SourceType::Annotations, AnnotationManager::SourceID) {
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

bool AnnotationSource::supportsLayerType(const mbgl::style::LayerTypeInfo* info) const {
    return !std::strcmp(info->type, "line") || !std::strcmp(info->type, "symbol") || !std::strcmp(info->type, "fill");
}

Mutable<Source::Impl> RouteSource::createMutable() const noexcept {
    return staticMutableCast<Source::Impl>(makeMutable<Impl>(impl()));
}

} // namespace mbgl
