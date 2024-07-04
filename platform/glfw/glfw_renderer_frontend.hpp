#pragma once

#include "glfw_view.hpp"
#include <mbgl/renderer/renderer_frontend.hpp>
#include <mbgl/renderer/render_tree.hpp>

#include <memory>

namespace mbgl {
class Renderer;
} // namespace mbgl

class GLFWRendererFrontend : public mbgl::RendererFrontend {
public:
    GLFWRendererFrontend(std::unique_ptr<mbgl::Renderer>, GLFWView&);
    ~GLFWRendererFrontend() override;
    
    void reset() override;
    void setObserver(mbgl::RendererObserver&) override;

    void update(std::shared_ptr<mbgl::UpdateParameters>) override;
    void prepare(std::function<void()> onUpdate, std::function<void(std::unique_ptr<mbgl::RenderTree>)> onFinish);
    void render(std::unique_ptr<mbgl::RenderTree> renderTree);
    
    mbgl::Renderer* getRenderer();

private:
    GLFWView& glfwView;
    std::unique_ptr<mbgl::Renderer> renderer;
    std::shared_ptr<mbgl::UpdateParameters> updateParameters;
};
