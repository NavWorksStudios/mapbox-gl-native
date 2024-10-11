//
//  nav.style.cpp
//
//  Created by BernieZhao on 2023/12/13.
//

#include "mbgl/nav/nav.style.hpp"
#include "mbgl/nav/nav.theme.hpp"


namespace nav {

const std::string& accessToken() {
    static std::string token = "pk.eyJ1IjoibmF2d29ya3MiLCJhIjoiY2x5M3U3MG96MDF5ZjJsb2lsMXZsczUxayJ9.Mnts5N9BU9OBsRb8fHauBQ";
    return token;
}

const std::string& assetsPath() {
    static const std::string path { NAV_ASSETS_PATH };
    return path;
}

std::string path(const char* file) {
    return assetsPath() + file;
}

namespace display {

namespace logic {

int _width = 1920;
int _height = 1080;

int width() {
    return _width;
}

int height() {
    return _height;
};

void set(int width, int height) {
    _width = width;
    _height = height;
}

}

namespace pixels {

float _ratio = 1.;

int width() {
    return logic::_width * _ratio;
}

int height() {
    return logic::_height * _ratio;
}

void setRatio(float ratio) {
    _ratio = ratio;
}

}

float clipping = 0.;

void update(float zoom) {
    static float region = logic::height() * atan(70 / M_PI) * .3;
    clipping = region * pow(2., zoom);
}

float clip_region() {
    return clipping;
}

float focus_region() {
    static float region = pow(logic::width(), 2) + pow(logic::height(), 2);
    return region;
}

}

namespace style {

const std::string& url() {
    return theme::style();
}

std::string parsing_domain;

Domain::Domain(const std::string& name) : pos(parsing_domain.size()) {
    parsing_domain.append("/");
    parsing_domain.append(name);
}

Domain::~Domain() {
    parsing_domain.resize(pos);
}

Domain::operator const std::string& () const {
    return parsing_domain;
}

}


}
