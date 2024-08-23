#pragma once

#include <mbgl/map/mode.hpp>

#include <memory>

#include <mapbox/std/weak.hpp>

namespace mbgl {

class TransformState;
class FileSource;
class AnnotationManager;
class RouteLineLayerManager;
class ImageManager;
class GlyphManager;

class TileParameters {
public:
    const float pixelRatio;
    const MapDebugOptions debugOptions;
    const TransformState& transformState;
    std::shared_ptr<FileSource> fileSource;
    const MapMode mode;
    mapbox::base::WeakPtr<AnnotationManager> annotationManager;
    mapbox::base::WeakPtr<RouteLineLayerManager> routeManager;
    ImageManager& imageManager;
    GlyphManager& glyphManager;
    const uint8_t prefetchZoomDelta;
};

} // namespace mbgl
