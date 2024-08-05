#include <mbgl/geometry/feature_index.hpp>
#include <mbgl/gfx/context.hpp>
#include <mbgl/gfx/cull_face_mode.hpp>
#include <mbgl/gfx/renderable.hpp>
#include <mbgl/gfx/renderer_backend.hpp>
#include <mbgl/programs/fill_program.hpp>
#include <mbgl/programs/programs.hpp>
#include <mbgl/renderer/buckets/fill_bucket.hpp>
#include <mbgl/renderer/image_manager.hpp>
#include <mbgl/renderer/layers/render_fill_layer.hpp>
#include <mbgl/renderer/paint_parameters.hpp>
#include <mbgl/renderer/render_source.hpp>
#include <mbgl/renderer/render_tile.hpp>
#include <mbgl/style/expression/image.hpp>
#include <mbgl/style/layers/fill_layer_impl.hpp>
#include <mbgl/tile/geometry_tile.hpp>
#include <mbgl/tile/tile.hpp>
#include <mbgl/util/intersection_tests.hpp>
#include <mbgl/util/math.hpp>

#include "mbgl/nav/nav.style.hpp"
#include "mbgl/nav/nav.palette.hpp"

namespace mbgl {

using namespace style;

namespace {

inline const FillLayer::Impl& impl_cast(const Immutable<style::Layer::Impl>& impl) {
    assert(impl->getTypeInfo() == FillLayer::Impl::staticTypeInfo());
    return static_cast<const FillLayer::Impl&>(*impl);
}

} // namespace

RenderFillLayer::RenderFillLayer(Immutable<style::FillLayer::Impl> _impl)
    : RenderLayer(makeMutable<FillLayerProperties>(std::move(_impl))),
      unevaluated(impl_cast(baseImpl).paint.untransitioned()) {
    bindToPalette(baseImpl->id, "fill-color", unevaluated.get<FillColor>().value);
    bindToPalette(baseImpl->id, "fill-outline-color", unevaluated.get<FillOutlineColor>().value);

    enableShaderPalette = nav::palette::enableShaderPalette(getID());
    enableWaterEffect = (getID() == "water");
}

RenderFillLayer::~RenderFillLayer() = default;

void RenderFillLayer::transition(const TransitionParameters& parameters) {
    unevaluated = impl_cast(baseImpl).paint.transitioned(parameters, std::move(unevaluated));
}

void RenderFillLayer::evaluate(const PropertyEvaluationParameters& parameters) {
    auto&& properties = makeMutable<FillLayerProperties>(
        staticImmutableCast<FillLayer::Impl>(baseImpl),
        parameters.getCrossfadeParameters(),
        unevaluated.evaluate(parameters));
    auto& evaluated = properties->evaluated;

    if (unevaluated.get<style::FillOutlineColor>().isUndefined()) {
        evaluated.get<style::FillOutlineColor>() = evaluated.get<style::FillColor>();
    }

    passes = RenderPass::Translucent;

    if (!(!unevaluated.get<style::FillPattern>().isUndefined()
        || evaluated.get<style::FillColor>().constantOr(Color()).a < 1.0f
        || evaluated.get<style::FillOpacity>().constantOr(0) < 1.0f)) {
        // Supply both - evaluated based on opaquePassCutoff in render().
        passes |= RenderPass::Opaque;
    }
    properties->renderPasses = mbgl::underlying_type(passes);
    evaluatedProperties = std::move(properties);
}

bool RenderFillLayer::hasTransition() const {
    return unevaluated.hasTransition();
}

bool RenderFillLayer::hasCrossfade() const {
    return getCrossfade<FillLayerProperties>(evaluatedProperties).t != 1;
}

void RenderFillLayer::render(PaintParameters& parameters) {
    assert(renderTiles);
    
    bool refreshPaintUniforms = true;
    using Properties = style::FillPaintProperties::DataDrivenProperties;
    mbgl::PaintPropertyBinders<Properties>::UniformValues paintUniformValues;

    if (unevaluated.get<FillPattern>().isUndefined()) {
        parameters.renderTileClippingMasks(renderTiles);
        
        const auto& color = nav::palette::getColorBase();
        FillProgram::LayoutUniformValues layoutUniformValues = {
            uniforms::matrix::Value(),
            uniforms::world::Value( parameters.backend.getDefaultRenderable().getSize() ),
            uniforms::spotlight::Value( nav::runtime::spotlight::value() ),
            uniforms::render_time::Value( nav::runtime::rendertime::value() ),
            uniforms::palette_color::Value( color ),
            uniforms::palette_lightness::Value( enableShaderPalette ? (color.r+color.g+color.b)/3. : 0. ),
            uniforms::water_wave::Value( enableWaterEffect ? util::clamp((parameters.state.getZoom()-13.)*.3,0.,1.) : 0. ),
            uniforms::water_data_z_scale::Value(),
            uniforms::clip_region::Value( nav::display::clip_region() ),
            uniforms::focus_region::Value( nav::display::focus_region() ),
        };
        
        size_t renderIndex = -1;
        for (const RenderTile& tile : *renderTiles) {
            renderIndex++;
            
            if (!tile.isRenderable(Tile::RenderMode::Standard)) {
                continue;
            }
            
            const LayerRenderData* renderData = getRenderDataForPass(renderIndex, parameters.pass);
            if (!renderData) {
                continue;
            }
            
            auto& bucket = static_cast<FillBucket&>(*renderData->bucket);
            const auto& evaluated = getEvaluated<FillLayerProperties>(renderData->layerProperties);
            const auto& paintPropertyBinders = bucket.paintPropertyBinders.at(getID());
            
            if (refreshPaintUniforms) {
                paintPropertyBinders.fillUniformValues(paintUniformValues, parameters.state.getZoom(), evaluated);
                refreshPaintUniforms = false;
            }
            
            const auto& matrix = tile.translatedMatrix(evaluated.get<FillTranslate>(), evaluated.get<FillTranslateAnchor>(), parameters.state);
            layoutUniformValues.template get<uniforms::matrix>() = matrix;
            layoutUniformValues.template get<uniforms::water_data_z_scale>() = pow(2.,16.-tile.id.canonical.z); // data_z [13,16]

            const auto draw = [&] (auto& programInstance,
                                   const auto& drawMode,
                                   const auto& depthMode,
                                   const auto& indexBuffer,
                                   const auto& segments,
                                   auto&& textureBindings) {
                
                const auto&& allAttributeBindings = programInstance.computeAllAttributeBindings(
                    *bucket.vertexBuffer,
                    paintPropertyBinders,
                    evaluated
                );

                checkRenderability(parameters, programInstance.activeBindingCount(allAttributeBindings));

                programInstance.draw(parameters.context,
                                     *parameters.renderPass,
                                     drawMode,
                                     depthMode,
                                     parameters.stencilModeForClipping(renderIndex),
                                     parameters.colorModeForRenderPass(),
                                     gfx::CullFaceMode::disabled(),
                                     indexBuffer,
                                     segments,
                                     layoutUniformValues,
                                     paintUniformValues,
                                     allAttributeBindings,
                                     std::forward<decltype(textureBindings)>(textureBindings),
                                     getID());
            };

            const bool opaque = (evaluated.get<FillColor>().constantOr(Color()).a >= 1.0f &&
                                 evaluated.get<FillOpacity>().constantOr(0) >= 1.0f &&
                                 parameters.currentLayer >= parameters.opaquePassCutoff);

            const auto fillRenderPass = opaque ? RenderPass::Opaque : RenderPass::Translucent;

            if (bucket.triangleIndexBuffer && parameters.pass == fillRenderPass) {
                const auto depthMaskType = parameters.pass == RenderPass::Opaque ? gfx::DepthMaskType::ReadWrite : gfx::DepthMaskType::ReadOnly;
                draw(parameters.programs.getFillLayerPrograms().fill,
                     gfx::Triangles(),
                     parameters.depthModeForSublayer(1, depthMaskType),
                     *bucket.triangleIndexBuffer,
                     bucket.triangleSegments,
                     FillProgram::TextureBindings{});
            }

            if (evaluated.get<FillAntialias>() && parameters.pass == RenderPass::Translucent) {
                draw(parameters.programs.getFillLayerPrograms().fillOutline,
                     gfx::Lines{ 2.0f },
                     parameters.depthModeForSublayer(unevaluated.get<FillOutlineColor>().isUndefined() ? 2 : 0, gfx::DepthMaskType::ReadOnly),
                     *bucket.lineIndexBuffer,
                     bucket.lineSegments,
                     FillOutlineProgram::TextureBindings{});
            }
        }
    } else {
        if (parameters.pass != RenderPass::Translucent) {
            return;
        }

        parameters.renderTileClippingMasks(renderTiles);

        size_t renderIndex = -1;
        for (const RenderTile& tile : *renderTiles) {
            renderIndex++;
            
            if (!tile.isRenderable(Tile::RenderMode::Standard)) {
                continue;
            }
            
            const LayerRenderData* renderData = getRenderDataForPass(renderIndex, parameters.pass);
            if (!renderData) {
                continue;
            }
            auto& bucket = static_cast<FillBucket&>(*renderData->bucket);
            const auto& evaluated = getEvaluated<FillLayerProperties>(renderData->layerProperties);
            const auto& crossfade = getCrossfade<FillLayerProperties>(renderData->layerProperties);

            const auto& fillPatternValue = evaluated.get<FillPattern>().constantOr(Faded<expression::Image>{"", ""});
            optional<ImagePosition> patternPosA = tile.getPattern(fillPatternValue.from.id());
            optional<ImagePosition> patternPosB = tile.getPattern(fillPatternValue.to.id());

            const auto draw = [&] (auto& programInstance,
                                   const auto& drawMode,
                                   const auto& depthMode,
                                   const auto& indexBuffer,
                                   const auto& segments,
                                   auto&& textureBindings) {
                const auto& paintPropertyBinders = bucket.paintPropertyBinders.at(getID());
                paintPropertyBinders.setPatternParameters(patternPosA, patternPosB, crossfade);
                
                if (refreshPaintUniforms) {
                    paintPropertyBinders.fillUniformValues(paintUniformValues, parameters.state.getZoom(), evaluated);
                    refreshPaintUniforms = false;
                }
                
                const auto&& allAttributeBindings = programInstance.computeAllAttributeBindings(
                    *bucket.vertexBuffer,
                    paintPropertyBinders,
                    evaluated
                );

                checkRenderability(parameters, programInstance.activeBindingCount(allAttributeBindings));

                programInstance.draw(parameters.context,
                                     *parameters.renderPass,
                                     drawMode,
                                     depthMode,
                                     parameters.stencilModeForClipping(renderIndex),
                                     parameters.colorModeForRenderPass(),
                                     gfx::CullFaceMode::disabled(),
                                     indexBuffer,
                                     segments,
                                     FillPatternProgram::layoutUniformValues(
                                         tile.translatedMatrix(evaluated.get<FillTranslate>(), evaluated.get<FillTranslateAnchor>(), parameters.state),
                                         parameters.backend.getDefaultRenderable().getSize(),
                                         tile.getIconAtlasTexture().size,
                                         crossfade,
                                         tile.id,
                                         parameters.state,
                                         parameters.pixelRatio),
                                     paintUniformValues,
                                     allAttributeBindings,
                                     std::forward<decltype(textureBindings)>(textureBindings),
                                     getID());
            };

            if (bucket.triangleIndexBuffer) {
                draw(parameters.programs.getFillLayerPrograms().fillPattern,
                     gfx::Triangles(),
                     parameters.depthModeForSublayer(1, gfx::DepthMaskType::ReadWrite),
                     *bucket.triangleIndexBuffer,
                     bucket.triangleSegments,
                     FillPatternProgram::TextureBindings{
                         textures::image::Value{ tile.getIconAtlasTexture().getResource(), gfx::TextureFilterType::Linear },
                     });
            }
            
            if (evaluated.get<FillAntialias>() && unevaluated.get<FillOutlineColor>().isUndefined()) {
                draw(parameters.programs.getFillLayerPrograms().fillOutlinePattern,
                     gfx::Lines { 2.0f },
                     parameters.depthModeForSublayer(2, gfx::DepthMaskType::ReadOnly),
                     *bucket.lineIndexBuffer,
                     bucket.lineSegments,
                     FillOutlinePatternProgram::TextureBindings{
                         textures::image::Value{ tile.getIconAtlasTexture().getResource(), gfx::TextureFilterType::Linear },
                     });
            }
        }
    }
}

bool RenderFillLayer::queryIntersectsFeature(const GeometryCoordinates& queryGeometry,
                                             const GeometryTileFeature& feature, const float,
                                             const TransformState& transformState, const float pixelsToTileUnits,
                                             const mat4&, const FeatureState&) const {
    const auto& evaluated = getEvaluated<FillLayerProperties>(evaluatedProperties);
    auto translatedQueryGeometry = FeatureIndex::translateQueryGeometry(
            queryGeometry,
            evaluated.get<style::FillTranslate>(),
            evaluated.get<style::FillTranslateAnchor>(),
            transformState.getBearing(),
            pixelsToTileUnits);

    return util::polygonIntersectsMultiPolygon(translatedQueryGeometry.value_or(queryGeometry), feature.getGeometries());
}

} // namespace mbgl
