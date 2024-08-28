#pragma once

#include "android_backend.hpp"

#include <mbgl/gfx/renderable.hpp>
#include <mbgl/gl/renderer_backend.hpp>

struct AndroidWindos;

class AndroidGLBackend final : public AndroidBackend,
                            public mbgl::gl::RendererBackend,
                            public mbgl::gfx::Renderable {
public:
    AndroidGLBackend(AndroidWindos*, bool capFrameRate);
    ~AndroidGLBackend() override;

    void swap();

    // GLFWRendererBackend implementation
public:
    mbgl::gfx::RendererBackend& getRendererBackend() override {
        return *this;
    }
    mbgl::Size getSize() const override;
    void setSize(mbgl::Size) override;

    // mbgl::gfx::RendererBackend implementation
public:
    mbgl::gfx::Renderable& getDefaultRenderable() override {
        return *this;
    }

protected:
    void activate() override;
    void deactivate() override;

    // mbgl::gl::RendererBackend implementation
protected:
    mbgl::gl::ProcAddress getExtensionFunctionPointer(const char*) override;
    void updateAssumedState() override;

private:
    AndroidWindos* window;
};
