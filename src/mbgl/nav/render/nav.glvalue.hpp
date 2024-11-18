//
//  nav.runtime.hpp
//
//  Created by BernieZhao on 2024/10/11.
//

#pragma once

#include <mbgl/gl/value.hpp>

using namespace mbgl::gl::value;
using namespace mbgl::gfx;

namespace nav {
namespace gl {

template <typename T> struct Value {
    typename T::Type v = T::Default;
    Value() { v = T::Get(); }
    ~Value() { restore(); }
    void restore() { T::Set(v); }
    void set(const T::Type& v) { T::Set(v); }
    operator typename T::Type () { return v; }
    operator const typename T::Type () const { return v; }
};

}
}



