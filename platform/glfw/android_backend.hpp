#pragma once

#include <mbgl/util/size.hpp>
#include <mbgl/gfx/backend.hpp>

namespace mbgl {
namespace gfx {
class RendererBackend;
} // namespace gfx
} // namespace mbgl

class AndroidWindow;
class AndroidBackend {
public:
    explicit AndroidBackend() = default;
    AndroidBackend(const AndroidBackend&) = delete;
    AndroidBackend& operator=(const AndroidBackend&) = delete;
    virtual ~AndroidBackend() = default;

    static std::unique_ptr<AndroidBackend> Create(AndroidWindow* window, bool capFrameRate) {
        return mbgl::gfx::Backend::Create<AndroidBackend, AndroidWindow*, bool>(window, capFrameRate);
    }

    virtual mbgl::gfx::RendererBackend& getRendererBackend() = 0;
    virtual mbgl::Size getSize() const = 0;
    virtual void setSize(mbgl::Size) = 0;
};
