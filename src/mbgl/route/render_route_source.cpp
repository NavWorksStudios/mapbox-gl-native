#include <mbgl/annotation/render_annotation_source.hpp>
#include <mbgl/annotation/annotation_tile.hpp>
#include <mbgl/route/route_tile.hpp>
#include <mbgl/renderer/render_tile.hpp>
#include <mbgl/renderer/paint_parameters.hpp>

#include <mbgl/layermanager/layer_manager.hpp>
#include <mbgl/route/render_route_source.hpp>

namespace mbgl {

using namespace style;

RenderRouteSource::RenderRouteSource(Immutable<RouteSource::Impl> impl_)
    : RenderTileSource(std::move(impl_)) {
    assert(LayerManager::routelineEnabled);
    tilePyramid.setObserver(this);
}

const RouteSource::Impl& RenderRouteSource::impl() const {
    return static_cast<const RouteSource::Impl&>(*baseImpl);
}

void RenderRouteSource::update(Immutable<style::Source::Impl> baseImpl_,
                                    const std::vector<Immutable<style::LayerProperties>>& layers,
                                    const bool needsRendering,
                                    const bool needsRelayout,
                                    const TileParameters& parameters) {
    std::swap(baseImpl, baseImpl_);

    enabled = needsRendering;

    tilePyramid.update(
        layers,
        needsRendering,
        needsRelayout,
        parameters,
        *baseImpl,
        util::tileSize,
        // Zoom level 16 is typically sufficient for annotations.
        // See https://github.com/mapbox/mapbox-gl-native/issues/10197
        {0, 16},
        optional<LatLngBounds>{},
        [&](const OverscaledTileID& tileID) { return std::make_unique<RouteTile>(tileID, parameters); });
}

std::unordered_map<nav::stringid, std::vector<Feature>>
RenderRouteSource::queryRenderedFeatures(const ScreenLineString& geometry,
                                              const TransformState& transformState,
                                              const std::unordered_map<nav::stringid, const RenderLayer*>& layers,
                                              const RenderedQueryOptions& options,
                                              const mat4& projMatrix) const {
    return tilePyramid.queryRenderedFeatures(geometry, transformState, layers, options, projMatrix, {});
}

std::vector<Feature> RenderRouteSource::querySourceFeatures(const SourceQueryOptions&) const {
    return {};
}


} // namespace mbgl
