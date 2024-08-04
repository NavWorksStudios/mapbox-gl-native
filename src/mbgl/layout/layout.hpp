#pragma once

#include <mbgl/renderer/image_atlas.hpp>
#include <mbgl/text/glyph_atlas.hpp>
#include <mbgl/tile/geometry_tile_data.hpp>
#include <memory>

#include "mbgl/nav/nav.stringid.hpp"
#include "mbgl/nav/nav.log.hpp"

namespace mbgl {

class Bucket;
class BucketParameters;
class RenderLayer;
class FeatureIndex;
class LayerRenderData;

class Layout {
public:
    virtual ~Layout() {
//        nav::log::i("Layout", "delete layout %p", this);
    }

    virtual void createBucket(const ImagePositions&,
                              std::unique_ptr<FeatureIndex>&,
                              std::unordered_map<nav::stringid, LayerRenderData>&,
                              bool,
                              bool,
                              const CanonicalTileID&) = 0;

    virtual void prepareSymbols(const GlyphMap&, const GlyphPositions&, const ImageMap&, const ImagePositions&){};

    virtual bool hasSymbolInstances() const {
        return true;
    };
    
    virtual bool hasDependencies() const = 0;
};

class LayoutParameters {
public:
    const BucketParameters& bucketParameters;
    GlyphDependencies& glyphDependencies;
    ImageDependencies& imageDependencies;
    std::set<std::string>& availableImages;
};

} // namespace mbgl
