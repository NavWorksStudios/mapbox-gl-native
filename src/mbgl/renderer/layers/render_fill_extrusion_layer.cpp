#include <mbgl/geometry/feature_index.hpp>
#include <mbgl/gfx/cull_face_mode.hpp>
#include <mbgl/gfx/render_pass.hpp>
#include <mbgl/gfx/renderer_backend.hpp>
#include <mbgl/programs/fill_extrusion_program.hpp>
#include <mbgl/programs/fill_extrusion_ssao_program.hpp>
#include <mbgl/programs/programs.hpp>
#include <mbgl/renderer/buckets/fill_extrusion_bucket.hpp>
#include <mbgl/renderer/image_manager.hpp>
#include <mbgl/renderer/layers/render_fill_extrusion_layer.hpp>
#include <mbgl/renderer/paint_parameters.hpp>
#include <mbgl/renderer/render_static_data.hpp>
#include <mbgl/renderer/render_tile.hpp>
#include <mbgl/style/expression/image.hpp>
#include <mbgl/style/layers/fill_extrusion_layer_impl.hpp>
#include <mbgl/tile/geometry_tile.hpp>
#include <mbgl/tile/tile.hpp>
#include <mbgl/util/intersection_tests.hpp>
#include <mbgl/util/math.hpp>

#include "mbgl/nav/nav.theme.hpp"
#include "mbgl/nav/ssao/v1/nav.ssao.hpp"
#include "mbgl/nav/ssao/v2/nav.ssao.hpp"
#include "mbgl/nav/ssao/v1/vec3.h"
#include "mbgl/nav/ssao/v1/mat4.h"

namespace mbgl {

using namespace style;

namespace {

inline const FillExtrusionLayer::Impl& impl_cast(const Immutable<style::Layer::Impl>& impl) {
    assert(impl->getTypeInfo() == FillExtrusionLayer::Impl::staticTypeInfo());
    return static_cast<const FillExtrusionLayer::Impl&>(*impl);
}

} // namespace

static RenderFillExtrusionLayer* renderFillExtrusionLayer = nullptr;

RenderFillExtrusionLayer::RenderFillExtrusionLayer(Immutable<style::FillExtrusionLayer::Impl> _impl)
    : RenderLayer(makeMutable<FillExtrusionLayerProperties>(std::move(_impl))),
      unevaluated(impl_cast(baseImpl).paint.untransitioned()) {
    renderFillExtrusionLayer = this;
    bindToPalette(baseImpl->id, "fill-extrusion-color", unevaluated.get<FillExtrusionColor>().value);
}

RenderFillExtrusionLayer::~RenderFillExtrusionLayer() {
    renderFillExtrusionLayer = nullptr;
}

void RenderFillExtrusionLayer::renderSSAO(PaintParameters& parameters) {
    // render extrusion with SSAO shader
    if(renderFillExtrusionLayer)
        renderFillExtrusionLayer->doRenderSSAO(parameters);
}

void RenderFillExtrusionLayer::doRenderSSAO(PaintParameters& parameters) {
    if(!renderTiles)
        return;

    const auto& evaluated = static_cast<const FillExtrusionLayerProperties&>(*evaluatedProperties).evaluated;
    const auto& crossfade = static_cast<const FillExtrusionLayerProperties&>(*evaluatedProperties).crossfade;
    if (evaluatedProperties->renderPasses == mbgl::underlying_type(RenderPass::None)) {
        return;
    }

    const auto depthMode = parameters.depthModeFor3D();
    
    bool refreshPaintUniforms = true;
    using Properties = style::FillExtrusionPaintProperties::DataDrivenProperties;
    mbgl::PaintPropertyBinders<Properties>::UniformValues paintUniformValues;

    const auto draw = [&](auto& programInstance,
                          const auto& evaluated_,
                          const auto& crossfade_,
                          const gfx::StencilMode& stencilMode,
                          const gfx::ColorMode& colorMode,
                          const auto& tileBucket,
                          auto& layoutUniformValues,
                          const optional<ImagePosition>& patternPositionA,
                          const optional<ImagePosition>& patternPositionB,
                          const auto& textureBindings,
                          const std::string& uniqueName) {
        const auto& paintPropertyBinders = tileBucket.paintPropertyBinders.at(getID());
        paintPropertyBinders.setPatternParameters(patternPositionA, patternPositionB, crossfade_);

        if (refreshPaintUniforms) {
            paintPropertyBinders.fillUniformValues(paintUniformValues, parameters.state.getZoom(), evaluated_);
            refreshPaintUniforms = false;
        }

        const auto&& allAttributeBindings = programInstance.computeAllAttributeBindings(
            *tileBucket.vertexBuffer,
            paintPropertyBinders,
            evaluated_
        );
        
        checkRenderability(parameters, programInstance.activeBindingCount(allAttributeBindings));
        
        // draw self
        programInstance.draw(
            parameters.context,
            *parameters.renderPass,
            gfx::Triangles(),
            depthMode,
            stencilMode,
            colorMode,
            gfx::CullFaceMode::backCCW(),
            *tileBucket.indexBuffer,
            tileBucket.triangleSegments,
            layoutUniformValues,
            paintUniformValues,
            allAttributeBindings,
            textureBindings,
            uniqueName);

    };

    // Draw solid color extrusions
    const auto drawTiles = [&](const gfx::StencilMode& stencilMode_, const gfx::ColorMode& colorMode_, const std::string& name) {
        auto layoutUniforms = FillExtrusionSSAOProgram::layoutUniformValues(
            uniforms::matrix::Value(),
            uniforms::model_view_matrix::Value(),
            uniforms::normal_matrix::Value()
        );
        
        const std::string uniqueName = getID().get() + "/" + name;

        size_t renderIndex = -1;
        for (const RenderTile& tile : *renderTiles) {
            renderIndex++;
            
            if (!tile.isRenderable(Tile::RenderMode::Detailed)) {
                continue;
            }
            
            const LayerRenderData* renderData = getRenderDataForPass(renderIndex, parameters.pass);
            if (!renderData) {
                continue;
            }

            auto& bucket = static_cast<FillExtrusionBucket&>(*renderData->bucket);
            
            const auto& translate = evaluated.get<FillExtrusionTranslate>();
            const auto& anchor = evaluated.get<FillExtrusionTranslateAnchor>();
            const auto& state = parameters.state;
            
            const auto matrix = tile.translatedClipMatrix(translate, anchor, state);
            layoutUniforms.template get<uniforms::matrix>() = matrix;

            layoutUniforms.template get<uniforms::model_view_matrix>() = tile.modelViewMatrix;
            
            auto& normalMatrix = layoutUniforms.template get<uniforms::normal_matrix>();
            matrix::invert(normalMatrix, tile.modelViewMatrix);
            matrix::transpose(normalMatrix);
            
            draw(parameters.programs.getFillExtrusionSSAOLayerPrograms().fillExtrusion,
                 evaluated,
                 crossfade,
                 stencilMode_,
                 colorMode_,
                 bucket,
                 layoutUniforms,
                 {},
                 {},
                 FillExtrusionSSAOProgram::TextureBindings{},
                 uniqueName
            );
        }
    };
    
    const auto drawTileFloors = [&]() {
        auto layoutUniforms = FillExtrusionSSAOProgram::layoutUniformValues(
            uniforms::matrix::Value(),
            uniforms::model_view_matrix::Value(),
            uniforms::normal_matrix::Value()
        );
        size_t renderIndex = -1;
        for (const RenderTile& tile : *renderTiles) {
            renderIndex++;
            
            if (!tile.isRenderable(Tile::RenderMode::Detailed)) {
                continue;
            }
            
            const auto& translate = evaluated.get<FillExtrusionTranslate>();
            const auto& anchor = evaluated.get<FillExtrusionTranslateAnchor>();
            const auto& state = parameters.state;
            
            const auto matrix = tile.translatedClipMatrix(translate, anchor, state);
            layoutUniforms.template get<uniforms::matrix>() = matrix;

            layoutUniforms.template get<uniforms::model_view_matrix>() = tile.modelViewMatrix;
            
            auto& normalMatrix = layoutUniforms.template get<uniforms::normal_matrix>();
            matrix::invert(normalMatrix, tile.modelViewMatrix);
            matrix::transpose(normalMatrix);
            
            // draw tile floors with ssao logic code
            nav::ssao::v2::renderTileFloor(matrix, tile.modelViewMatrix, normalMatrix);
        }
    };

    
    const auto drawTileShadows = [&](const gfx::StencilMode& stencilMode_, const gfx::ColorMode& colorMode_, const std::string& name) {
        auto layoutUniforms = FillExtrusionSSAOProgram::layoutUniformValues(
            uniforms::matrix::Value(),
            uniforms::model_view_matrix::Value(),
            uniforms::normal_matrix::Value()
        );
        
        const std::string uniqueName = getID().get() + "/" + name;

        size_t renderIndex = -1;
        for (const RenderTile& tile : *renderTiles) {
            renderIndex++;
            
            if (!tile.isRenderable(Tile::RenderMode::Detailed)) {
                continue;
            }
            
            const LayerRenderData* renderData = getRenderDataForPass(renderIndex, parameters.pass);
            if (!renderData) {
                continue;
            }

            auto& bucket = static_cast<FillExtrusionBucket&>(*renderData->bucket);
            
            const auto& translate = evaluated.get<FillExtrusionTranslate>();
            const auto& anchor = evaluated.get<FillExtrusionTranslateAnchor>();
            const auto& state = parameters.state;
            
            // #*# 使用相机矩阵进行渲染
            const auto matrix = tile.translatedClipMatrix(translate, anchor, state);
            layoutUniforms.template get<uniforms::matrix>() = matrix;
            layoutUniforms.template get<uniforms::model_view_matrix>() = tile.modelViewMatrix;
            auto& normalMatrix = layoutUniforms.template get<uniforms::normal_matrix>();
            matrix::invert(normalMatrix, tile.modelViewMatrix);
            matrix::transpose(normalMatrix);
            
#if 1
            // #*# 使用光源矩阵进行渲染
            auto convertVec3 = [] (std::array<float, 3> vec3) {
                Vec3 v(vec3.at(0), vec3.at(1), vec3.at(2));
                return v;
            };

            auto convertMatrixTo = [] (Mat4& m) {
                mbgl::mat4 matrix;
                for (int i = 0; i < 16; i++) matrix[i] = ((float*)&m)[i];
                return matrix;
            };
            
            auto latLonToWorldPositon = [] (const mbgl::Point<double>& point, mbgl::Point<double>& point_local, const float z) {
                const double size_local = mbgl::util::EXTENT * std::pow(2, z);
                const double size = std::pow(2, z);
                
                auto x_local = (point.x + mbgl::util::LONGITUDE_MAX) * size_local / mbgl::util::DEGREES_MAX;
                auto y_local =
                    (mbgl::util::LONGITUDE_MAX - (std::log(std::tan(point.y * M_PI / mbgl::util::DEGREES_MAX + M_PI / 4.0)) * mbgl::util::RAD2DEG)) *
                    size_local / mbgl::util::DEGREES_MAX;
                
                point_local.x = x_local;
                point_local.y = y_local;
            };

            const double worldsize = Projection::worldSize(state.zoomScale(state.getZoom()));
            mbgl::Point<double> point_local;
            mbgl::Point<double> point = {state.getLatLng().longitude(), state.getLatLng().latitude()};
            latLonToWorldPositon(point, point_local, state.getZoom());
            
            // #*# lighting pos
            auto lightPos = parameters.evaluatedLight.get<LightPosition>().getCartesian();
            lightPos.at(0) *= worldsize;
            lightPos.at(1) *= worldsize;
            lightPos.at(2) *= worldsize;
//            lightPos.at(2) = 0.000015;
//            lightPos = {0., 1.5, 1.5};
            Vec3 lookat = Vec3(point_local.x, point_local.y, 0);
//            Vec3 lookat = Vec3(0, 0, 0);
            const static double pi = acos(0.0) * 2;
            Mat4 ident = Mat4::identityMatrix();
            Mat4 view, viewNorm;
            Mat4::lookAtMatrix(convertVec3(lightPos), lookat, Vec3(0, 1, 0), view, viewNorm);
            Mat4 projMat = Mat4::perspectiveMatrix(pi * 0.5, 1.333333f, 0.1f, 1000.0f);
            Mat4 invProjMat = Mat4::perspectiveInvMatrix(pi * 0.5, 1.3333333f, 0.1f, 1000.0f);
            Mat4 vp = projMat * view;

            mbgl::mat4 u_mvp, u_mv, u_mv_normal;
            mbgl::mat4 u_view = convertMatrixTo(view);
            mbgl::mat4 u_p = convertMatrixTo(projMat);

            matrix::multiply(u_mv, tile.modelMatrix, u_view);
            matrix::invert(u_mv_normal, u_mv);
            matrix::transpose(u_mv_normal);
            matrix::multiply(u_mvp, u_mv, u_p);

            layoutUniforms.template get<uniforms::matrix>() = u_mvp;
            layoutUniforms.template get<uniforms::model_view_matrix>() = u_mv;
            layoutUniforms.template get<uniforms::normal_matrix>() = u_mv_normal;
#endif
            
            draw(parameters.programs.getFillExtrusionSSAOLayerPrograms().fillExtrusion,
                 evaluated,
                 crossfade,
                 stencilMode_,
                 colorMode_,
                 bucket,
                 layoutUniforms,
                 {},
                 {},
                 FillExtrusionSSAOProgram::TextureBindings{},
                 uniqueName
            );
        }
    };

    drawTileFloors();

    drawTiles(gfx::StencilMode::disabled(), parameters.colorModeForRenderPass(), "color");
    
    // 绘制阴影画布前需要先设置相应gl环境
//    nav::ssao::v2::enableShadowEnv();
//    drawTileShadows(gfx::StencilMode::disabled(), parameters.colorModeForRenderPass(), "color");
}

void RenderFillExtrusionLayer::renderShadowDepth(PaintParameters& parameters) {
    // render extrusion with shadow depth shader
    if(renderFillExtrusionLayer)
        renderFillExtrusionLayer->doRenderShadowDepth(parameters);
}

void RenderFillExtrusionLayer::doRenderShadowDepth(PaintParameters&) {
    
    
}

void RenderFillExtrusionLayer::transition(const TransitionParameters& parameters) {
    unevaluated = impl_cast(baseImpl).paint.transitioned(parameters, std::move(unevaluated));
}

void RenderFillExtrusionLayer::evaluate(const PropertyEvaluationParameters& parameters) {
    auto&& properties = makeMutable<FillExtrusionLayerProperties>(
        staticImmutableCast<FillExtrusionLayer::Impl>(baseImpl),
        parameters.getCrossfadeParameters(),
        unevaluated.evaluate(parameters));

    passes = (properties->evaluated.get<style::FillExtrusionOpacity>() > 0)
                 ? (RenderPass::Translucent | RenderPass::Pass3D)
                 : RenderPass::None;
    properties->renderPasses = mbgl::underlying_type(passes);
    evaluatedProperties = std::move(properties);
}

bool RenderFillExtrusionLayer::hasTransition() const {
    return unevaluated.hasTransition();
}

bool RenderFillExtrusionLayer::hasCrossfade() const {
    return getCrossfade<FillExtrusionLayerProperties>(evaluatedProperties).t != 1;
}

bool RenderFillExtrusionLayer::is3D() const {
    return true;
}

void RenderFillExtrusionLayer::render(PaintParameters& parameters) {
    assert(renderTiles);
    if (parameters.pass != RenderPass::Translucent) {
        return;
    }

    const auto& evaluated = static_cast<const FillExtrusionLayerProperties&>(*evaluatedProperties).evaluated;
    const auto& crossfade = static_cast<const FillExtrusionLayerProperties&>(*evaluatedProperties).crossfade;
    if (evaluatedProperties->renderPasses == mbgl::underlying_type(RenderPass::None)) {
        return;
    }

    const auto depthMode = parameters.depthModeFor3D();
    
    bool refreshPaintUniforms = true;
    using Properties = style::FillExtrusionPaintProperties::DataDrivenProperties;
    mbgl::PaintPropertyBinders<Properties>::UniformValues paintUniformValues;

    const auto draw = [&](auto& programInstance,
                          const auto& evaluated_,
                          const auto& crossfade_,
                          const gfx::StencilMode& stencilMode,
                          const gfx::ColorMode& colorMode,
                          const auto& tileBucket,
                          auto& layoutUniformValues,
                          const optional<ImagePosition>& patternPositionA,
                          const optional<ImagePosition>& patternPositionB,
                          const auto& textureBindings,
                          const std::string& uniqueName) {
        const auto& paintPropertyBinders = tileBucket.paintPropertyBinders.at(getID());
        paintPropertyBinders.setPatternParameters(patternPositionA, patternPositionB, crossfade_);

        if (refreshPaintUniforms) {
            paintPropertyBinders.fillUniformValues(paintUniformValues, parameters.state.getZoom(), evaluated_);
            refreshPaintUniforms = false;
        }

        const auto&& allAttributeBindings = programInstance.computeAllAttributeBindings(
            *tileBucket.vertexBuffer,
            paintPropertyBinders,
            evaluated_
        );
        
        checkRenderability(parameters, programInstance.activeBindingCount(allAttributeBindings));
        
        // draw reflection
        if (nav::theme::enableBuildingReflection()) {
            layoutUniformValues.template get<uniforms::is_reflection>() = true;
            programInstance.draw(
                parameters.context,
                *parameters.renderPass,
                gfx::Triangles(),
                depthMode,
                stencilMode,
                colorMode,
                gfx::CullFaceMode::backCCW(),
                *tileBucket.reflectionIndexBuffer,
                tileBucket.triangleSegments,
                layoutUniformValues,
                paintUniformValues,
                allAttributeBindings,
                textureBindings,
                uniqueName);
        }
        
        // draw self
        layoutUniformValues.template get<uniforms::is_reflection>() = false;
        programInstance.draw(
            parameters.context,
            *parameters.renderPass,
            gfx::Triangles(),
            depthMode,
            stencilMode,
            colorMode,
            gfx::CullFaceMode::backCCW(),
            *tileBucket.indexBuffer,
            tileBucket.triangleSegments,
            layoutUniformValues,
            paintUniformValues,
            allAttributeBindings,
            textureBindings,
            uniqueName);

    };

    if (unevaluated.get<FillExtrusionPattern>().isUndefined()) {
        // Draw solid color extrusions
        const auto drawTiles = [&](const gfx::StencilMode& stencilMode_, const gfx::ColorMode& colorMode_, const std::string& name) {
            auto layoutUniforms = FillExtrusionProgram::layoutUniformValues(
                uniforms::matrix::Value(),
                uniforms::model_matrix::Value(),
                parameters.state,
                evaluated.get<FillExtrusionOpacity>(),
                parameters.evaluatedLight,
                evaluated.get<FillExtrusionVerticalGradient>(),
                uniforms::is_reflection::Value()
            );
            
            const std::string uniqueName = getID().get() + "/" + name;

            size_t renderIndex = -1;
            for (const RenderTile& tile : *renderTiles) {
                renderIndex++;
                
                if (!tile.isRenderable(Tile::RenderMode::Detailed)) {
                    continue;
                }
                
                const LayerRenderData* renderData = getRenderDataForPass(renderIndex, parameters.pass);
                if (!renderData) {
                    continue;
                }

                auto& bucket = static_cast<FillExtrusionBucket&>(*renderData->bucket);
                
                const auto& translate = evaluated.get<FillExtrusionTranslate>();
                const auto& anchor = evaluated.get<FillExtrusionTranslateAnchor>();
                const auto& state = parameters.state;
                layoutUniforms.template get<uniforms::matrix>() = tile.translatedClipMatrix(translate, anchor, state);
                layoutUniforms.template get<uniforms::model_matrix>() = tile.modelMatrix;
                
                draw(parameters.programs.getFillExtrusionLayerPrograms().fillExtrusion,
                     evaluated,
                     crossfade,
                     stencilMode_,
                     colorMode_,
                     bucket,
                     layoutUniforms,
                     {},
                     {},
                     FillExtrusionProgram::TextureBindings{},
                     uniqueName
                );
            }
        };

        if (evaluated.get<FillExtrusionOpacity>() == 1) {
            // Draw opaque extrusions
            drawTiles(gfx::StencilMode::disabled(), parameters.colorModeForRenderPass(), "color");
        } else {
            // Draw transparent buildings in two passes so that only the closest surface is drawn.
            // First draw all the extrusions into only the depth buffer. No colors are drawn.
            drawTiles(gfx::StencilMode::disabled(), gfx::ColorMode::disabled(), "depth");

            // Then draw all the extrusions a second time, only coloring fragments if they have the
            // same depth value as the closest fragment in the previous pass. Use the stencil buffer
            // to prevent the second draw in cases where we have coincident polygons.
            drawTiles(parameters.stencilModeFor3D(), parameters.colorModeForRenderPass(), "color");
        }
    } else {
        // Draw textured extrusions
        const auto& fillPatternValue =
            evaluated.get<FillExtrusionPattern>().constantOr(mbgl::Faded<expression::Image>{"", ""});
        
        const auto drawTiles = [&](const gfx::StencilMode& stencilMode_, const gfx::ColorMode& colorMode_, const std::string& name) {
            size_t renderIndex = -1;
            for (const RenderTile& tile : *renderTiles) {
                renderIndex++;
                
                if (!tile.isRenderable(Tile::RenderMode::Detailed)) {
                    continue;
                }
                
                const LayerRenderData* renderData = getRenderDataForPass(renderIndex, parameters.pass);
                if (!renderData) {
                    continue;
                }
                
                auto& bucket = static_cast<FillExtrusionBucket&>(*renderData->bucket);
                optional<ImagePosition> patternPosA = tile.getPattern(fillPatternValue.from.id());
                optional<ImagePosition> patternPosB = tile.getPattern(fillPatternValue.to.id());
                
                const auto& translate = evaluated.get<FillExtrusionTranslate>();
                const auto& anchor = evaluated.get<FillExtrusionTranslateAnchor>();
                const auto& state = parameters.state;

                auto&& layoutUniforms = FillExtrusionPatternProgram::layoutUniformValues(
                    tile.translatedClipMatrix(translate, anchor, state),
                    tile.getIconAtlasTexture().size,
                    crossfade,
                    tile.id,
                    parameters.state,
                    evaluated.get<FillExtrusionOpacity>(),
                    -std::pow(2, tile.id.canonical.z) / util::tileSize / 8.0f,
                    parameters.pixelRatio,
                    parameters.evaluatedLight,
                    evaluated.get<FillExtrusionVerticalGradient>(),
                    false
                );
                
                draw(parameters.programs.getFillExtrusionLayerPrograms().fillExtrusionPattern,
                     evaluated,
                     crossfade,
                     stencilMode_,
                     colorMode_,
                     bucket,
                     layoutUniforms,
                     patternPosA,
                     patternPosB,
                     FillExtrusionPatternProgram::TextureBindings{
                         textures::image::Value{ tile.getIconAtlasTexture().getResource(), gfx::TextureFilterType::Linear },
                     },
                     name
                );
            }
        };

        // Draw transparent buildings in two passes so that only the closest surface is drawn.
        // First draw all the extrusions into only the depth buffer. No colors are drawn.
        drawTiles(gfx::StencilMode::disabled(), gfx::ColorMode::disabled(), "depth");

        // Then draw all the extrusions a second time, only coloring fragments if they have the
        // same depth value as the closest fragment in the previous pass. Use the stencil buffer
        // to prevent the second draw in cases where we have coincident polygons.
        drawTiles(parameters.stencilModeFor3D(), parameters.colorModeForRenderPass(), "color");
    }
}

bool RenderFillExtrusionLayer::queryIntersectsFeature(const GeometryCoordinates& queryGeometry,
                                                      const GeometryTileFeature& feature, const float,
                                                      const TransformState& transformState,
                                                      const float pixelsToTileUnits, const mat4&,
                                                      const FeatureState&) const {
    const auto& evaluated = static_cast<const FillExtrusionLayerProperties&>(*evaluatedProperties).evaluated;
    const auto translatedQueryGeometry = FeatureIndex::translateQueryGeometry(
            queryGeometry,
            evaluated.get<style::FillExtrusionTranslate>(),
            evaluated.get<style::FillExtrusionTranslateAnchor>(),
            transformState.getBearing(),
            pixelsToTileUnits);

    return util::polygonIntersectsMultiPolygon(translatedQueryGeometry.value_or(queryGeometry), feature.getGeometries());
}

} // namespace mbgl
