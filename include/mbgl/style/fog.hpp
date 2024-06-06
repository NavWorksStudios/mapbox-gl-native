// clang-format off

// This file is generated. Do not edit.

#pragma once

#include <mbgl/style/conversion.hpp>
#include <mbgl/style/property_value.hpp>
#include <mbgl/style/style_property.hpp>
#include <mbgl/style/transition_options.hpp>
#include <mbgl/style/types.hpp>
#include <mbgl/util/immutable.hpp>

namespace mbgl {
namespace style {

class FogObserver;

class Fog {
public:
    Fog();
    ~Fog();

    // Dynamic properties
    optional<conversion::Error> setProperty(const std::string& name, const conversion::Convertible& value);
    StyleProperty getProperty(const std::string&) const;
    
    static std::array<float, 2> getDefaultRange();
    PropertyValue<std::array<float, 2>> getRange() const;
    void setRange(const PropertyValue<std::array<float, 2>>&);
    void setRangeTransition(const TransitionOptions&);
    TransitionOptions getRangeTransition() const;

    static Color getDefaultColor();
    PropertyValue<Color> getColor() const;
    void setColor(PropertyValue<Color>);
    void setColorTransition(const TransitionOptions&);
    TransitionOptions getColorTransition() const;
    
    static Color getDefaultHighColor();
    PropertyValue<Color> getHighColor() const;
    void setHighColor(PropertyValue<Color>);
    void setHighColorTransition(const TransitionOptions&);
    TransitionOptions getHighColorTransition() const;
    
    static Color getDefaultSpaceColor();
    PropertyValue<Color> getSpaceColor() const;
    void setSpaceColor(PropertyValue<Color>);
    void setSpaceColorTransition(const TransitionOptions&);
    TransitionOptions getSpaceColorTransition() const;

    static float getDefaultHorizonBlend();
    PropertyValue<float> getHorizonBlend() const;
    void setHorizonBlend(PropertyValue<float>);
    void setHorizonBlendTransition(const TransitionOptions&);
    TransitionOptions getHorizonBlendTransition() const;
    
    static float getDefaultStarIntensity();
    PropertyValue<float> getStarIntensity() const;
    void setStarIntensity(PropertyValue<float>);
    void setStarIntensityTransition(const TransitionOptions&);
    TransitionOptions getStarIntensityTransition() const;

    class Impl;
    Immutable<Impl> impl;
    explicit Fog(Immutable<Impl>);
    Mutable<Impl> mutableImpl() const;

    FogObserver* observer = nullptr;
    void setObserver(FogObserver*);
};

} // namespace style
} // namespace mbgl

// clang-format on
