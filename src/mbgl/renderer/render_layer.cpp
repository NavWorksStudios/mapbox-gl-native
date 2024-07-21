#include <mbgl/renderer/render_layer.hpp>
#include <mbgl/renderer/paint_parameters.hpp>
#include <mbgl/renderer/render_source.hpp>
#include <mbgl/renderer/render_tile.hpp>
#include <mbgl/style/types.hpp>
#include <mbgl/style/layer.hpp>
#include <mbgl/tile/tile.hpp>
#include <mbgl/gfx/context.hpp>
#include <mbgl/util/logging.hpp>

namespace mbgl {

using namespace style;

RenderLayer::RenderLayer(Immutable<style::LayerProperties> properties)
    : evaluatedProperties(std::move(properties)),
      baseImpl(evaluatedProperties->baseImpl) {
}

void RenderLayer::transition(const TransitionParameters& parameters, Immutable<style::Layer::Impl> newImpl) {
    baseImpl = std::move(newImpl);
    transition(parameters);
}

bool RenderLayer::needsPlacement() const {
    return baseImpl->getTypeInfo()->crossTileIndex == style::LayerTypeInfo::CrossTileIndex::Required
           && !placementData.empty();
}

const nav::stringid& RenderLayer::getID() const {
    return baseImpl->id;
}

bool RenderLayer::hasRenderPass(RenderPass pass) const {
    return passes & pass;
}

bool RenderLayer::needsRendering() const {
    return passes != RenderPass::None
           && baseImpl->visibility != style::VisibilityType::None;
}

bool RenderLayer::supportsZoom(float zoom) const {
    // TODO: shall we use rounding or epsilon comparisons?
    return baseImpl->minZoom <= zoom && baseImpl->maxZoom >= zoom;
}

void RenderLayer::prepare(const LayerPrepareParameters& params) {
    assert(params.source);
    assert(params.source->isEnabled());
    renderTiles = params.source->getRenderTiles();
    if (params.source->renderTilesChanged()) addRenderPassesFromTiles();
}

optional<Color> RenderLayer::getSolidBackground() const {
    return nullopt;
}

void RenderLayer::markContextDestroyed() {
    // no-op
}

void RenderLayer::checkRenderability(const PaintParameters& parameters,
                                     const uint32_t activeBindingCount) {
    // Only warn once for every layer.
    if (hasRenderFailures) {
        return;
    }

    if (activeBindingCount > parameters.context.maximumVertexBindingCount) {
        Log::Error(Event::OpenGL,
                   "The layer '%s' uses more data-driven properties than the current device "
                   "supports, and will have rendering errors. To ensure compatibility with this "
                   "device, use %d fewer data driven properties in this layer.",
                   getID().get().c_str(),
                   activeBindingCount - gfx::Context::minimumRequiredVertexBindingCount);
        hasRenderFailures = true;
    } else if (activeBindingCount > gfx::Context::minimumRequiredVertexBindingCount) {
        Log::Warning(Event::OpenGL,
                     "The layer '%s' uses more data-driven properties than some devices may support. "
                     "Though it will render correctly on this device, it may have rendering errors "
                     "on other devices. To ensure compatibility with all devices, use %d fewer "
                     "data-driven properties in this layer.",
                     getID().get().c_str(),
                     activeBindingCount - gfx::Context::minimumRequiredVertexBindingCount);
        hasRenderFailures = true;
    }
}

void RenderLayer::addRenderPassesFromTiles() {
    assert(renderTiles);
    passes = RenderPass::None;
    renderDatas.clear();
    for (const RenderTile& tile : *renderTiles) {
        const LayerRenderData* renderData = tile.getLayerRenderData(*baseImpl);
        renderDatas.emplace_back(renderData);
        if (renderData) {
            passes |= RenderPass(renderData->layerProperties->renderPasses);
        }
    }
}

const LayerRenderData* RenderLayer::getRenderDataForPass(size_t index, RenderPass pass) const {
    if (const LayerRenderData* renderData = renderDatas[index]) {
        return bool(RenderPass(renderData->layerProperties->renderPasses) & pass) ? renderData : nullptr;
    }
    return nullptr;
}

} //namespace mbgl

