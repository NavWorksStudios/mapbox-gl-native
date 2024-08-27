#include "android_renderer_frontend.hpp"

#include <mbgl/renderer/renderer.hpp>
#include <mbgl/gfx/backend_scope.hpp>
#include <mbgl/renderer/update_parameters.hpp>

#include "mbgl/nav/nav.style.hpp"

AndroidRendererFrontend::AndroidRendererFrontend(std::unique_ptr<mbgl::Renderer> renderer_, AndroidView& androidView_)
    : androidView(androidView_)
    , renderer(std::move(renderer_)) {
        androidView.setRenderFrontend(this);
}

AndroidRendererFrontend::~AndroidRendererFrontend() = default;

void AndroidRendererFrontend::reset() {
    assert(renderer);
    renderer.reset();
}

void AndroidRendererFrontend::setObserver(mbgl::RendererObserver& observer) {
    assert(renderer);
    renderer->setObserver(&observer);
}

void AndroidRendererFrontend::update(std::shared_ptr<mbgl::UpdateParameters> params) {
    updateParameters = std::move(params);
    androidView.invalidate();
}

void AndroidRendererFrontend::render() {
    assert(renderer);
    
    if (!updateParameters) return;
    
    mbgl::gfx::BackendScope guard { androidView.getRendererBackend(), mbgl::gfx::BackendScope::ScopeType::Implicit };
    
    nav::display::update(updateParameters->transformState.getZoom());

    // onStyleImageMissing might be called during a render. The user implemented method
    // could trigger a call to MGLRenderFrontend#update which overwrites `updateParameters`.
    // Copy the shared pointer here so that the parameters aren't destroyed while `render(...)` is
    // still using them.
    auto updateParameters_ = updateParameters;
    renderer->render(updateParameters_);
}

mbgl::Renderer* AndroidRendererFrontend::getRenderer() {
    assert(renderer);
    return renderer.get();
}
