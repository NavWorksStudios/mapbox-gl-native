#pragma once

#include "android_view.hpp"
#include <mbgl/renderer/renderer_frontend.hpp>

#include <memory>

namespace mbgl {
class Renderer;
} // namespace mbgl

class AndroidRendererFrontend : public mbgl::RendererFrontend {
public:
    AndroidRendererFrontend(std::unique_ptr<mbgl::Renderer>, AndroidView&);
    ~AndroidRendererFrontend() override;
    
    void reset() override;
    void setObserver(mbgl::RendererObserver&) override;

    void update(std::shared_ptr<mbgl::UpdateParameters>) override;
    void render();
    
    mbgl::Renderer* getRenderer();

private:
    AndroidView& androidView;
    std::unique_ptr<mbgl::Renderer> renderer;
    std::shared_ptr<mbgl::UpdateParameters> updateParameters;
};
