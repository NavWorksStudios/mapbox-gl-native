#pragma once

#include <mbgl/style/fog.hpp>
#include <mbgl/style/conversion.hpp>
#include <mbgl/util/optional.hpp>

namespace mbgl {
namespace style {
namespace conversion {

template <>
struct Converter<Fog> {
public:
    optional<Fog> operator()(const Convertible& value, Error& error) const;
};

} // namespace conversion
} // namespace style
} // namespace mbgl
