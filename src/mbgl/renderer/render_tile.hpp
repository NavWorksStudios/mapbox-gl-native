#pragma once

#include <mbgl/map/mode.hpp>
#include <mbgl/gfx/texture.hpp>
#include <mbgl/tile/tile_id.hpp>
#include <mbgl/util/mat4.hpp>
#include <mbgl/renderer/image_atlas.hpp>
#include <mbgl/style/layer_impl.hpp>
#include <mbgl/style/types.hpp>

#include <array>
#include <memory>

namespace mbgl {

namespace gfx {
class UploadPass;
} // namespace gfx

class Bucket;
class LayerRenderData;
class Tile;
class TransformState;
class PaintParameters;
class DebugBucket;
class SourcePrepareParameters;
class FeatureIndex;
class TileRenderData;

class RenderTile final {
public:
    RenderTile(UnwrappedTileID, Tile&);
    ~RenderTile();
    RenderTile(const RenderTile&) = delete;
    RenderTile(RenderTile&&) = default;
    RenderTile& operator=(const RenderTile&) = delete;

    UnwrappedTileID id;

    mat4 modelMatrix;
    mat4 viewMatrix;
    mat4 modelViewMatrix;
    
    mat4 modelNormalMatrix;
    mat4 viewNormalMatrix;
    mat4 modelViewNormalMatrix;
    
    // mvp matrix
    mat4 matrix; // the standard tile matrix
    mat4 nearClippedMatrix; // has near plane moved further, to enhance depth buffer precision
    
    // Contains the tile ID string for painting debug information.
    std::unique_ptr<DebugBucket> debugBucket;

    using Translation = const std::array<float, 2>;
    mat4 translatedMatrix(Translation& translation, style::TranslateAnchorType anchor, const TransformState&) const;
    mat4 translatedClipMatrix(Translation& translation, style::TranslateAnchorType anchor, const TransformState&) const;

    const OverscaledTileID& getOverscaledTileID() const;
    bool holdForFade() const;

    Bucket* getBucket(const style::Layer::Impl&) const;
    const LayerRenderData* getLayerRenderData(const style::Layer::Impl&) const;
    optional<ImagePosition> getPattern(const std::string& pattern) const;
    const gfx::Texture& getGlyphAtlasTexture() const;
    const gfx::Texture& getIconAtlasTexture() const;

    void upload(gfx::UploadPass&) const;
    void prepare(const SourcePrepareParameters&);
    void finishRender(PaintParameters&) const;

    mat4 translateVtxMatrix(const mat4& tileMatrix,
                            const std::array<float, 2>& translation,
                            style::TranslateAnchorType anchor,
                            const TransformState& state,
                            bool inViewportPixelUnits) const;

    void setFeatureState(const LayerFeatureStates&);
    bool isRenderable(int renderMode) const;

private:
    Tile& tile;
    // The following members are reset at placement stage.
    std::unique_ptr<TileRenderData> renderData;
//    bool needsRendering = false;
    bool needsRendering = true;
};

} // namespace mbgl
