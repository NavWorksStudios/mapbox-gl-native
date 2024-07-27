#include <mbgl/tile/tile_id.hpp>
#include <mbgl/util/hash.hpp>

namespace std {

size_t hash<mbgl::CanonicalTileID>::operator()(const mbgl::CanonicalTileID& id) const {
//    std::size_t seed = 0;
//    mbgl::util::hash_combine(seed, id.x);
//    mbgl::util::hash_combine(seed, id.y);
//    mbgl::util::hash_combine(seed, id.z);
//    return seed;
    
    std::size_t seed;
    uint32_t* p = (uint32_t*) &seed;
    p[0] = id.x + id.z;
    p[1] = id.y + 8249;
    return seed;
}

size_t hash<mbgl::UnwrappedTileID>::operator()(const mbgl::UnwrappedTileID& id) const {
//    std::size_t seed = 0;
//    mbgl::util::hash_combine(seed, std::hash<mbgl::CanonicalTileID>{}(id.canonical));
//    mbgl::util::hash_combine(seed, id.wrap);
//    return seed;
    
    std::size_t seed;
    uint32_t* p = (uint32_t*) &seed;
    p[0] = id.canonical.x + id.canonical.z;
    p[1] = id.canonical.y + id.wrap + 8249;
    return seed;
}

size_t hash<mbgl::OverscaledTileID>::operator()(const mbgl::OverscaledTileID& id) const {
//    std::size_t seed = 0;
//    mbgl::util::hash_combine(seed, std::hash<mbgl::CanonicalTileID>{}(id.canonical));
//    mbgl::util::hash_combine(seed, id.overscaledZ);
//    return seed;
    
    std::size_t seed;
    uint32_t* p = (uint32_t*) &seed;
    p[0] = id.canonical.x + id.canonical.z + id.overscaledZ;
    p[1] = id.canonical.y + id.wrap + 8249;
    return seed;
}

} // namespace std
