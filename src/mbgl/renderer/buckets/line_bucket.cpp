#include <mbgl/renderer/buckets/line_bucket.hpp>
#include <mbgl/renderer/bucket_parameters.hpp>
#include <mbgl/style/layers/line_layer_impl.hpp>
#include <mbgl/util/math.hpp>
#include <mbgl/util/constants.hpp>

#include <cassert>
#include <utility>

namespace mbgl {

using namespace style;

LineBucket::LineBucket(LineBucket::PossiblyEvaluatedLayoutProperties layout_,
                       const std::map<nav::stringid, Immutable<LayerProperties>>& layerPaintProperties,
                       const float zoom_,
                       const uint32_t overscaling_)
    : layout(std::move(layout_)), zoom(zoom_), overscaling(overscaling_) {
    for (const auto& pair : layerPaintProperties) {
        paintPropertyBinders.emplace(
            std::piecewise_construct,
            std::forward_as_tuple(pair.first),
            std::forward_as_tuple(
                getEvaluated<LineLayerProperties>(pair.second),
                zoom));
    }
    
    if (paintPropertyBinders.size() == 1) {
        paintBinders = &paintPropertyBinders.begin()->second;
    }
}

LineBucket::~LineBucket() = default;

void LineBucket::addFeature(const GeometryTileFeature& feature,
                            const GeometryCollection& geometryCollection,
                            const ImagePositions& patternPositions,
                            const PatternLayerMap& patternDependencies,
                            std::size_t index,
                            const CanonicalTileID& canonical) {
    for (auto& line : geometryCollection) {
        addGeometry(line, feature, canonical);
    }

    for (auto& pair : paintPropertyBinders) {
        const auto it = patternDependencies.find(pair.first);
        if (it != patternDependencies.end()){
            pair.second.populateVertexVectors(
                feature, vertices.elements(), index, patternPositions, it->second, canonical);
        } else {
            pair.second.populateVertexVectors(feature, vertices.elements(), index, patternPositions, {}, canonical);
        }
    }
}

/*
 * Sharp corners cause dashed lines to tilt because the distance along the line
 * is the same at both the inner and outer corners. To improve the appearance of
 * dashed lines we add extra points near sharp corners so that a smaller part
 * of the line is tilted.
 *
 * COS_HALF_SHARP_CORNER controls how sharp a corner has to be for us to add an
 * extra vertex. The default is 75 degrees.
 *
 * The newly created vertices are placed SHARP_CORNER_OFFSET pixels from the corner.
 */
const float COS_HALF_SHARP_CORNER = std::cos(75.0 / 2.0 * (M_PI / 180.0));
const float SHARP_CORNER_OFFSET = 15.0f;

// Angle per triangle for approximating round line joins.
const float DEG_PER_TRIANGLE = 20.0f;

// The number of bits that is used to store the line distance in the buffer.
const int LINE_DISTANCE_BUFFER_BITS = 14;

// We don't have enough bits for the line distance as we'd like to have, so
// use this value to scale the line distance (in tile units) down to a smaller
// value. This lets us store longer distances while sacrificing precision.
const float LINE_DISTANCE_SCALE = 1.0 / 2.0;

// The maximum line distance, in tile units, that fits in the buffer.
const float MAX_LINE_DISTANCE = std::pow(2, LINE_DISTANCE_BUFFER_BITS) / LINE_DISTANCE_SCALE;

class LineBucket::Distances {
public:
    Distances(double clipStart_, double clipEnd_, double total_)
    : clipStart(clipStart_), clipEnd(clipEnd_), total(total_) {}

    // Scale line distance from tile units to [0, 2^15).
    double scaleToMaxLineDistance(double tileDistance) const {
        double relativeTileDistance = tileDistance / total;
        if (std::isinf(relativeTileDistance) || std::isnan(relativeTileDistance)) {
            assert(false);
            relativeTileDistance = 0.0;
        }
        return (relativeTileDistance * (clipEnd - clipStart) + clipStart) * (MAX_LINE_DISTANCE - 1);
    }

private:
    double clipStart;
    double clipEnd;
    double total;
};

// 添加一条线段
void LineBucket::addGeometry(const GeometryCoordinates& coordinates,
                             const GeometryTileFeature& feature,
                             const CanonicalTileID& canonical) {

    const FeatureType type = feature.getType();
    
    // ======================================== 基础处理 ========================================

    const std::size_t len = [&coordinates] {
        std::size_t l = coordinates.size();
        // If the line has duplicate vertices at the end, adjust length to remove them.
        // 如果线条末端有重复的顶点，调整长度以将其删除。
        while (l >= 2 && coordinates[l - 1] == coordinates[l - 2]) {
            l--;
        }
        return l;
    }();

    const std::size_t first = [&coordinates, &len] {
        std::size_t i = 0;
        // If the line has duplicate vertices at the start, adjust index to remove them.
        // 如果线条开始处有重复的顶点，请调整索引以将其删除。
        while (i < len - 1 && coordinates[i] == coordinates[i + 1]) {
            i++;
        }
        return i;
    }();

    // Ignore invalid geometry.
    // 忽略无效的几何图形。
    if (len < (type == FeatureType::Polygon ? 3 : 2)) {
        return;
    }
    
    // ======================================== 计算此段路线距离长度 ========================================

    optional<Distances> lineDistances;

    const auto &props = feature.getProperties();
    auto clip_start_it = props.find("mapbox_clip_start");
    auto clip_end_it = props.find("mapbox_clip_end");
    if (clip_start_it != props.end() && clip_end_it != props.end()) {
        double total_length = 0.0;
        for (std::size_t i = first; i < len - 1; ++i) {
            total_length += util::dist<double>(coordinates[i], coordinates[i + 1]);
        }

        lineDistances = Distances{
            *numericValue<double>(clip_start_it->second), 
            *numericValue<double>(clip_end_it->second),
            total_length};
    }

    // ======================================== 路线属性，连接属性（join type）& 端点样式（cap type） ========================================
    // https://www.jianshu.com/p/55158aa236a4

    const LineJoinType joinType = layout.evaluate<LineJoin>(zoom, feature, canonical);
    const float miterLimit = (joinType == LineJoinType::Bevel) ? 1.05f : float(layout.get<LineMiterLimit>());
    const double sharpCornerOffset = (overscaling == 0) ?
        (SHARP_CORNER_OFFSET * (float(util::EXTENT) / util::tileSize)) :
        (overscaling <= 16.0 ? SHARP_CORNER_OFFSET * (float(util::EXTENT) / (util::tileSize * overscaling)) : 0.0f);

    const GeometryCoordinate firstCoordinate = coordinates[first];
    const LineCapType beginCap = layout.get<LineCap>();
    const LineCapType endCap = (type == FeatureType::Polygon) ? LineCapType::Butt : LineCapType(layout.get<LineCap>());

    // ======================================== 遍历添加所有点 ========================================

    double geoDistance = 0.0;
    bool startOfLine = true;
    optional<GeometryCoordinate> currentCoordinate;
    optional<GeometryCoordinate> prevCoordinate;
    optional<GeometryCoordinate> nextCoordinate;
    optional<Point<double>> prevNormal;
    optional<Point<double>> nextNormal;

    // the last three vertices added
    e1 = e2 = e3 = -1;

    if (type == FeatureType::Polygon) {
        currentCoordinate = coordinates[len - 2];
        nextNormal = util::perp(util::unit(convertPoint<double>(firstCoordinate - *currentCoordinate)));
    }

    const std::size_t startVertex = vertices.elements();
    std::vector<TriangleElement> triangleStore;

    for (std::size_t i = first; i < len; ++i) {
        float heightRatio = 1.; // [0,1]
        
        // 计算下一个点，循环或非循环的情况
        if (i < len - 1) {
            // just the next vertex
            nextCoordinate = coordinates[i + 1];
//            if (i == first && isTermination[0]) zHeightRatio = 0;
        } else {
            if (type == FeatureType::Polygon) {
                // if the line is closed, we treat the last vertex like the first
                // 如果线是闭合的，我们将最后一个顶点视为第一个顶点
                nextCoordinate = coordinates[first + 1];
            } else {
                // there is no next vertex
                nextCoordinate = {};
//                if (isTermination[1]) zHeightRatio = 0;
            }
        }

        // if two consecutive vertices exist, skip the current one
        // 如果存在两个重复点，则跳过当前顶点
        if (nextCoordinate && coordinates[i] == *nextCoordinate) {
            continue;
        }
        
        // 上一个点的法线
        if (nextNormal) {
            prevNormal = *nextNormal;
        }
        if (currentCoordinate) {
            prevCoordinate = *currentCoordinate;
        }
        
        // 当前点
        currentCoordinate = coordinates[i];

        // Calculate the normal towards the next vertex in this line. In case
        // there is no next vertex, pretend that the line is continuing straight,
        // meaning that we are just using the previous normal.
        // 计算该直线中下一个顶点的法线。万一没有下一个顶点，假设这条线是直的，这意味着我们只是在使用之前的法线。
        nextNormal = nextCoordinate ?
        util::perp(util::unit(convertPoint<double>(*nextCoordinate - *currentCoordinate))) :
        prevNormal;

        // If we still don't have a previous normal, this is the beginning of a
        // non-closed line, so we're doing a straight "join".
        // 如果没有之前的法线，这应该是非封闭线起点，所以我们直接“连接”。
        if (!prevNormal) {
            prevNormal = *nextNormal;
        }

        // Determine the normal of the join extrusion. It is the angle bisector
        // of the segments between the previous line and the next line.
        // In the case of 180° angles, the prev and next normals cancel each other out:
        // prevNormal + nextNormal = (0, 0), its magnitude is 0, so the unit vector would be
        // undefined. In that case, we're keeping the joinNormal at (0, 0), so that the cosHalfAngle
        // below will also become 0 and miterLength will become Infinity.
        // 确定连接挤压处的法线。它是前后线段的角平分线。
        // 如果是180°角，前后法线相互抵消：prevNormal+nextNormal=（0,0），其大小为0，因此单位向量为未定义。
        // 在这种情况下，我们将joinNormal保持在（0,0），这样cosHalfAngle下面也将变为0，miterLength将变为无穷大。
        Point<double> joinNormal = *prevNormal + *nextNormal;
        if (joinNormal.x != 0 || joinNormal.y != 0) {
            joinNormal = util::unit(joinNormal);
        }

        /*  joinNormal     prevNormal
         *             ↖      ↑
         *                .________. prevVertex
         *                |
         * nextNormal  ←  |  currentVertex
         *                |
         *     nextVertex !
         *
         */

        // Calculate cosines of the angle (and its half) using dot product.
        // 使用点积计算角度余弦（及其一半）。
        const double cosAngle = prevNormal->x * nextNormal->x + prevNormal->y * nextNormal->y;
        const double cosHalfAngle = joinNormal.x * nextNormal->x + joinNormal.y * nextNormal->y;

        // Calculate the length of the miter (the ratio of the miter to the width)
        // as the inverse of cosine of the angle between next and join normals.
        // 计算斜接的长度（斜接与宽度之比）作为下一个法线和连接法线之间角度的余弦的倒数。
        const double miterLength = (cosHalfAngle != 0) ? 1 / cosHalfAngle : std::numeric_limits<double>::infinity();

        // Approximate angle from cosine.
        // 余弦的近似角度。
        const double approxAngle = 2 * std::sqrt(2 - 2 * cosHalfAngle);

        const bool isSharpCorner = cosHalfAngle < COS_HALF_SHARP_CORNER && prevCoordinate && nextCoordinate;

        // 处理转折角度过大尖角点，填补一个新pre坐标点
        if (isSharpCorner && i > first) {
            const auto prevSegmentLength = util::dist<double>(*currentCoordinate, *prevCoordinate);
            if (prevSegmentLength > 2.0 * sharpCornerOffset) {
                // 伪造新的中间点
                GeometryCoordinate newPrevVertex = *currentCoordinate -
                convertPoint<int16_t>(util::round(convertPoint<double>(*currentCoordinate - *prevCoordinate) * (sharpCornerOffset / prevSegmentLength)));
                geoDistance += util::dist<double>(newPrevVertex, *prevCoordinate);
                addCurrentVertex(newPrevVertex, heightRatio, geoDistance, *prevNormal, 0, 0, false,
                                 startVertex, triangleStore, lineDistances);
                
                prevCoordinate = newPrevVertex;
            }
        }

        // The join if a middle vertex, otherwise the cap
        // 连接要么是中间顶点，要么是线帽
        const bool hasPrevNextVertex = prevCoordinate && nextCoordinate;
        LineJoinType currentJoin = joinType;
        const LineCapType currentCap = nextCoordinate ? beginCap : endCap;

        // 前后都有点
        if (hasPrevNextVertex) {
            if (currentJoin == LineJoinType::Round) {
                if (miterLength < layout.get<LineRoundLimit>()) {
                    currentJoin = LineJoinType::Miter;
                } else if (miterLength <= 2) {
                    currentJoin = LineJoinType::FakeRound;
                }
            }

            if (currentJoin == LineJoinType::Miter && miterLength > miterLimit) {
                currentJoin = LineJoinType::Bevel;
            }

            if (currentJoin == LineJoinType::Bevel) {
                // The maximum extrude length is 128 / 63 = 2 times the width of the line
                // so if miterLength >= 2 we need to draw a different type of bevel here.
                // 最大拉伸长度为 128/63 = 线条宽度的2倍。因此，如果miterLength>=2，我们需要在这里绘制一种不同类型的斜面。
                if (miterLength > 2) {
                    currentJoin = LineJoinType::FlipBevel;
                }

                // If the miterLength is really small and the line bevel wouldn't be visible,
                // just draw a miter join to save a triangle.
                // 如果斜接长度真的很小，并且线斜面不可见，只需绘制斜接以保存三角形面。
                if (miterLength < miterLimit) {
                    currentJoin = LineJoinType::Miter;
                }
            }
        }

        // Calculate how far along the line the currentVertex is
        // 计算当前顶点沿直线的距离
        if (prevCoordinate)
            geoDistance += util::dist<double>(*currentCoordinate, *prevCoordinate);

        if (hasPrevNextVertex && currentJoin == LineJoinType::Miter) {
            joinNormal = joinNormal * miterLength;
            addCurrentVertex(*currentCoordinate, heightRatio, geoDistance, joinNormal, 0, 0, false, 
                             startVertex, triangleStore, lineDistances);
        } else if (hasPrevNextVertex && currentJoin == LineJoinType::FlipBevel) {
            // miter is too big, flip the direction to make a beveled join
            // 斜接太大，翻转方向以形成斜面连接
            if (miterLength > 100) {
                // Almost parallel lines 几乎平行的线
                joinNormal = *nextNormal * -1.0;
            } else {
                const double direction = prevNormal->x * nextNormal->y - prevNormal->y * nextNormal->x > 0 ? -1 : 1;
                const double bevelLength = miterLength * util::mag(*prevNormal + *nextNormal) / util::mag(*prevNormal - *nextNormal);
                joinNormal = util::perp(joinNormal) * bevelLength * direction;
            }

            addCurrentVertex(*currentCoordinate, heightRatio, geoDistance, joinNormal, 0, 0, false, 
                             startVertex, triangleStore, lineDistances);

            addCurrentVertex(*currentCoordinate, heightRatio, geoDistance, joinNormal * -1.0, 0, 0, false, 
                             startVertex, triangleStore, lineDistances);
        } else if (hasPrevNextVertex && (currentJoin == LineJoinType::Bevel || currentJoin == LineJoinType::FakeRound)) {
            const bool lineTurnsLeft = (prevNormal->x * nextNormal->y - prevNormal->y * nextNormal->x) > 0;
            const float offset = -std::sqrt(miterLength * miterLength - 1);
            float offsetA;
            float offsetB;

            if (lineTurnsLeft) {
                offsetB = 0;
                offsetA = offset;
            } else {
                offsetA = 0;
                offsetB = offset;
            }

            // Close previous segement with bevel 用斜面封闭上一段
            if (!startOfLine) {
                addCurrentVertex(*currentCoordinate, heightRatio, geoDistance, *prevNormal, offsetA, offsetB, false,
                                 startVertex, triangleStore, lineDistances);
            }

            if (currentJoin == LineJoinType::FakeRound) {
                // The join angle is sharp enough that a round join would be visible.
                // Bevel joins fill the gap between segments with a single pie slice triangle.
                // Create a round join by adding multiple pie slices. The join isn't actually round, but
                // it looks like it is at the sizes we render lines at.
                // 连接角度足够锋利，可以看到圆形连接。斜面连接用单个饼状切片三角形填充分段之间的间隙。
                // 通过添加多个饼图切片创建圆形连接。连接实际上不是圆的，但是它看起来就像我们渲染线条的大小。

                // Pick the number of triangles for approximating round join by based on the angle between normals.
                // 根据法线之间的角度，选择用于近似圆形连接的三角形数量。
                const unsigned n = ::round((approxAngle * 180 / M_PI) / DEG_PER_TRIANGLE);

                for (unsigned m = 1; m < n; ++m) {
                    double t = double(m) / n;
                    if (t != 0.5) {
                        // approximate spherical interpolation https://observablehq.com/@mourner/approximating-geometric-slerp
                        const double t2 = t - 0.5;
                        const double A = 1.0904 + cosAngle * (-3.2452 + cosAngle * (3.55645 - cosAngle * 1.43519));
                        const double B = 0.848013 + cosAngle * (-1.06021 + cosAngle * 0.215638);
                        t = t + t * t2 * (t - 1) * (A * t2 * t2 + B);
                    }
                    auto approxFractionalNormal = util::unit(*prevNormal * (1.0 - t) + *nextNormal * t);
                    addPieSliceVertex(*currentCoordinate, geoDistance, approxFractionalNormal, lineTurnsLeft, startVertex, triangleStore, lineDistances);
                }
            }

            // Start next segment
            if (nextCoordinate) {
                addCurrentVertex(*currentCoordinate, heightRatio, geoDistance, *nextNormal, -offsetA, -offsetB,
                                 false, startVertex, triangleStore, lineDistances);
            }

        } else if (!hasPrevNextVertex && currentCap == LineCapType::Butt) {
            if (!startOfLine) {
                // Close previous segment with a butt
                // 用butt关闭上一段
                addCurrentVertex(*currentCoordinate, heightRatio, geoDistance, *prevNormal, 0, 0, false,
                                 startVertex, triangleStore, lineDistances);
            }

            // Start next segment with a butt
            // 用butt开始下一段
            if (nextCoordinate) {
                addCurrentVertex(*currentCoordinate, heightRatio, geoDistance, *nextNormal, 0, 0, false,
                                 startVertex, triangleStore, lineDistances);
            }

        } else if (!hasPrevNextVertex && currentCap == LineCapType::Square) {
            if (!startOfLine) {
                // Close previous segment with a square cap
                // 用square关闭上一段
                addCurrentVertex(*currentCoordinate, heightRatio, geoDistance, *prevNormal, 1, 1, false,
                                 startVertex, triangleStore, lineDistances);

                // The segment is done. Unset vertices to disconnect segments.
                e1 = e2 = -1;
            }

            // Start next segment
            if (nextCoordinate) {
                addCurrentVertex(*currentCoordinate, heightRatio, geoDistance, *nextNormal, -1, -1, false,
                                 startVertex, triangleStore, lineDistances);
            }

        } else if (hasPrevNextVertex ? currentJoin == LineJoinType::Round : currentCap == LineCapType::Round) {
            if (!startOfLine) {
                // Close previous segment with a butt
                // 用butt关闭上一段
                addCurrentVertex(*currentCoordinate, heightRatio, geoDistance, *prevNormal, 0, 0, false,
                                 startVertex, triangleStore, lineDistances);

                // Add round cap or linejoin at end of segment
                // 在线段末端添加圆形帽或线条连接
                addCurrentVertex(*currentCoordinate, heightRatio, geoDistance, *prevNormal, 1, 1, true, startVertex,
                                 triangleStore, lineDistances);

                // The segment is done. Unset vertices to disconnect segments.
                e1 = e2 = -1;
            }

            // Start next segment with a butt
            if (nextCoordinate) {
                // Add round cap before first segment
                // 在第一段之前添加圆形盖子
                addCurrentVertex(*currentCoordinate, heightRatio, geoDistance, *nextNormal, -1, -1, true,
                                 startVertex, triangleStore, lineDistances);

                addCurrentVertex(*currentCoordinate, heightRatio, geoDistance, *nextNormal, 0, 0, false,
                                 startVertex, triangleStore, lineDistances);
            }
        }

        // 处理转折角度过大尖角点，填补一个新pre坐标点
        if (isSharpCorner && i < len - 1) {
            const auto nextSegmentLength = util::dist<double>(*currentCoordinate, *nextCoordinate);
            if (nextSegmentLength > 2 * sharpCornerOffset) {
                // 伪造新的中间点
                GeometryCoordinate newCurrentVertex = *currentCoordinate +
                convertPoint<int16_t>(util::round(convertPoint<double>(*nextCoordinate - *currentCoordinate) * (sharpCornerOffset / nextSegmentLength)));
                geoDistance += util::dist<double>(newCurrentVertex, *currentCoordinate);
                addCurrentVertex(newCurrentVertex, heightRatio, geoDistance, *nextNormal, 0, 0, false,
                                 startVertex, triangleStore, lineDistances);
                
                currentCoordinate = newCurrentVertex;
            }
        }

        startOfLine = false;
    }

    const std::size_t endVertex = vertices.elements();
    const std::size_t vertexCount = endVertex - startVertex;

    if (segments.empty() || segments.back().vertexLength + vertexCount > std::numeric_limits<uint16_t>::max()) {
        segments.emplace_back(startVertex, triangles.elements());
    }

    auto& segment = segments.back();
    assert(segment.vertexLength <= std::numeric_limits<uint16_t>::max());
    uint16_t index = segment.vertexLength;

    for (const auto& triangle : triangleStore) {
        triangles.emplace_back(index + triangle.a, index + triangle.b, index + triangle.c);
    }

    segment.vertexLength += vertexCount;
    segment.indexLength += triangleStore.size() * 3;
}

void LineBucket::addCurrentVertex(const GeometryCoordinate& currentCoordinate,
                                  float heightRatio,
                                  double &distance,
                                  const Point<double>& normal,
                                  double endLeft,
                                  double endRight,
                                  bool round,
                                  std::size_t startVertex,
                                  std::vector<TriangleElement>& triangleStore,
                                  optional<Distances> lineDistances) {
    Point<double> extrude = normal;
    const double scaledDistance = lineDistances ? lineDistances->scaleToMaxLineDistance(distance) : distance;
    const float height = heightRatio * _nav_layer_base_height;
    int16_t condition = _nav_enable_traffic_condition ? int(distance / 1000.) % 5 : 0;

    if (endLeft)
        extrude = extrude - (util::perp(normal) * endLeft);
    vertices.emplace_back(LineProgram::layoutVertex(currentCoordinate, height, condition,
                                                    extrude, round, false, endLeft,
                                                    scaledDistance * LINE_DISTANCE_SCALE));
    e3 = vertices.elements() - 1 - startVertex;
    if (e1 >= 0 && e2 >= 0) {
        triangleStore.emplace_back(e1, e2, e3);
    }
    e1 = e2;
    e2 = e3;

    extrude = normal * -1.0;
    if (endRight)
        extrude = extrude - (util::perp(normal) * endRight);
    vertices.emplace_back(LineProgram::layoutVertex(currentCoordinate, height, condition,
                                                    extrude, round, true, -endRight,
                                                    scaledDistance * LINE_DISTANCE_SCALE));
    e3 = vertices.elements() - 1 - startVertex;
    if (e1 >= 0 && e2 >= 0) {
        triangleStore.emplace_back(e1, e2, e3);
    }
    e1 = e2;
    e2 = e3;

    // There is a maximum "distance along the line" that we can store in the buffers.
    // When we get close to the distance, reset it to zero and add the vertex again with
    // a distance of zero. The max distance is determined by the number of bits we allocate
    // to `linesofar`.
    if (distance > MAX_LINE_DISTANCE / 2.0f && !lineDistances) {
        distance = 0.0;
        addCurrentVertex(currentCoordinate, height, distance, normal, endLeft, endRight,
                         round, startVertex, triangleStore, lineDistances);
    }
}

void LineBucket::addPieSliceVertex(const GeometryCoordinate& currentVertex,
                                   double distance,
                                   const Point<double>& extrude,
                                   bool lineTurnsLeft,
                                   std::size_t startVertex,
                                   std::vector<TriangleElement>& triangleStore,
                                   optional<Distances> lineDistances) {
    Point<double> flippedExtrude = extrude * (lineTurnsLeft ? -1.0 : 1.0);
    if (lineDistances) {
        distance = lineDistances->scaleToMaxLineDistance(distance);
    }

    int16_t condition = _nav_enable_traffic_condition ? int(distance / 1000.) % 5 : 0;
    vertices.emplace_back(LineProgram::layoutVertex(currentVertex, 0, condition,
                                                    flippedExtrude, false, lineTurnsLeft, 0,
                                                    distance * LINE_DISTANCE_SCALE));
    e3 = vertices.elements() - 1 - startVertex;
    if (e1 >= 0 && e2 >= 0) {
        triangleStore.emplace_back(e1, e2, e3);
    }

    if (lineTurnsLeft) {
        e2 = e3;
    } else {
        e1 = e3;
    }
}

void LineBucket::upload(gfx::UploadPass& uploadPass) {
    if (!uploaded) {
        vertexBuffer = uploadPass.createVertexBuffer(std::move(vertices));
        indexBuffer = uploadPass.createIndexBuffer(std::move(triangles));
    }

    for (auto& pair : paintPropertyBinders) {
        pair.second.upload(uploadPass);
    }

    uploaded = true;
}

bool LineBucket::hasData() const {
    return !segments.empty();
}

template <class Property>
static float get(const LinePaintProperties::PossiblyEvaluated& evaluated, const nav::stringid& id, 
                 const std::unordered_map<nav::stringid, LineProgram::Binders>& paintPropertyBinders) {
    auto it = paintPropertyBinders.find(id);
    if (it == paintPropertyBinders.end() || !it->second.statistics<Property>().max()) {
        return evaluated.get<Property>().constantOr(Property::defaultValue());
    } else {
        return *it->second.statistics<Property>().max();
    }
}

float LineBucket::getQueryRadius(const RenderLayer& layer) const {
    const auto& evaluated = getEvaluated<LineLayerProperties>(layer.evaluatedProperties);
    const std::array<float, 2>& translate = evaluated.get<LineTranslate>();
    float offset = get<LineOffset>(evaluated, layer.getID(), paintPropertyBinders);
    float lineWidth = get<LineWidth>(evaluated, layer.getID(), paintPropertyBinders);
    float gapWidth = get<LineGapWidth>(evaluated, layer.getID(), paintPropertyBinders);
    if (gapWidth) {
        lineWidth = gapWidth + 2 * lineWidth;
    }

    return lineWidth / 2.0f + std::abs(offset) + util::length(translate[0], translate[1]);
}

void LineBucket::update(const FeatureStates& states, const GeometryTileLayer& layer, const nav::stringid& layerID,
                        const ImagePositions& imagePositions) {
    auto it = paintPropertyBinders.find(layerID);
    if (it != paintPropertyBinders.end()) {
        it->second.updateVertexVectors(states, layer, imagePositions);
        uploaded = false;
    }
}

} // namespace mbgl
