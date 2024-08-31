// clang-format off

// This file is generated. Do not edit.

#pragma once

#include <mbgl/util/geo.hpp>

namespace mbgl {

//namespace route {

const int16_t eRoadCondition_MostFast = -3;
const int16_t eRoadCondition_VeryFast = -2;
const int16_t eRoadCondition_Fast = 0;
const int16_t eRoadCondition_Normal = 0;
const int16_t eRoadCondition_Congested = 1;
const int16_t eRoadCondition_VeryCongested = 2;
const int16_t eRoadCondition_MostCongested = 3;

struct link {
    
    std::vector<const mbgl::LatLng> points;
    
    struct traffic {
        int16_t percent;
        int16_t condition;
    };
    
    std::vector<traffic> traffics;
};

struct segment {
    
    std::vector<link> links;
};

struct path {
    
    std::vector<segment> segments;
    
};

struct route {
    
    std::vector<path> paths;
    
};

void set(std::shared_ptr<route>);

void clear();

std::shared_ptr<route> get();

//}

} // namespace mbgl

// clang-format on
