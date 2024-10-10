#pragma once

#include <mbgl/renderer/render_layer.hpp>
#include <mbgl/style/layers/fill_extrusion_layer_impl.hpp>
#include <mbgl/style/layers/fill_extrusion_layer_properties.hpp>
#include <mbgl/gfx/offscreen_texture.hpp>

namespace mbgl {

class RenderFillExtrusionLayer final : public RenderLayer {
public:
    explicit RenderFillExtrusionLayer(Immutable<style::FillExtrusionLayer::Impl>);
    ~RenderFillExtrusionLayer() override;
    
public:
    static void renderDeferredGeoBuffer(PaintParameters& parameters);
    static void renderShadowDepth(PaintParameters& parameters);

private:
    void doRenderDeferredGeoBuffer(PaintParameters&);
    void doRenderShadowDepth(PaintParameters&);

private:
    void transition(const TransitionParameters&) override;
    void evaluate(const PropertyEvaluationParameters&) override;
    bool hasTransition() const override;
    bool hasCrossfade() const override;
    bool is3D() const override;
    void render(PaintParameters&) override;
    bool queryIntersectsFeature(const GeometryCoordinates&,
                                const GeometryTileFeature&,
                                float,
                                const TransformState&,
                                float,
                                const mat4&,
                                const FeatureState&) const override;

    // Paint properties
    style::FillExtrusionPaintProperties::Unevaluated unevaluated;

    std::unique_ptr<gfx::OffscreenTexture> renderTexture;
};

} // namespace mbgl
