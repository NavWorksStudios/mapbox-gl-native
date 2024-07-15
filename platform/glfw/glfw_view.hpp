#pragma once

#include <mbgl/map/map.hpp>
#include <mbgl/map/map_snapshotter.hpp>
#include <mbgl/util/geometry.hpp>
#include <mbgl/util/optional.hpp>
#include <mbgl/util/run_loop.hpp>
#include <mbgl/util/timer.hpp>
#if defined(MBGL_RENDER_BACKEND_OPENGL) && !defined(MBGL_LAYER_CUSTOM_DISABLE_ALL)
#include <mbgl/style/layers/location_indicator_layer.hpp>
#endif

#include <utility>

struct GLFWwindow;
class GLFWBackend;
class GLFWRendererFrontend;
class SnapshotObserver;

namespace mbgl {
namespace gfx {
class RendererBackend;
} // namespace gfx
} // namespace mbgl

class GLFWView : public mbgl::MapObserver {
public:
    GLFWView(bool fullscreen, bool benchmark, const mbgl::ResourceOptions &options, bool headless);
    ~GLFWView() override;

    float getPixelRatio() const;

    void setMap(mbgl::Map*);
    
    void setRenderFrontend(GLFWRendererFrontend*);

    mbgl::gfx::RendererBackend& getRendererBackend();

    void setTestDirectory(std::string dir) { testDirectory = std::move(dir); };

    // Callback called when the user presses the key mapped to style change.
    // The expected action is to set a new style, different to the current one.
    void setChangeStyleCallback(std::function<void()> callback);

    void setPauseResumeCallback(std::function<void()> callback) { pauseResumeCallback = std::move(callback); };

    void setOnlineStatusCallback(std::function<void()> callback) { onlineStatusCallback = std::move(callback); }

    void setResetCacheCallback(std::function<void()> callback) { resetDatabaseCallback = std::move(callback); };

    void setShouldClose();

    void setWindowTitle(const std::string&);

    void run();
    
    void invalidate();

    mbgl::Size getSize() const;

    // mbgl::MapObserver implementation
    void onDidFinishLoadingStyle() override;
    void onWillStartRenderingFrame() override;

protected:
    // mbgl::Backend implementation
    
public:
    void onKey(int key, int action, int mods);
    void onScroll(double yoffset);
    void onMouseClick(int button, int action, int modifiers);
    void onMouseMove(double x, double y);
    
private:
    // Window callbacks
    static void onKey(GLFWwindow *window, int key, int scancode, int action, int mods);
    static void onScroll(GLFWwindow *window, double xoffset, double yoffset);
    static void onWindowResize(GLFWwindow *window, int width, int height);
    static void onFramebufferResize(GLFWwindow *window, int width, int height);
    static void onMouseClick(GLFWwindow *window, int button, int action, int modifiers);
    static void onMouseMove(GLFWwindow *window, double x, double y);
    static void onWindowFocus(GLFWwindow *window, int focused);

    // Internal
    void report(float duration);

    mbgl::Color makeRandomColor() const;
    mbgl::Point<double> makeRandomPoint() const;
    static std::unique_ptr<mbgl::style::Image> makeImage(const std::string& id, int width, int height, float pixelRatio);

    void nextOrientation();

    void addRandomPointAnnotations(int count);
    void addLineAnnotations(const mbgl::LatLng& tagPosition);
    void hideCurrentLineAnnotations();
    void updateLineAnnotations(const mbgl::LatLng& orgPosition, const mbgl::LatLng& tagPosition);
    void addRandomShapeAnnotations(int count);
    void addRandomCustomPointAnnotations(int count);
    void addAnimatedAnnotation();
    void updateFreeCameraDemo();
    void updateAnimatedAnnotations();
    void toggleCustomSource();
    void toggleLocationIndicatorLayer(bool visibility);

    void cycleDebugOptions();
    void clearAnnotations();
    void popAnnotation();

    void makeSnapshot(bool withOverlay = false);

    mbgl::AnnotationIDs annotationIDs;
    std::vector<std::string> spriteIDs;

    mbgl::AnnotationIDs animatedAnnotationIDs;
    std::vector<double> animatedAnnotationAddedTimes;

private:
    void toggle3DExtrusions(bool visible);

    mbgl::Map* map = nullptr;
    GLFWRendererFrontend* rendererFrontend = nullptr;
    std::unique_ptr<GLFWBackend> renderBackend;

    std::string testDirectory = ".";

    double freeCameraDemoPhase = -1;
    mbgl::TimePoint freeCameraDemoStartTime;
    bool fullscreen = false;
    const bool benchmark = false;
    bool tracking = false;
    bool rotating = false;
    bool pitching = false;
    bool show3DExtrusions = false;

    int width = 1800;
    int height = 720;
    float pixelRatio;

    struct Mouse {
        mbgl::ScreenCoordinate coord;
        double time { 0 };
        int sequenceNum { 0 };
        
        float velocity(const Mouse& from) const {
            const float duration = time - from.time;
            const float distance = pow(pow(coord.x-from.coord.x, 2.) + pow(coord.y-from.coord.y, 2.), 0.5);
            return duration > 0. ? distance / duration : 0.;
        }
        
        bool prefer(double t, int s) {
            return (t < time && s == sequenceNum);
        }
    };

    struct MouseHistory {
        std::deque<Mouse> history;
        int sequenceNum { 0 };
        
        void push_back(mbgl::ScreenCoordinate coord, double time) {
            history.push_back({ coord, time, sequenceNum });
            if (history.size() > 100) history.pop_front();
        }
        
        size_t size() const { return history.size(); }
        void newSequence() { sequenceNum++; }
        
        const Mouse& operator [] (int index) const {
            return history.at(fmin(fmax(0, history.size() - 1 + index), history.size()));
        }

        const Mouse& prefer(double time) {
            auto it = history.end() - 1;
            while (it != history.begin() && (it - 1)->prefer(time, sequenceNum)) {
                it--;
            }
            return *it;
        }
    } _mouseHistory;

    std::function<void()> changeStyleCallback;
    std::function<void()> pauseResumeCallback;
    std::function<void()> onlineStatusCallback;
    std::function<void()> resetDatabaseCallback;
    std::function<void(mbgl::Map*)> animateRouteCallback;

    mbgl::util::RunLoop runLoop;
    mbgl::util::Timer frameTick;

    GLFWwindow *window = nullptr;
    bool dirty = false;
    mbgl::optional<std::string> featureID;
    std::unique_ptr<mbgl::MapSnapshotter> snapshotter;
    std::unique_ptr<SnapshotObserver> snapshotterObserver;
    mbgl::ResourceOptions mapResourceOptions;

#if defined(MBGL_RENDER_BACKEND_OPENGL) && !defined(MBGL_LAYER_CUSTOM_DISABLE_ALL)
    bool routePaused = false;
    bool puckFollowsCameraCenter = true;
    float notFollowCounter = 0;
    mapbox::geometry::point<double> lastPoint;
    mbgl::style::LocationIndicatorLayer *puck = nullptr;
    mbgl::LatLng targetLatLng = {0.0, 0.0};
#endif
};
