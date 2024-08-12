#pragma once

#include <mbgl/util/geometry.hpp>
#include <mbgl/util/variant.hpp>
#include <mbgl/util/color.hpp>
#include <mbgl/style/property_value.hpp>

#include <cstdint>
#include <vector>
#include <string>

#include "mbgl/nav/nav_stringid.hpp"

namespace mbgl {

class LineRoutePlanLink {   // #*# 同一路况的路况的连续点
public:
    LineRoutePlanLink(int16_t condition_ = 0) {
        condition = condition_;
    }
    LineString<int64_t> points;
    int16_t condition;  // condition index: -3 -> +3, 0 is normal condition
    void addPoint(mbgl::Point<int64_t>& point_) {
        points.emplace_back(point_);
    }
};

class LineRoutePlanSegment {    // #*# 同一tile下的一段连续点
public:
    std::vector<LineRoutePlanLink> links;  // every link is needed for diff condition
    LineString<int64_t> points;
    std::vector<int16_t> conditions;
public:
    void addPoint(mbgl::Point<int64_t>& point_, int16_t condition_ = 0) {
        points.emplace_back(point_);
        conditions.emplace_back(condition_);
        
        if(links.size() == 0) {
            LineRoutePlanLink link(condition_);
            link.addPoint(point_);
            links.emplace_back(link);
        }
        else {
            LineRoutePlanLink& link = links.back();
            if(link.condition == condition_)
                link.addPoint(point_);
            else {
                link.addPoint(point_);
                LineRoutePlanLink link_new(condition_);
                link_new.addPoint(point_);
                links.emplace_back(link_new);
            }
        }
    }
};

class LineRoutePlanTile {   // #*# tile内的全部连续点信息
public:
    LineRoutePlanTile(const CanonicalTileID& tileID_)
        : tileID(tileID_) {}
        
    CanonicalTileID tileID;
    std::vector<LineRoutePlanSegment> segments; // every segment is independent, but, they are orderly
public:
    void addPoint(mbgl::Point<int64_t>& point_, int16_t condition_ = 0, bool new_segment_ = false) {
        if(new_segment_ || segments.size() == 0) {
            LineRoutePlanSegment* segment = new LineRoutePlanSegment();
            segment->addPoint(point_, condition_);
            segments.emplace_back(*segment);
        }
        else {
            LineRoutePlanSegment& segment = segments.back();// segments[segments.size()-1];
            segment.addPoint(point_, condition_);
        }
    }
};

} // namespace mbgl
