#pragma once

#include <mbgl/style/expression/expression.hpp>
#include <mbgl/style/expression/is_constant.hpp>
#include <mbgl/style/expression/interpolate.hpp>
#include <mbgl/style/expression/step.hpp>
#include <mbgl/style/expression/find_zoom_curve.hpp>
#include <mbgl/util/range.hpp>

#include <mbgl/util/variant.hpp>
#include <mbgl/style/undefined.hpp>
#include "mbgl/nav/nav_mb_style.hpp"


namespace mbgl {
namespace style {

class PropertyExpressionBase {
public:
    explicit PropertyExpressionBase(std::unique_ptr<expression::Expression>);

    bool isZoomConstant() const noexcept;
    bool isPitchConstant() const noexcept;
    bool isFeatureConstant() const noexcept;
    bool isRuntimeConstant() const noexcept;
    float interpolationFactor(const Range<float>&, float) const noexcept;
    Range<float> getCoveringStops(float, float) const noexcept;
    const expression::Expression& getExpression() const noexcept;

    // Can be used for aggregating property expressions from multiple
    // properties(layers) into single match / case expression. Method may
    // be removed if a better way of aggregation is found.
    std::shared_ptr<const expression::Expression> getSharedExpression() const noexcept;

    bool useIntegerZoom = false;

protected:
    std::shared_ptr<const expression::Expression> expression;
    variant<std::nullptr_t, const expression::Interpolate*, const expression::Step*> zoomCurve;
    bool isZoomConstant_;
    bool isPitchConstant_;
    bool isFeatureConstant_;
    bool isRuntimeConstant_;
};


template <typename T> struct PaleteBinding;

template <class T>
class PropertyExpression final : public PropertyExpressionBase {
public:
    // Second parameter to be used only for conversions from legacy functions.
    PropertyExpression(std::unique_ptr<expression::Expression> expression_, optional<T> defaultValue_ = nullopt)
        : PropertyExpressionBase(std::move(expression_)), defaultValue(std::move(defaultValue_)) {
        paletteBinding.bind(defaultValue);
    }

    T evaluate(const expression::EvaluationContext& context, T finalDefaultValue = T()) const {
        const expression::EvaluationResult result = expression->evaluate(context);
        if (result) {
            const optional<T> typed = expression::fromExpressionValue<T>(*result);
            return typed ? *typed : defaultValue ? *defaultValue : finalDefaultValue;
        }
        return defaultValue ? *defaultValue : finalDefaultValue;
    }

    T evaluateZoom(float zoom) const {
        assert(!isZoomConstant());
        assert(isFeatureConstant());
        
        expression::EvaluationContext context;
        context.withZoom(zoom);
        const auto& e = context;
        
        return evaluate(e);
    }
    
    T evaluatePitch(float pitch) const {
        assert(!isPitchConstant());
        assert(isFeatureConstant());
        
        expression::EvaluationContext context;
        context.withPitch(pitch);
        const auto& e = context;
        
        return evaluate(e);
    }

    T evaluate(const GeometryTileFeature& feature, T finalDefaultValue) const {
        assert(isZoomConstant());
        assert(!isFeatureConstant());
        
        expression::EvaluationContext context;
        context.withGeometryTileFeature(&feature);
        const auto& e = context;
        
        return evaluate(e, finalDefaultValue);
    }

    T evaluate(const GeometryTileFeature& feature, const std::set<std::string>& availableImages, T finalDefaultValue) const {
        expression::EvaluationContext context;
        context.withGeometryTileFeature(&feature).withAvailableImages(&availableImages);
        const auto& e = context;

        return evaluate(e, finalDefaultValue);
    }

    T evaluate(const GeometryTileFeature& feature, const CanonicalTileID& canonical, T finalDefaultValue) const {
        expression::EvaluationContext context;
        context.withGeometryTileFeature(&feature).withCanonicalTileID(&canonical);
        const auto& e = context;

        return evaluate(e, finalDefaultValue);
    }

    T evaluate(const GeometryTileFeature& feature, const std::set<std::string>& availableImages,
               const CanonicalTileID& canonical, T finalDefaultValue) const {
        expression::EvaluationContext context;
        context.withGeometryTileFeature(&feature).withAvailableImages(&availableImages).withCanonicalTileID(&canonical);
        const auto& e = context;

        return evaluate(e, finalDefaultValue);
    }

    T evaluate(float zoom, const GeometryTileFeature& feature, T finalDefaultValue) const {
        expression::EvaluationContext context;
        context.withZoom(zoom).withGeometryTileFeature(&feature);
        const auto& e = context;
        
        return evaluate(e, finalDefaultValue);
    }

    T evaluate(float zoom, const GeometryTileFeature& feature, const std::set<std::string>& availableImages, T finalDefaultValue) const {
        expression::EvaluationContext context;
        context.withZoom(zoom).withGeometryTileFeature(&feature).withAvailableImages(&availableImages);
        const auto& e = context;
        
        return evaluate(e, finalDefaultValue);
    }

    T evaluate(float zoom, const GeometryTileFeature& feature, const std::set<std::string>& availableImages,
               const CanonicalTileID& canonical, T finalDefaultValue) const {
        expression::EvaluationContext context;
        context.withZoom(zoom).withGeometryTileFeature(&feature).withAvailableImages(&availableImages).withCanonicalTileID(&canonical);
        const auto& e = context;
        
        return evaluate(e, finalDefaultValue);
    }

    T evaluate(float zoom, const GeometryTileFeature& feature, const CanonicalTileID& canonical, T finalDefaultValue) const {
        expression::EvaluationContext context;
        context.withZoom(zoom).withGeometryTileFeature(&feature).withCanonicalTileID(&canonical);
        const auto& e = context;
        
        return evaluate(e, finalDefaultValue);
    }

    T evaluate(float zoom, const GeometryTileFeature& feature, const FeatureState& state, T finalDefaultValue) const {
        assert(!isFeatureConstant());
        expression::EvaluationContext context;
        context.withZoom(zoom).withGeometryTileFeature(&feature).withFeatureState(&state);
        const auto& e = context;
        
        return evaluate(e, finalDefaultValue);
    }

    std::vector<optional<T>> possibleOutputs() const {
        return expression::fromExpressionValues<T>(expression->possibleOutputs());
    }

    friend bool operator==(const PropertyExpression& lhs, const PropertyExpression& rhs) {
        return *lhs.expression == *rhs.expression;
    }

private:
    optional<T> defaultValue;
    PaleteBinding<T> paletteBinding;
};

template <typename T> struct PaleteBinding {
    using Variant = variant<Undefined, T, PropertyExpression<T>>;
    void bind(optional<T>&) { }
    void bind(Variant&) { }
};

template <> struct PaleteBinding<mbgl::Color> {
    using Variant = variant<Undefined, mbgl::Color, PropertyExpression<mbgl::Color>>;

    void bind(optional<mbgl::Color>& value) {
        if (value)
        nav::style::palette::bind(value.value(), [&value](const mbgl::Color& color) {
            value = color;
        });
    }
    
    void bind(Variant& variant) {
        nav::style::palette::bind(variant.get<mbgl::Color>(), [&variant](const mbgl::Color& color) {
            variant = color;
        });
    }
};


} // namespace style
} // namespace mbgl
