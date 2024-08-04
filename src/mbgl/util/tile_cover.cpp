#include <mbgl/map/transform_state.hpp>
#include <mbgl/math/log2.hpp>
#include <mbgl/util/bounding_volumes.hpp>
#include <mbgl/util/constants.hpp>
#include <mbgl/util/interpolate.hpp>
#include <mbgl/util/tile_coordinate.hpp>
#include <mbgl/util/tile_cover.hpp>
#include <mbgl/util/tile_cover_impl.hpp>

#include <functional>
#include <list>

#include "mbgl/nav/nav.log.hpp"

namespace mbgl {

namespace {

using ScanLine = const std::function<void(int32_t x0, int32_t x1, int32_t y)>;

// Taken from polymaps src/Layer.js
// https://github.com/simplegeo/polymaps/blob/master/src/Layer.js#L333-L383
struct edge {
    double x0 = 0, y0 = 0;
    double x1 = 0, y1 = 0;
    double dx = 0, dy = 0;

    edge(Point<double> a, Point<double> b) {
        if (a.y > b.y) std::swap(a, b);
        x0 = a.x;
        y0 = a.y;
        x1 = b.x;
        y1 = b.y;
        dx = b.x - a.x;
        dy = b.y - a.y;
    }
};

// scan-line conversion
void scanSpans(edge e0, edge e1, int32_t ymin, int32_t ymax, ScanLine& scanLine) {
    double y0 = ::fmax(ymin, std::floor(e1.y0));
    double y1 = ::fmin(ymax, std::ceil(e1.y1));

    // sort edges by x-coordinate
    if ((e0.x0 == e1.x0 && e0.y0 == e1.y0) ?
        (e0.x0 + e1.dy / e0.dy * e0.dx < e1.x1) :
        (e0.x1 - e1.dy / e0.dy * e0.dx < e1.x0)) {
        std::swap(e0, e1);
    }

    // scan lines!
    double m0 = e0.dx / e0.dy;
    double m1 = e1.dx / e1.dy;
    double d0 = e0.dx > 0; // use y + 1 to compute x0
    double d1 = e1.dx < 0; // use y + 1 to compute x1
    for (int32_t y = y0; y < y1; y++) {
        double x0 = m0 * ::fmax(0, ::fmin(e0.dy, y + d0 - e0.y0)) + e0.x0;
        double x1 = m1 * ::fmax(0, ::fmin(e1.dy, y + d1 - e1.y0)) + e1.x0;
        scanLine(std::floor(x1), std::ceil(x0), y);
    }
}

// scan-line conversion
void scanTriangle(const Point<double>& a,
                  const Point<double>& b,
                  const Point<double>& c,
                  int32_t ymin,
                  int32_t ymax,
                  ScanLine& scanLine) {
    edge ab = edge(a, b);
    edge bc = edge(b, c);
    edge ca = edge(c, a);

    // sort edges by y-length
    if (ab.dy > bc.dy) { std::swap(ab, bc); }
    if (ab.dy > ca.dy) { std::swap(ab, ca); }
    if (bc.dy > ca.dy) { std::swap(bc, ca); }

    // scan span! scan span!
    if (ab.dy) scanSpans(ca, ab, ymin, ymax, scanLine);
    if (bc.dy) scanSpans(ca, bc, ymin, ymax, scanLine);
}

} // namespace

namespace util {

namespace {

void tileCover(std::vector<UnwrappedTileID>& ids,
               const Point<double>& tl, 
               const Point<double>& tr,
               const Point<double>& br, 
               const Point<double>& bl,
               const Point<double>& c, 
               uint8_t z) {
    ids.clear();

    const int32_t tiles = 1 << z;

    struct ID {
        int32_t x, y;
        double sqDist;
    };

    static std::vector<ID> t;

    auto scanLine = [&](int32_t x0, int32_t x1, int32_t y) {
        int32_t x;
        if (y >= 0 && y <= tiles) {
            for (x = x0; x < x1; ++x) {
                const auto dx = x + 0.5 - c.x;
                const auto dy = y + 0.5 - c.y;
                t.emplace_back(ID{ x, y, dx * dx + dy * dy });
            }
        }
    };

    // Divide the screen up in two triangles and scan each of them:
    // \---+
    // | \ |
    // +---\.
    scanTriangle(tl, tr, br, 0, tiles, scanLine);
    scanTriangle(br, bl, tl, 0, tiles, scanLine);

    // Sort first by distance, then by x/y.
    std::sort(t.begin(), t.end(), [](const ID& a, const ID& b) {
        return std::tie(a.sqDist, a.x, a.y) < std::tie(b.sqDist, b.x, b.y);
    });

    // Erase duplicate tile IDs (they typically occur at the common side of both triangles).
    t.erase(std::unique(t.begin(), t.end(), [](const ID& a, const ID& b) {
                return a.x == b.x && a.y == b.y;
            }), t.end());

    for (const auto& id : t) {
        ids.emplace_back(z, id.x, id.y);
    }
}

} // namespace

int32_t coveringZoomLevel(double zoom, style::SourceType type, uint16_t size) {
    zoom += util::log2(util::tileSize / size);
    if (type == style::SourceType::Raster || type == style::SourceType::Video) {
        return ::round(zoom);
    } else {
        return std::floor(zoom);
    }
}

void tileCover(std::vector<OverscaledTileID>& ids, const strategy::Type& strategy, const TransformState& state, uint8_t z, const optional<uint8_t>& overscaledZ) {
    ids.clear();

    struct Node {
        AABB aabb;
        uint8_t zoom;
        uint32_t x, y;
        int16_t wrap;
        bool fullyVisible;
    };

    struct ResultTile {
        OverscaledTileID id;
        double sqrDist;
    };

    // z级有多少tile
    const double numTiles = std::pow(2.0, z);
    // 按照逻辑tile尺寸512，在当前比例尺下的大小
    const double worldSize = Projection::worldSize(state.getScale());
    
    // LOD降级取值范围
    constexpr float MAX_PITCH = (70.0 / 180.0) * M_PI;
    const uint8_t minZoom = (1. - state.getPitch() / MAX_PITCH) * (z - 2);
    const uint8_t maxZoom = fmin(z, strategy.Z_RANGE.max);
    const uint8_t minSeenZoom = fmax(fmax(maxZoom - 5, minZoom), strategy.Z_RANGE.min);
    
    const uint8_t overscaledZoom = overscaledZ.value_or(z);
    const bool flippedY = state.getViewportMode() == ViewportMode::FlippedY;

    // 通过屏幕坐标中心点，计算当前缩放级别坐标系中，地图中心点坐标
    const ScreenCoordinate screenCenter(state.getSize().width / 2.0, state.getSize().height / 2.0);
    auto centerPoint = TileCoordinate::fromScreenCoordinate(state, z, screenCenter).p;
    vec3 centerCoord = {centerPoint.x, centerPoint.y, 0.0};

    // 计算当前缩放级别坐标系中，平截头体空间
    const Frustum frustum = Frustum::fromInvProjMatrix(state.getInvProjectionMatrix(), worldSize, z, flippedY);

    // 围绕中心点附近，保证几个当前最高级别（最详尽级别）的tile
    // There should always be a certain number of maximum zoom level tiles surrounding the center location
    const double radiusOfMaxLvlLodInTiles = 3;

    // 制作左3，右3，中间1，共7个最低等级的tile，用来判断可见性，并往细粒度拆分
    const auto newRootTile = [&](int16_t wrap) -> Node {
        const vec3 min = { wrap * numTiles, 0.0, 0.0 };
        const vec3 max = { (wrap + 1) * numTiles, numTiles, 0.0 };
        const AABB envelopeBox(min, max);
        return {
            envelopeBox,                // AABB
            uint8_t(0),                 // zoom
            uint16_t(0), uint16_t(0),   // x, y
            wrap,                       // wrap
            false                       // fully visible
        };
    };
    
    // Perform depth-first traversal on tile tree to find visible tiles
    // 深度优先遍历树，计算可见瓦片
    static std::vector<Node> stack;
    static std::vector<ResultTile> result;
    
    stack.clear();
    result.clear();

    // World copies shall be rendered three times on both sides from closest to farthest
    // 世界副本应从最近到最远的两侧各呈现三次
    for (int i = 1; i <= 3; i++) {
        stack.push_back(newRootTile(-i));
        stack.push_back(newRootTile(i));
    }

    stack.push_back(newRootTile(0));

    // 开始从低等级向高等级拆分，收集由远及近、由粗到细LOD的tile序列
    while (!stack.empty()) { // 如果还存在需要拆分的tile，继续循环
        Node node = stack.back();
        stack.pop_back();

        // Use cached visibility information of ancestor nodes
        if (!node.fullyVisible) {
            const IntersectionResult intersection = frustum.intersects(node.aabb);
            // 如果该tile和平截头体不相交，则丢弃该tile
            if (intersection == IntersectionResult::Separate) continue;
            
            // 如果包含关系，则可见
            node.fullyVisible = (intersection == IntersectionResult::Contains);
        }

        // 该tile距离地图中心点，在xyz分量中最大值
        const vec3 distanceXyz = node.aabb.distanceXYZ(centerCoord);
        const double* longestDimension = std::max_element(distanceXyz.data(), distanceXyz.data() + distanceXyz.size());
        assert(longestDimension);

        // 计算该比例尺下需要拆分tile的距离阈值，进入更细级别的距离阈值
        // We're using distance based heuristics to determine if a tile should be split into quadrants or not.
        // radiusOfMaxLvlLodInTiles defines that there's always a certain number of maxLevel tiles next to the map
        // center. Using the fact that a parent node in quadtree is twice the size of its children (per dimension) we
        // can define distance thresholds for each relative level: f(k) = offset + 2 + 4 + 8 + 16 + ... + 2^k. This is
        // the same as "offset+2^(k+1)-2"
        // 使用基于距离的启发式，来决定瓦片是否应该继续细化拆分四块。radiusOfMaxLvlLodInTiles定义了地图中心点周边保证有一定数量的最大级别图块。
        // 利用四叉树中的父节点是其子节点大小的两倍特性，为每个级别定义距离阈值：f(k) = offset + 2 + 4 + 8 + 16 + ... + 2^k. 这是与"offset+2^(k+1)-2"相同
        const double distanceToSplit = (radiusOfMaxLvlLodInTiles + (1 << (maxZoom - node.zoom)) - 2) / strategy.LOD_INTENSITY;

        // 不能再拆分的tile直接进入结果列表，包括：
        // 已经是显示的最大级别而不能再拆分的，或者是超过该级别拆分距离而不能再拆分的
        // Have we reached the target depth or is the tile too far away to be any split further?
        if ((node.zoom == maxZoom) ||
            (*longestDimension > distanceToSplit && node.zoom >= minZoom)) {
            // 再次进行校验，必须粗相交测试全部可见，或者精确相交测试后可见的
            // Perform precise intersection test between the frustum and aabb. This will cull < 1% false positives
            // missed by the original test
            const bool visible = node.fullyVisible || (frustum.intersectsPrecise(node.aabb, true) != IntersectionResult::Separate);
            if (visible && node.zoom >= minSeenZoom) {
                const OverscaledTileID id = {
                    node.zoom == maxZoom ? overscaledZoom : node.zoom,
                    node.wrap, 
                    node.zoom,
                    node.x, node.y };

                const double dx = node.wrap * numTiles + node.x + 0.5 - centerCoord[0];
                const double dy = node.y + 0.5 - centerCoord[1];

                result.push_back({id, dx * dx + dy * dy});
            }
            continue;
        }

        // 需要继续拆分的tile被1拆4，重复上述操作，直至没有tile需要拆分
        for (int i = 0; i < 4; i++) {
            const uint32_t childX = (node.x << 1) + (i % 2);
            const uint32_t childY = (node.y << 1) + (i >> 1);

            // Create child node and push to the stack for traversal
            Node child = node;

            child.aabb = node.aabb.quadrant(i);
            child.zoom = node.zoom + 1;
            child.x = childX;
            child.y = childY;

            stack.push_back(child);
        }
    }

    // 由近及远排序结果列表
    // Sort results by distance
    auto compare = [](const ResultTile& a, const ResultTile& b) {
        return a.sqrDist < b.sqrDist;
    };
    std::sort(result.begin(), result.end(), compare);

    // 组织结果tile id列表
    for (const auto& tile : result) {
        ids.push_back(tile.id);
    }
}

void tileCover(std::vector<UnwrappedTileID>& ids, const LatLngBounds& bounds_, uint8_t z) {
    ids.clear();

    if (bounds_.isEmpty() ||
        bounds_.south() >  util::LATITUDE_MAX ||
        bounds_.north() < -util::LATITUDE_MAX) {
        return;
    }

    LatLngBounds bounds = LatLngBounds::hull(
        { std::max(bounds_.south(), -util::LATITUDE_MAX), bounds_.west() },
        { std::min(bounds_.north(),  util::LATITUDE_MAX), bounds_.east() });

    tileCover(ids,
        Projection::project(bounds.northwest(), z),
        Projection::project(bounds.northeast(), z),
        Projection::project(bounds.southeast(), z),
        Projection::project(bounds.southwest(), z),
        Projection::project(bounds.center(), z),
        z);
}

void tileCover(std::vector<UnwrappedTileID>& ids, const Geometry<double>& geometry, uint8_t z) {
    ids.clear();

    TileCover tc(geometry, z, true);
    while (tc.hasNext()) {
        ids.push_back(*tc.next());
    };

    return ids;
}

// Taken from https://github.com/mapbox/sphericalmercator#xyzbbox-zoom-tms_style-srs
// Computes the projected tiles for the lower left and upper right points of the bounds
// and uses that to compute the tile cover count
uint64_t tileCount(const LatLngBounds& bounds, uint8_t zoom){
    if (zoom == 0) {
        return 1;
    }
    auto sw = Projection::project(bounds.southwest(), zoom);
    auto ne = Projection::project(bounds.northeast(), zoom);
    auto maxTile = std::pow(2.0, zoom);
    auto x1 = floor(sw.x);
    auto x2 = ceil(ne.x) - 1;
    auto y1 = util::clamp(floor(sw.y), 0.0, maxTile - 1);
    auto y2 = util::clamp(floor(ne.y), 0.0, maxTile - 1);

    auto dx = x1 > x2 ? (maxTile - x1) + x2 : x2 - x1;
    auto dy = y1 - y2;
    return (dx + 1) * (dy + 1);
}

uint64_t tileCount(const Geometry<double>& geometry, uint8_t z) {
    uint64_t tileCount = 0;

    TileCover tc(geometry, z, true);
    while (tc.next()) {
        tileCount++;
    };
    return tileCount;
}

TileCover::TileCover(const LatLngBounds&bounds_, uint8_t z) {
    LatLngBounds bounds = LatLngBounds::hull(
        { std::max(bounds_.south(), -util::LATITUDE_MAX), bounds_.west() },
        { std::min(bounds_.north(),  util::LATITUDE_MAX), bounds_.east() });

    if (bounds.isEmpty() ||
        bounds.south() >  util::LATITUDE_MAX ||
        bounds.north() < -util::LATITUDE_MAX) {
        bounds = LatLngBounds::world();
    }

    auto sw = Projection::project(bounds.southwest(), z);
    auto ne = Projection::project(bounds.northeast(), z);
    auto se = Projection::project(bounds.southeast(), z);
    auto nw = Projection::project(bounds.northwest(), z);

    Polygon<double> p({ {sw, nw, ne, se, sw} });
    impl = std::make_unique<TileCover::Impl>(z, p, false);
}

TileCover::TileCover(const Geometry<double>& geom, uint8_t z, bool project/* = true*/)
 : impl( std::make_unique<TileCover::Impl>(z, geom, project)) {
}

TileCover::~TileCover() = default;

optional<UnwrappedTileID> TileCover::next() {
    return impl->next();
}

bool TileCover::hasNext() {
    return impl->hasNext();
}

} // namespace util
} // namespace mbgl
