#include "android_gl_backend.hpp"

#include <mbgl/gfx/backend_scope.hpp>
#include <mbgl/gl/renderable_resource.hpp>
#include "mbgl/nav/nav.style.hpp"

#include <mbgl/platform/GLES2/gl2.h>
#include <mbgl/platform/GLES2/gl2ext.h>

#if __APPLE__
#else
#include <EGL/egl.h>
#endif

class AndroidGLRenderableResource final : public mbgl::gl::RenderableResource {
public:
    explicit AndroidGLRenderableResource(AndroidGLBackend& backend_) : backend(backend_) {}

    void bind() override {
        backend.setFramebufferBinding(0);
        backend.setViewport(0, 0, backend.getSize());
    }

    void swap() override {
        backend.swap();
    }

private:
    AndroidGLBackend& backend;
};

AndroidGLBackend::AndroidGLBackend(AndroidWindow* window_, const bool capFrameRate)
    : mbgl::gl::RendererBackend(mbgl::gfx::ContextMode::Unique),
      mbgl::gfx::Renderable(
          [window_] {
            int fbWidth = nav::display::width();
            int fbHeight = nav::display::height();
                // #*# android 适配屏蔽
//              glfwGetFramebufferSize(window_, &fbWidth, &fbHeight);
              return mbgl::Size{ static_cast<uint32_t>(fbWidth), static_cast<uint32_t>(fbHeight) };
          }(),
          std::make_unique<AndroidGLRenderableResource>(*this)),
      window(window_) {
    // #*# android 适配屏蔽
//    glfwMakeContextCurrent(window);
    if (!capFrameRate) {
        // Disables vsync on platforms that support it.
        // #*# android 适配屏蔽
//        glfwSwapInterval(0);
    } else {
        // #*# android 适配屏蔽
//        glfwSwapInterval(1);
    }
}

AndroidGLBackend::~AndroidGLBackend() = default;

void AndroidGLBackend::activate() {
    // #*# android 适配屏蔽
//    glfwMakeContextCurrent(window);
}

void AndroidGLBackend::deactivate() {
    // #*# android 适配屏蔽
//    glfwMakeContextCurrent(nullptr);
}

mbgl::gl::ProcAddress AndroidGLBackend::getExtensionFunctionPointer(const char* name) {
    // #*# android适配，使用egl接口 - eglGetProcAddress
#if __APPLE__
    return nullptr;
#else
    return eglGetProcAddress(name);
#endif
}

void AndroidGLBackend::updateAssumedState() {
    // #*# android 适配屏蔽
    assumeFramebufferBinding(0);
    setViewport(0, 0, size);
}

mbgl::Size AndroidGLBackend::getSize() const {
    return size;
}

void AndroidGLBackend::setSize(const mbgl::Size newSize) {
    size = newSize;
}

void AndroidGLBackend::swap() {
    // #*# android 适配屏蔽
//    glfwSwapBuffers(window);
}

namespace mbgl {
namespace gfx {

template <>
std::unique_ptr<AndroidBackend>
Backend::Create<mbgl::gfx::Backend::Type::OpenGL>(AndroidWindow* window, bool capFrameRate) {
    return std::make_unique<AndroidGLBackend>(window, capFrameRate);
}

} // namespace gfx
} // namespace mbgl
