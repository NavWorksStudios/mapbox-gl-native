#include <mbgl/style/expression/compound_expression.hpp>
#include <mbgl/style/expression/expression.hpp>
#include <mbgl/tile/geometry_tile_data.hpp>
#include <utility>

namespace mbgl {
namespace style {
namespace expression {

class GeoJSONFeature : public GeometryTileFeature {
public:
    const Feature& feature;
    mutable optional<GeometryCollection> geometry;

    explicit GeoJSONFeature(const Feature& feature_) : feature(feature_) {}
    GeoJSONFeature(const Feature& feature_, const CanonicalTileID& canonical) : feature(feature_) {
        geometry = convertGeometry(feature.geometry, canonical);
        // https://github.com/mapbox/geojson-vt-cpp/issues/44
        if (getTypeImpl() == FeatureType::Polygon) {
            geometry = fixupPolygons(*geometry);
        }
    }

    FeatureType getType() const override { return getTypeImpl(); }

    const PropertyMap& getProperties() const override { return feature.properties; }
    FeatureIdentifier getID() const override { return feature.id; }
    optional<mbgl::Value> getValue(const std::string& key) const override {
        auto it = feature.properties.find(key);
        if (it != feature.properties.end()) {
            return optional<mbgl::Value>(it->second);
        }
        return optional<mbgl::Value>();
    }
    const GeometryCollection& getGeometries() const override {
        if (geometry) return *geometry;
        geometry = GeometryCollection();
        return *geometry;
    }

private:
    FeatureType getTypeImpl() const { return apply_visitor(ToFeatureType(), feature.geometry); }
};

EvaluationResult Expression::evaluate(optional<float> zoom,
                                      const Feature& feature,
                                      optional<double> colorRampParameter) const {
    GeoJSONFeature f(feature);

    style::expression::EvaluationContext context;
    context.withZoom(std::move(zoom)).withGeometryTileFeature(&f).withColorRampParameter(std::move(colorRampParameter));
    const auto& e = context;

    return this->evaluate(e);
}

EvaluationResult Expression::evaluate(optional<float> zoom,
                                      const Feature& feature,
                                      optional<double> colorRampParameter,
                                      const std::set<std::string>& availableImages) const {
    GeoJSONFeature f(feature);
    
    style::expression::EvaluationContext context;
    context.withZoom(std::move(zoom)).
    withGeometryTileFeature(&f).
    withColorRampParameter(std::move(colorRampParameter)).
    withAvailableImages(&availableImages);
    const auto& e = context;
    
    return this->evaluate(e);
}

EvaluationResult Expression::evaluate(optional<float> zoom,
                                      const Feature& feature,
                                      optional<double> colorRampParameter,
                                      const std::set<std::string>& availableImages,
                                      const CanonicalTileID& canonical) const {
    GeoJSONFeature f(feature, canonical);
    
    style::expression::EvaluationContext context;
    context.withZoom(std::move(zoom)).
    withGeometryTileFeature(&f).
    withColorRampParameter(std::move(colorRampParameter)).
    withAvailableImages(&availableImages).
    withCanonicalTileID(&canonical);
    const auto& e = context;

    return this->evaluate(e);
}

EvaluationResult Expression::evaluate(optional<mbgl::Value> accumulated, const Feature& feature) const {
    GeoJSONFeature f(feature);

    style::expression::EvaluationContext context;
    context.withAccumulated(std::move(accumulated)).
    withGeometryTileFeature(&f);
    const auto& e = context;
    
    return this->evaluate(e);
}

} // namespace expression
} // namespace style
} // namespace mbgl
