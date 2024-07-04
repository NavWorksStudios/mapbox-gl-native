#include "glfw_renderer_frontend.hpp"

#include <mbgl/renderer/renderer.hpp>
#include <mbgl/gfx/backend_scope.hpp>
#include <mbgl/renderer/update_parameters.hpp>

GLFWRendererFrontend::GLFWRendererFrontend(std::unique_ptr<mbgl::Renderer> renderer_, GLFWView& glfwView_)
    : glfwView(glfwView_)
    , renderer(std::move(renderer_)) {
        glfwView.setRenderFrontend(this);
}

GLFWRendererFrontend::~GLFWRendererFrontend() = default;

void GLFWRendererFrontend::reset() {
    assert(renderer);
    renderer.reset();
}

void GLFWRendererFrontend::setObserver(mbgl::RendererObserver& observer) {
    assert(renderer);
    renderer->setObserver(&observer);
}

void GLFWRendererFrontend::update(std::shared_ptr<mbgl::UpdateParameters> params) {
    updateParameters = std::move(params);
    glfwView.invalidate();
}

void GLFWRendererFrontend::prepare(std::function<void(std::unique_ptr<mbgl::RenderTree>)> notify) {
    assert(renderer);

    if (!updateParameters) return;

    // onStyleImageMissing might be called during a render. The user implemented method
    // could trigger a call to MGLRenderFrontend#update which overwrites `updateParameters`.
    // Copy the shared pointer here so that the parameters aren't destroyed while `render(...)` is
    // still using them.
    auto updateParameters_ = updateParameters;
    renderer->prepare(updateParameters_, notify);

    updateParameters->transformState.notifyProjectionTransform();
}

void GLFWRendererFrontend::render(std::unique_ptr<mbgl::RenderTree> renderTree) {
    assert(renderer);
    mbgl::gfx::BackendScope guard { glfwView.getRendererBackend(), mbgl::gfx::BackendScope::ScopeType::Implicit };
    renderer->render(std::move(renderTree));
}

mbgl::Renderer* GLFWRendererFrontend::getRenderer() {
    assert(renderer);
    return renderer.get();
}
