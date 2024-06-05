// This file is generated. Do not edit.

#include <mbgl/style/fog.hpp>
#include <mbgl/style/fog_impl.hpp>
#include <mbgl/style/fog_observer.hpp>
#include <mbgl/style/conversion/fog.hpp>
#include <mbgl/style/conversion/property_value.hpp>
#include <mbgl/style/conversion/transition_options.hpp>
#include <mbgl/style/conversion/json.hpp>
#include <mbgl/style/conversion_impl.hpp>
#include <mbgl/util/traits.hpp>

#include <mapbox/eternal.hpp>

#include <utility>

namespace mbgl {
namespace style {

static FogObserver nullObserver;

Fog::Fog(Immutable<Fog::Impl> impl_) : impl(std::move(impl_)), observer(&nullObserver) {}

Fog::Fog() : Fog(makeMutable<Impl>()) {}

Fog::~Fog() = default;

void Fog::setObserver(FogObserver* observer_) {
    observer = observer_ ? observer_ : &nullObserver;
}

Mutable<Fog::Impl> Fog::mutableImpl() const {
    return makeMutable<Impl>(*impl);
}

using namespace conversion;

namespace {

enum class Property : uint8_t {
    Range,
    Color,
    HighColor,
    SpaceColor,
    HorizonBlend,
    StarIntensity,
};

template <typename T>
constexpr uint8_t toUint8(T t) noexcept {
    return uint8_t(mbgl::underlying_type(t));
}

MAPBOX_ETERNAL_CONSTEXPR const auto properties = mapbox::eternal::hash_map<mapbox::eternal::string, uint8_t>(
    {{"range", toUint8(Property::Range)},
     {"color", toUint8(Property::Color)},
     {"high-color", toUint8(Property::HighColor)},
     {"space-color", toUint8(Property::SpaceColor)},
     {"horizon-blend", toUint8(Property::HorizonBlend)},
     {"star-intensity", toUint8(Property::StarIntensity)}});
} // namespace
                                                                                                             
optional<Error> Fog::setProperty(const std::string& name, const Convertible& value) {
    const auto it = properties.find(name.c_str());
    if (it == properties.end()) {
        return Error { "fog doesn't support this property" };
    }

    auto property = static_cast<Property>(it->second);
        
    if (property == Property::Range) {
        Error error;
        optional<PropertyValue<std::array<float, 2>>> typedValue = convert<PropertyValue<std::array<float, 2>>>(value, error, false, false);
        if (!typedValue) {
            return error;
        }
        
        setRange(*typedValue);
        return nullopt;
        
    }
    
    else if (property == Property::Color) {
        Error error;
        optional<PropertyValue<Color>> typedValue = convert<PropertyValue<Color>>(value, error, false, false);
        if (!typedValue) {
            return error;
        }
        
        setColor(*typedValue);
        return nullopt;
        
    }
    
    else if (property == Property::HighColor) {
        Error error;
        optional<PropertyValue<Color>> typedValue = convert<PropertyValue<Color>>(value, error, false, false);
        if (!typedValue) {
            return error;
        }
        
        setHighColor(*typedValue);
        return nullopt;
        
    }
    
    else if (property == Property::SpaceColor) {
        Error error;
        optional<PropertyValue<Color>> typedValue = convert<PropertyValue<Color>>(value, error, false, false);
        if (!typedValue) {
            return error;
        }
        
        setSpaceColor(*typedValue);
        return nullopt;
        
    }
    
    else if (property == Property::HorizonBlend) {
        Error error;
        optional<PropertyValue<float>> typedValue = convert<PropertyValue<float>>(value, error, false, false);
        if (!typedValue) {
            return error;
        }
        
        setHorizonBlend(*typedValue);
        return nullopt;
        
    }
    
    else if (property == Property::StarIntensity) {
        Error error;
        optional<PropertyValue<float>> typedValue = convert<PropertyValue<float>>(value, error, false, false);
        if (!typedValue) {
            return error;
        }
        
        setStarIntensity(*typedValue);
        return nullopt;
        
    }

    return Error { "fog doesn't support this property" };
}

StyleProperty Fog::getProperty(const std::string& name) const {
    const auto it = properties.find(name.c_str());
    if (it == properties.end()) {
        return {};
    }

    switch (static_cast<Property>(it->second)) {
        case Property::Range:
            return makeStyleProperty(getRange());
        case Property::Color:
            return makeStyleProperty(getColor());
        case Property::HighColor:
            return makeStyleProperty(getHighColor());
        case Property::SpaceColor:
            return makeStyleProperty(getSpaceColor());
        case Property::HorizonBlend:
            return makeStyleProperty(getHorizonBlend());
        case Property::StarIntensity:
            return makeStyleProperty(getStarIntensity());
    }
    return {};
}

// range

std::array<float, 2> Fog::getDefaultRange() {
    return FogRange::defaultValue();
}

PropertyValue<std::array<float, 2>> Fog::getRange() const {
    return impl->properties.template get<FogRange>().value;
}

void Fog::setRange(const PropertyValue<std::array<float, 2>>& property) {
    auto impl_ = mutableImpl();
    impl_->properties.template get<FogRange>().value = std::move(property);
    impl = std::move(impl_);
    observer->onFogChanged(*this);
}

void Fog::setRangeTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->properties.template get<FogRange>().options = options;
    impl = std::move(impl_);
    observer->onFogChanged(*this);
}

TransitionOptions Fog::getRangeTransition() const {
    return impl->properties.template get<FogRange>().options;
}

// color

Color Fog::getDefaultColor() {
    return FogColor::defaultValue();
}

PropertyValue<Color> Fog::getColor() const {
    return impl->properties.template get<FogColor>().value;
}

void Fog::setColor(PropertyValue<Color> property) {
    auto impl_ = mutableImpl();
    impl_->properties.template get<FogColor>().value = std::move(property);
    impl = std::move(impl_);
    observer->onFogChanged(*this);
}

void Fog::setColorTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->properties.template get<FogColor>().options = options;
    impl = std::move(impl_);
    observer->onFogChanged(*this);
}

TransitionOptions Fog::getColorTransition() const {
    return impl->properties.template get<FogColor>().options;
}

// high color

Color Fog::getDefaultHighColor() {
    return FogHighColor::defaultValue();
}

PropertyValue<Color> Fog::getHighColor() const {
    return impl->properties.template get<FogHighColor>().value;
}

void Fog::setHighColor(PropertyValue<Color> property) {
    auto impl_ = mutableImpl();
    impl_->properties.template get<FogHighColor>().value = std::move(property);
    impl = std::move(impl_);
    observer->onFogChanged(*this);
}

void Fog::setHighColorTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->properties.template get<FogHighColor>().options = options;
    impl = std::move(impl_);
    observer->onFogChanged(*this);
}

TransitionOptions Fog::getHighColorTransition() const {
    return impl->properties.template get<FogHighColor>().options;
}

// space color

Color Fog::getDefaultSpaceColor() {
    return FogSpaceColor::defaultValue();
}

PropertyValue<Color> Fog::getSpaceColor() const {
    return impl->properties.template get<FogSpaceColor>().value;
}

void Fog::setSpaceColor(PropertyValue<Color> property) {
    auto impl_ = mutableImpl();
    impl_->properties.template get<FogSpaceColor>().value = std::move(property);
    impl = std::move(impl_);
    observer->onFogChanged(*this);
}

void Fog::setSpaceColorTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->properties.template get<FogSpaceColor>().options = options;
    impl = std::move(impl_);
    observer->onFogChanged(*this);
}

TransitionOptions Fog::getSpaceColorTransition() const {
    return impl->properties.template get<FogSpaceColor>().options;
}

// horizon blend

float Fog::getDefaultHorizonBlend() {
    return FogHorizonBlend::defaultValue();
}

PropertyValue<float> Fog::getHorizonBlend() const {
    return impl->properties.template get<FogHorizonBlend>().value;
}

void Fog::setHorizonBlend(PropertyValue<float> property) {
    auto impl_ = mutableImpl();
    impl_->properties.template get<FogHorizonBlend>().value = std::move(property);
    impl = std::move(impl_);
    observer->onFogChanged(*this);
}

void Fog::setHorizonBlendTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->properties.template get<FogHorizonBlend>().options = options;
    impl = std::move(impl_);
    observer->onFogChanged(*this);
}

TransitionOptions Fog::getHorizonBlendTransition() const {
    return impl->properties.template get<FogHorizonBlend>().options;
}

// star intensity

float Fog::getDefaultStarIntensity() {
    return FogStarIntensity::defaultValue();
}

PropertyValue<float> Fog::getStarIntensity() const {
    return impl->properties.template get<FogStarIntensity>().value;
}

void Fog::setStarIntensity(PropertyValue<float> property) {
    auto impl_ = mutableImpl();
    impl_->properties.template get<FogStarIntensity>().value = std::move(property);
    impl = std::move(impl_);
    observer->onFogChanged(*this);
}

void Fog::setStarIntensityTransition(const TransitionOptions& options) {
    auto impl_ = mutableImpl();
    impl_->properties.template get<FogStarIntensity>().options = options;
    impl = std::move(impl_);
    observer->onFogChanged(*this);
}

TransitionOptions Fog::getStarIntensityTransition() const {
    return impl->properties.template get<FogStarIntensity>().options;
}


} // namespace style
} // namespace mbgl
