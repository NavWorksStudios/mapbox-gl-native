#pragma once

#include <mbgl/annotation/annotation_source.hpp>
#include <mbgl/route/route_source.hpp>
#include <mbgl/renderer/sources/render_tile_source.hpp>

namespace mbgl {

class RenderRouteSource final : public RenderTileSource {
public:
    explicit RenderRouteSource(Immutable<RouteSource::Impl>);

    void update(Immutable<style::Source::Impl>,
                const std::vector<Immutable<style::LayerProperties>>&,
                bool needsRendering,
                bool needsRelayout,
                const TileParameters&) final;

    std::unordered_map<nav::stringid, std::vector<Feature>>
    queryRenderedFeatures(const ScreenLineString& geometry,
                          const TransformState& transformState,
                          const std::unordered_map<nav::stringid, const RenderLayer*>& layers,
                          const RenderedQueryOptions& options,
                          const mat4& projMatrix) const final;

    std::vector<Feature>
    querySourceFeatures(const SourceQueryOptions&) const final;

private:
    const RouteSource::Impl& impl() const;
};

} // namespace mbgl
