#include <mbgl/style/conversion/fog.hpp>
#include <mbgl/style/conversion/position.hpp>
#include <mbgl/style/conversion/property_value.hpp>
#include <mbgl/style/conversion/transition_options.hpp>
#include <mbgl/style/conversion_impl.hpp>

namespace mbgl {
namespace style {
namespace conversion {

optional<Fog> Converter<Fog>::operator()(const Convertible& value, Error& error) const {
    if (!isObject(value)) {
        error.message = "fog must be an object";
        return nullopt;
    }

    Fog fog;

    const auto range = objectMember(value, "range");
    if (range) {
        optional<std::array<float, 2>> converted =
            convert<std::array<float, 2>>(*range, error);
        if (converted) {
            fog.setRange(*converted);
        } else {
            return nullopt;
        }
    }

    const auto color = objectMember(value, "color");
    if (color) {
        optional<Color> converted =
            convert<Color>(*color, error);
        if (converted) {
            fog.setColor(*converted);
        } else {
            return nullopt;
        }
    }
    
    const auto highcolor = objectMember(value, "high-color");
    if (highcolor) {
        optional<Color> converted =
            convert<Color>(*highcolor, error);
        if (converted) {
            fog.setHighColor(*converted);
        } else {
            return nullopt;
        }
    }
    
    const auto spacecolor = objectMember(value, "space-color");
    if (spacecolor) {
        optional<Color> converted =
            convert<Color>(*spacecolor, error);
        if (converted) {
            fog.setSpaceColor(*converted);
        } else {
            return nullopt;
        }
    }

    const auto horizonblend = objectMember(value, "horizon-blend");
    if (horizonblend) {
        optional<float> converted =
            convert<float>(*horizonblend, error);
        if (converted) {
            fog.setHorizonBlend(*converted);
        } else {
            return nullopt;
        }
    }

    const auto starintensity = objectMember(value, "star-intensity");
    if (starintensity) {
        optional<float> converted =
            convert<float>(*starintensity, error);
        if (converted) {
            fog.setStarIntensity(*converted);
        } else {
            return nullopt;
        }
    }

    return { std::move(fog) };
}

} // namespace conversion
} // namespace style
} // namespace mbgl
