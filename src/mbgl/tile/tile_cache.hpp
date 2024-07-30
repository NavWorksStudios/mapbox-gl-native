#pragma once

#include <mbgl/tile/tile_id.hpp>
#include <mbgl/tile/tile.hpp>


#include <list>
#include <memory>
#include <unordered_map>

namespace mbgl {

class TileCache {
public:
    TileCache(size_t size_ = 0) : size(size_) {}

    void setSize(size_t);
    size_t getSize() const { return size; };
    void add(const OverscaledTileID& key, std::unique_ptr<Tile> tile);
    std::unique_ptr<Tile> pop(const OverscaledTileID& key);
    Tile* get(const OverscaledTileID& key);
    bool has(const OverscaledTileID& key);
    void clear();

private:
    std::unordered_map<OverscaledTileID, std::unique_ptr<Tile>> tiles;
    std::list<OverscaledTileID> orderedKeys;

    size_t size;
};

} // namespace mbgl
