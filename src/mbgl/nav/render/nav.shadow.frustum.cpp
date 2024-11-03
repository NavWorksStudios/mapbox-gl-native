//
//  nav.shadow.frustum.cpp
//
//  Created by BernieZhao on 2024/10/31.
//

#include "mbgl/nav/render/nav.shadow.frustum.hpp"
#include "mbgl/nav/nav.log.hpp"

#include <mbgl/util/mat4.hpp>
#include <mbgl/util/bounding_volumes.hpp>

#include <array>


namespace nav {

namespace shadow {

struct Plane {
    mbgl::vec3 normal;
    float distance;
};

struct Linesegment {
    mbgl::vec3 from;
    mbgl::vec3 to;
    
    mbgl::vec3 direction() const {
        return { to[0] - from[0], to[1] - from[1], to[2] - from[2] };
    }
};

mbgl::vec3 getIntersect(const Plane& plane, const Linesegment& line) {
    const auto dir = line.direction();
    const auto& nor = plane.normal;
    
    double denominator = nor[0] * dir[0] + nor[1] * dir[1] + nor[2] * dir[2];
    if (denominator == 0) {
        printf("getIntersect 线段与平面平行，无交点");
        assert(0);
    }
    
    mbgl::vec3 intersect;
    double t = -(nor[0] * line.from[0] + nor[1] * line.from[1] + nor[2] * line.from[2] + plane.distance) / denominator;
    if (t >= 0 && t <= 1) {
        intersect[0] = line.from[0] + t * dir[0];
        intersect[1] = line.from[1] + t * dir[1];
        intersect[2] = line.from[2] + t * dir[2];
    } else {
        printf("getIntersect 交点不在线段上");
        assert(0);
    }
    
    return intersect;
}


void print(const mbgl::vec3* projection) {
    printf("DynamicView : (%8.2lf, %8.2lf, %8.2lf) (%8.2lf, %8.2lf, %8.2lf) (%8.2lf, %8.2lf, %8.2lf) (%8.2lf, %8.2lf, %8.2lf) \n",
           projection[0][0], projection[0][1], projection[0][2],
           projection[1][0], projection[1][1], projection[1][2],
           projection[2][0], projection[2][1], projection[2][2],
           projection[3][0], projection[3][1], projection[3][2]);
};

void print(const char* name, const frustum::ortho::AABB& aabb) {
    nav::log::i("sunlight", "%s min(%8.2lf, %8.2lf, %8.2lf) max(%8.2lf, %8.2lf, %8.2lf)", name,
                aabb.min[0], aabb.min[1], aabb.min[2],
                aabb.max[0], aabb.max[1], aabb.max[2]);
}

namespace frustum {

namespace ortho {

void AABB::invalidate() {
    min = { NAN, };
    max = { NAN, };
}

bool AABB::valid() const {
    if (std::isnan(min[0]) ||
        std::isnan(min[1]) ||
        std::isnan(min[2]) ||
        std::isnan(max[0]) ||
        std::isnan(max[1]) ||
        std::isnan(max[2])) return false;

    return
    min[0] <= max[0] &&
    min[1] <= max[1] &&
    min[2] <= max[2];
}

void AABB::include(double x, double y, double z) {
    min[0] = std::isnan(min[0]) ? x : fmin(min[0], x);
    min[1] = std::isnan(min[1]) ? y : fmin(min[1], y);
    min[2] = std::isnan(min[2]) ? z : fmin(min[2], z);

    max[0] = std::isnan(max[0]) ? x : fmax(max[0], x);
    max[1] = std::isnan(max[1]) ? y : fmax(max[1], y);
    max[2] = std::isnan(max[2]) ? z : fmax(max[2], z);
};

AABB AABB::intersect(const AABB& aabb) const {
    AABB intersection;

    intersection.min = {
        fmax(min[0], aabb.min[0]),
        fmax(min[1], aabb.min[1]),
        fmax(min[2], aabb.min[2]),
    };
    
    intersection.max = {
        fmin(max[0], aabb.max[0]),
        fmin(max[1], aabb.max[1]),
        fmin(max[2], aabb.max[2]),
    };
    
    return intersection;
}


namespace height {

enum Format {
    H8192 = 5000,
    H512 = int(H8192 * mbgl::util::tileSize / mbgl::util::EXTENT),
};

template <Format H> float get(float zoom) {
#if 0
    const int MIN_Z = 15;
    return H * pow(2, fmax(0, fmin(zoom, 17.) - MIN_Z));
#else
    return H;
#endif
}

}

namespace area {

struct Point {
    double x, y;
};

// 计算两个向量的叉积
double crossProduct(const Point& a, const Point& b) {
    return a.x * b.y - a.y * b.x;
}

// 计算三角形面积
double triangleArea(const Point& a, const Point& b, const Point& c) {
    Point ab = {b.x - a.x, b.y - a.y};
    Point ac = {c.x - a.x, c.y - a.y};
    return std::abs(crossProduct(ab, ac)) / 2.0;
}

// 计算四边形面积
double quadrilateralArea(const Point& a, const Point& b, const Point& c, const Point& d) {
    return triangleArea(a, b, c) + triangleArea(a, c, d);
}

}


void Frumstum::update(const mbgl::TransformState& state, const std::vector<mbgl::OverscaledTileID>& tileIDs) {
    if (tileIDs.size() == 0) return;
    
    const auto& lightViewMatrix = state.getSunlightWorldToViewMatrix();
    
    // 可见楼块tile在光照空间的AABB
    tileAABB.invalidate();
    {
        // 计算最小外接
        for (const auto& tile : tileIDs) {
            // model pos
            const double h = height::get<height::H8192>(tile.canonical.z);
            const mbgl::vec4 envelope[8] = {
                { 0, 0, 0, 1 },
                { 0, mbgl::util::EXTENT, 0, 1 },
                { mbgl::util::EXTENT, 0, 0, 1 },
                { mbgl::util::EXTENT, mbgl::util::EXTENT, 0, 1 },

                { 0, 0, h, 1 },
                { 0, mbgl::util::EXTENT, h, 1 },
                { mbgl::util::EXTENT, 0, h, 1 },
                { mbgl::util::EXTENT, mbgl::util::EXTENT, h, 1 },
            };
            
            // model to world
            mbgl::mat4 modelmatrix;
            state.matrixFor(modelmatrix, tile.toUnwrapped());

            for (int i=0; i<8; i++) {
                mbgl::vec4 v = envelope[i];
                mbgl::matrix::transformMat4(v, v, modelmatrix); // to world space
                mbgl::matrix::transformMat4(v, v, lightViewMatrix); // to light view space
                tileAABB.include(v[0], v[1], -v[2]);
            }
        }

        print("tile", tileAABB);
    }
    
    // 可见地面在光照空间的AABB
    groundAABB.invalidate();
    {
        // 构造主相机视锥体
        const auto worldSize = mbgl::Projection::worldSize(state.getScale()) / mbgl::util::tileSize;
        const auto flippedY = state.getViewportMode() == mbgl::ViewportMode::FlippedY;
        const auto frustum = mbgl::util::Frustum::fromInvProjMatrix(state.getInvProjectionMatrix(), worldSize, state.getZoom(), flippedY);

        // 计算主相机视锥体与地面交点
        enum { near_tl = 0, near_tr = 1, near_br = 2, near_bl = 3,
               far_tl = 4, far_tr = 5, far_br = 6, far_bl = 7, };
        const auto& points = frustum.getPoints();
        const Plane ground = { { 0, 0, 1 }, 0 };
        
        //            camera
        //              A                                far
        //            near                 tl [0] ================== [1] tr
        //            /  |                        \                /
        //           /   |                       0 \______________/ 1
        //          /    |                          \            /
        //         /     |                           \__________/
        //     ___/======|_______              bl [3]            [2] br
        //  top  /       |  bottom                       near
        //          far
        
        mbgl::vec3 projection[4] = {
            getIntersect(ground, { points[near_tl], points[far_tl] }), // [0]
            getIntersect(ground, { points[near_tr], points[far_tr] }), // [1]
            getIntersect(ground, { points[near_br], points[far_br] }), // [2]
            getIntersect(ground, { points[near_bl], points[far_bl] }), // [3]
        };

        // 动态视野 (pitch, zoom)
        {
            const double zf = fmax(0., fmin(1., (state.getZoom() - 15.) / 4.)); // (0, 1) 15-20
            const double pf = fmin(state.getPitch() / M_PI * 180. / 70., 1.); // (0, 1) 俯视, 平视

            //       俯视                                       平视
            //  z\p |.0 |.1 |.2 |.3 |.4 |.5 |.6 |.7 |.8 |.9 |1. |
            // .0   |   |   |   |   |   |   |1. |   |   |   |.4 |
            //                                                  |
            //                                                  |
            //                                                  |
            //                   left                           | right
            //                                                  |
            //                                                  |
            //                                                  |
            // .1   |   |   |.5 |   |   |   |   |   |   |   |.05|
            // 近

            const double p[2] = { .6 - .4 * zf, 1. };
            const double r[2] = { 1. - .5 * zf, .4 - .35 * zf };
            const double result = r[0] + fmax(pf - p[0], 0.) / (p[1] - p[0]) * (r[1] - r[0]);
            
            static auto shrink = [] (mbgl::vec3& near, mbgl::vec3& far, float shrink) {
                far[0] = near[0] + (far[0] - near[0]) * shrink; // x
                far[1] = near[1] + (far[1] - near[1]) * shrink; // y
            };
            
            double area0 =
            area::quadrilateralArea({ projection[0][0],projection[0][1] },
                                    { projection[1][0],projection[1][1] },
                                    { projection[2][0],projection[2][1] },
                                    { projection[3][0],projection[3][1] });
            
            enum { tl = 0, tr = 1, br = 2, bl = 3, };
            shrink(projection[bl], projection[tl], result);
            shrink(projection[br], projection[tr], result);
            
            
            double area1 =
            area::quadrilateralArea({ projection[0][0],projection[0][1] },
                                    { projection[1][0],projection[1][1] },
                                    { projection[2][0],projection[2][1] },
                                    { projection[3][0],projection[3][1] });
            
            printf("I <sunlight> zoom(%lf) pitch(%lf) | z(%lf) p(%lf) r(%lf) | area(%lf,%lf)\n", state.getZoom(), state.getPitch(), zf, pf, result, area0, area1);
        }

        // 计算最小外接
        const double h = height::get<height::H512>(state.getZoom());
        for (int i=0; i<4; i++) {
            const auto& p = projection[i];
            mbgl::vec4 envelope[2] = {
                { p[0], p[1], 0, 1 },
                { p[0], p[1], h, 1 },
            };
            
            for (int i=0; i<2; i++) {
                auto& v = envelope[i];
                mbgl::matrix::transformMat4(v, v, lightViewMatrix); // to light view space
                groundAABB.include(v[0], v[1], -v[2]);
            }
        }

        print("proj", groundAABB);
    }
    
    // 求tileAABB和groundAABB交集，得frustumAABB
    {
        frustumAABB = tileAABB.intersect(groundAABB);
        print("frus", frustumAABB);
    }
    
    assert(tileAABB.valid());
    assert(groundAABB.valid());
    assert(frustumAABB.valid());
    
}

void Frumstum::render(std::function<void()> bindScreen) {
    
}

Frumstum& sunlight() {
    static Frumstum frustum;
    return frustum;
}

}   // end ortho

}   // end frustum

}   // end shadow

}   // end nav



