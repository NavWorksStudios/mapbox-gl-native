#pragma once

#include <mbgl/tile/tile_id.hpp>
#include <mbgl/tile/tile_necessity.hpp>
#include <mbgl/util/optional.hpp>
#include <mbgl/util/range.hpp>

#include <unordered_set>

namespace mbgl {
namespace algorithm {

template <typename GetTileFn,
          typename CreateTileFn,
          typename RetainTileFn,
          typename SetTileToRenderFn,
          typename SetRenderModeFn,
          typename IdealTileIDs>
void updateRenderables(GetTileFn getTile,
                       CreateTileFn createTile,
                       RetainTileFn retainTile,
                       SetTileToRenderFn setTileToRender,
                       SetRenderModeFn setTileRenderMode,
                       const IdealTileIDs& tileIDs,
                       const Range<uint8_t>& zoomRange,
                       const optional<uint8_t>& maxParentOverscaleFactor = nullopt) {
    std::unordered_set<OverscaledTileID> checked;
    bool covered;
    int32_t overscaledZ;

    // for (all in the set of ideal tiles of the source) {
    for (const auto& tileID : tileIDs) {
        assert(tileID.canonical.z >= zoomRange.min);
        assert(tileID.canonical.z <= zoomRange.max);
        assert(tileID.overscaledZ >= tileID.canonical.z);

        const UnwrappedTileID renderTileID = tileID.toUnwrapped();
        auto tile = getTile(tileID);
        if (!tile) {
            tile = createTile(tileID);
            // For source types where TileJSON.bounds is set, tiles outside the
            // bounds are not created
            if(tile == nullptr) {
                continue;
            }
        }

        // if (source has the tile and bucket is loaded) {
        if (tile->isRenderable()) {
            retainTile(*tile, TileNecessity::Required);
            setTileToRender(renderTileID, *tile);
            setTileRenderMode(*tile);
        } else {
            // We are now attempting to load child and parent tiles.
            bool parentHasTriedOptional = tile->hasTriedCache();
            bool parentIsLoaded = tile->isLoaded();

            // The tile isn't loaded yet, but retain it anyway because it's an ideal tile.
            retainTile(*tile, TileNecessity::Required);
            covered = true;
            overscaledZ = tileID.overscaledZ + 1;
            if (overscaledZ > zoomRange.max) {
                // We're looking for an overzoomed child tile.
                const auto childDataTileID = tileID.scaledTo(overscaledZ);
                tile = getTile(childDataTileID);
                if (tile && tile->isRenderable()) {
                    retainTile(*tile, TileNecessity::Optional);
                    setTileToRender(renderTileID, *tile);
                    setTileRenderMode(*tile);
                } else {
                    covered = false;
                }
            } else {
                // Check all four actual child tiles.
                for (const auto& childTileID : tileID.canonical.children()) {
                    const OverscaledTileID childDataTileID(overscaledZ, renderTileID.wrap, childTileID);
                    tile = getTile(childDataTileID);
                    if (tile && tile->isRenderable()) {
                        retainTile(*tile, TileNecessity::Optional);
                        setTileToRender(childDataTileID.toUnwrapped(), *tile);
                        setTileRenderMode(*tile);
                    } else {
                        // At least one child tile doesn't exist, so we are going to look for
                        // parents as well.
                        covered = false;
                    }
                }
            }

            if (!covered) {
                // We couldn't find child tiles that entirely cover the ideal tile.
                for (overscaledZ = tileID.overscaledZ - 1; overscaledZ >= zoomRange.min; --overscaledZ) {
                    const auto parentDataTileID = tileID.scaledTo(overscaledZ);

                    // Request / render parent tile only if it's overscale factor is less than defined maximum.
                    if (maxParentOverscaleFactor &&
                        (tileID.overscaledZ - overscaledZ) > *maxParentOverscaleFactor) {
                        break;
                    }

                    if (checked.find(parentDataTileID) != checked.end()) {
                        // Break parent tile ascent, this route has been checked by another child
                        // tile before.
                        break;
                    } else {
                        checked.emplace(parentDataTileID);
                    }

                    tile = getTile(parentDataTileID);
                    if (!tile && (parentHasTriedOptional || parentIsLoaded)) {
                        tile = createTile(parentDataTileID);
                    }

                    if (tile) {
                        if (!parentIsLoaded) {
                            // We haven't completed loading the child, so we only do an optional
                            // (cache) request in an attempt to quickly load data that we can show.
                            retainTile(*tile, TileNecessity::Optional);
                        } else {
                            // Now that we've checked the child and know for sure that we can't load
                            // it, we attempt to load the parent from the network.
                            retainTile(*tile, TileNecessity::Required);
                        }

                        // Save the current values, since they're the parent of the next iteration
                        // of the parent tile ascent loop.
                        parentHasTriedOptional = tile->hasTriedCache();
                        parentIsLoaded = tile->isLoaded();

                        if (tile->isRenderable()) {
                            setTileToRender(parentDataTileID.toUnwrapped(), *tile);
                            setTileRenderMode(*tile);
                            // Break parent tile ascent, since we found one.
                            break;
                        }
                    }
                }
            }
        }
    }
}

} // namespace algorithm
} // namespace mbgl
