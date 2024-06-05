#pragma once

#include <mbgl/style/fog.hpp>
#include <mbgl/style/property_value.hpp>
#include <mbgl/style/types.hpp>
#include <mbgl/style/position.hpp>
#include <mbgl/style/properties.hpp>
#include <mbgl/renderer/property_evaluator.hpp>
#include <mbgl/util/color.hpp>
#include <mbgl/util/indexed_tuple.hpp>

namespace mbgl {
namespace style {

template <class T>
class FogProperty {
public:
    using TransitionableType = Transitionable<PropertyValue<T>>;
    using UnevaluatedType = Transitioning<PropertyValue<T>>;
    using EvaluatorType = PropertyEvaluator<T>;
    using PossiblyEvaluatedType = T;
    using Type = T;
    static constexpr bool IsDataDriven = false;
    static constexpr bool IsOverridable = false;
};

struct FogRange : FogProperty<std::array<float, 2>> {
    static std::array<float, 2> defaultValue() {
        return { -1, 10 };
    }
};

struct FogColor : FogProperty<Color> {
    static Color defaultValue() {
        return Color::white();
    }
};

struct FogHighColor : FogProperty<Color> {
    static Color defaultValue() {
        return Color::white();
    }
};

struct FogSpaceColor : FogProperty<Color> {
    static Color defaultValue() {
        return Color::white();
    }
};

struct FogHorizonBlend : FogProperty<float> {
    static float defaultValue() {
        return 0.1;
    }
};

struct FogStarIntensity : FogProperty<float> {
    static float defaultValue() {
        return 0.15;
    }
};

using FogProperties = Properties<FogRange, FogColor, FogHighColor, FogSpaceColor, FogHorizonBlend, FogStarIntensity>;

class Fog::Impl {
public:
    FogProperties::Transitionable properties;
};

} // namespace style
} // namespace mbgl
