#pragma once

#include <mbgl/map/zoom_history.hpp>
#include <mbgl/util/chrono.hpp>

namespace mbgl {

class CrossfadeParameters {
public:
    float fromScale;
    float toScale;
    float t;
};

class PropertyEvaluationParameters {
public:
    explicit PropertyEvaluationParameters(float zoom_)
        : zoom(zoom_), now(Clock::time_point::max()), defaultFadeDuration(0) { }

    PropertyEvaluationParameters(ZoomHistory zoomHistory_, TimePoint now_, Duration defaultFadeDuration_)
        : zoom(zoomHistory_.lastZoom), now(now_), zoomHistory(zoomHistory_), defaultFadeDuration(defaultFadeDuration_) {}

    CrossfadeParameters getCrossfadeParameters() const {
        const float fraction = zoom - std::floor(zoom);
        const std::chrono::duration<float> d = defaultFadeDuration;
        const float t = d != std::chrono::duration<float>::zero()
                ? std::min((now - zoomHistory.lastIntegerZoomTime) / d, 1.0f)
                : 1.0f;

        return zoom > zoomHistory.lastIntegerZoom
            ? CrossfadeParameters { 2.0f, 1.0f, fraction + (1.0f - fraction) * t }
            : CrossfadeParameters { 0.5f, 1.0f, 1 - (1 - t) * fraction };
    }

    float zoom;
    TimePoint now;
    ZoomHistory zoomHistory;
    Duration defaultFadeDuration;
};

} // namespace mbgl
