#pragma once

#include <mbgl/tile/tile_id.hpp>
#include <mbgl/style/types.hpp>
#include <mbgl/util/geometry.hpp>
#include <mbgl/util/optional.hpp>
#include <mbgl/util/range.hpp>

#include <vector>
#include <memory>

namespace mbgl {

class TransformState;
class LatLngBounds;

namespace util {

// Helper class to stream tile-cover results per row
class TileCover {
public:
    TileCover(const LatLngBounds&, uint8_t z);
    // When project == true, projects the geometry points to tile coordinates
    TileCover(const Geometry<double>&, uint8_t z, bool project = true);
    ~TileCover();

    optional<UnwrappedTileID> next();
    bool hasNext();

private:
    class Impl;
    std::unique_ptr<Impl> impl;
};

int32_t coveringZoomLevel(double z, style::SourceType type, uint16_t tileSize);

namespace strategy {
struct Type {
    const float LOD_INTENSITY;
    Range<uint8_t> Z_RANGE;
};
Type standard(const TransformState& state);
Type detailed();
}

void tileCover(std::vector<OverscaledTileID>& ids, const strategy::Type& strategy,
               const TransformState& state, uint8_t z, const optional<uint8_t>& overscaledZ = nullopt);
void tileCover(std::vector<UnwrappedTileID>& ids, const LatLngBounds&, uint8_t z);
void tileCover(std::vector<UnwrappedTileID>& ids, const Geometry<double>&, uint8_t z);

// Compute only the count of tiles needed for tileCover
uint64_t tileCount(const LatLngBounds&, uint8_t z);
uint64_t tileCount(const Geometry<double>&, uint8_t z);

} // namespace util
} // namespace mbgl
