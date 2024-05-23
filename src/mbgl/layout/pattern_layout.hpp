#pragma once
#include <list>
#include <mbgl/geometry/feature_index.hpp>
#include <mbgl/layout/layout.hpp>
#include <mbgl/renderer/bucket_parameters.hpp>
#include <mbgl/renderer/render_layer.hpp>
#include <mbgl/style/expression/image.hpp>
#include <mbgl/style/layer_properties.hpp>
#include <mbgl/clipper2/clipper.h>

#include "mbgl/nav/nav_log.hpp"

namespace mbgl {

class PatternDependency {
public:
    std::string min;
    std::string mid;
    std::string max;
};

using PatternLayerMap = std::map<std::string, PatternDependency>;

class PatternFeature  {
public:
    PatternFeature(std::size_t i_,
                   std::unique_ptr<GeometryTileFeature> feature_,
                   PatternLayerMap patterns_,
                   float sortKey_ = 0.0f)
        : i(i_), feature(std::move(feature_)), patterns(std::move(patterns_)), sortKey(sortKey_) {}

    friend bool operator<(const PatternFeature& lhs, const PatternFeature& rhs) { return lhs.sortKey < rhs.sortKey; }

    std::size_t i;
    std::unique_ptr<GeometryTileFeature> feature;
    PatternLayerMap patterns;
    float sortKey;
};

template <typename SortKeyPropertyType>
struct PatternFeatureInserter;

template <>
struct PatternFeatureInserter<void> {
    template <typename PropertiesType>
    static void insert(std::vector<PatternFeature>& features,
                       std::size_t index,
                       std::unique_ptr<GeometryTileFeature> feature,
                       PatternLayerMap patternDependencyMap,
                       float /*zoom*/,
                       const PropertiesType&,
                       const CanonicalTileID&) {
        features.emplace_back(index, std::move(feature), std::move(patternDependencyMap));
    }
};

template <class SortKeyPropertyType>
struct PatternFeatureInserter {
    template <typename PropertiesType>
    static void insert(std::vector<PatternFeature>& features,
                       std::size_t index,
                       std::unique_ptr<GeometryTileFeature> feature,
                       PatternLayerMap patternDependencyMap,
                       float zoom,
                       const PropertiesType& properties,
                       const CanonicalTileID& canonical) {
        const auto& sortKeyProperty = properties.template get<SortKeyPropertyType>();
        float sortKey = sortKeyProperty.evaluate(*feature, zoom, canonical, SortKeyPropertyType::defaultValue());
        PatternFeature patternFeature{index, std::move(feature), std::move(patternDependencyMap), sortKey};
        const auto lowerBound = std::lower_bound(features.cbegin(), features.cend(), patternFeature);
        features.insert(lowerBound, std::move(patternFeature));
    }
};

template <class BucketType,
          class LayerPropertiesType,
          class PatternPropertyType,
          class LayoutPropertiesType = typename style::Properties<>,
          class SortKeyPropertyType = void>
class PatternLayout : public Layout {
public:
    PatternLayout(const BucketParameters& parameters,
                  const std::vector<Immutable<style::LayerProperties>>& group,
                  std::unique_ptr<GeometryTileLayer> sourceLayer_,
                  const LayoutParameters& layoutParameters)
        : sourceLayer(std::move(sourceLayer_)),
          zoom(parameters.tileID.overscaledZ),
          overscaling(parameters.tileID.overscaleFactor()),
          hasPattern(false) {
        assert(!group.empty());
        auto leaderLayerProperties = staticImmutableCast<LayerPropertiesType>(group.front());
        layout = leaderLayerProperties->layerImpl().layout.evaluate(PropertyEvaluationParameters(zoom));
        sourceLayerID = leaderLayerProperties->layerImpl().sourceLayer;
        bucketLeaderID = leaderLayerProperties->layerImpl().id;

        for (const auto& layerProperties : group) {
            const std::string& layerId = layerProperties->baseImpl->id;
            const auto& evaluated = style::getEvaluated<LayerPropertiesType>(layerProperties);
            const auto& patternProperty = evaluated.template get<PatternPropertyType>();
            const auto constantPattern = patternProperty.constantOr(Faded<style::expression::Image>{"", ""});
            // determine if layer group has any layers that use *-pattern property and add
            // constant pattern dependencies.
            if (!patternProperty.isConstant()) {
                hasPattern = true;
            } else if (!constantPattern.to.id().empty()) {
                hasPattern = true;
                layoutParameters.imageDependencies.emplace(constantPattern.to.id(), ImageType::Pattern);
                layoutParameters.imageDependencies.emplace(constantPattern.from.id(), ImageType::Pattern);
            }
            layerPropertiesMap.emplace(layerId, layerProperties);
        }
              
        if(!sourceLayer)
            return;
              
        const size_t featureCount = sourceLayer->featureCount();
        for (size_t i = 0; i < featureCount; ++i) {
            auto feature = sourceLayer->getFeature(i);
            if (!leaderLayerProperties->layerImpl().filter(
                    style::expression::EvaluationContext(this->zoom, feature.get())
                        .withCanonicalTileID(&parameters.tileID.canonical)))
                continue;

            PatternLayerMap patternDependencyMap;
            if (hasPattern) {
                for (const auto& layerProperties : group) {
                    const std::string& layerId = layerProperties->baseImpl->id;
                    const auto it = layerPropertiesMap.find(layerId);
                    if (it != layerPropertiesMap.end()) {
                        const auto paint = static_cast<const LayerPropertiesType&>(*it->second).evaluated;
                        const auto& patternProperty = paint.template get<PatternPropertyType>();
                        if (!patternProperty.isConstant()) {
                            // For layers with non-data-constant pattern properties, evaluate their expression and add
                            // the patterns to the dependency vector
                            const auto min = patternProperty.evaluate(*feature,
                                                                      zoom - 1,
                                                                      layoutParameters.availableImages,
                                                                      parameters.tileID.canonical,
                                                                      PatternPropertyType::defaultValue());
                            const auto mid = patternProperty.evaluate(*feature,
                                                                      zoom,
                                                                      layoutParameters.availableImages,
                                                                      parameters.tileID.canonical,
                                                                      PatternPropertyType::defaultValue());
                            const auto max = patternProperty.evaluate(*feature,
                                                                      zoom + 1,
                                                                      layoutParameters.availableImages,
                                                                      parameters.tileID.canonical,
                                                                      PatternPropertyType::defaultValue());

                            layoutParameters.imageDependencies.emplace(min.to.id(), ImageType::Pattern);
                            layoutParameters.imageDependencies.emplace(mid.to.id(), ImageType::Pattern);
                            layoutParameters.imageDependencies.emplace(max.to.id(), ImageType::Pattern);
                            patternDependencyMap.emplace(layerId,
                                                         PatternDependency{min.to.id(), mid.to.id(), max.to.id()});
                        }
                    }
                }
            }

            PatternFeatureInserter<SortKeyPropertyType>::insert(features,
                                                                i,
                                                                std::move(feature),
                                                                std::move(patternDependencyMap),
                                                                zoom,
                                                                layout,
                                                                parameters.tileID.canonical);
        }
    };

    bool hasDependencies() const override { return hasPattern; }

    void createBucket(const ImagePositions& patternPositions,
                      std::unique_ptr<FeatureIndex>& featureIndex,
                      std::unordered_map<std::string, LayerRenderData>& renderData,
                      const bool /*firstLoad*/,
                      const bool /*showCollisionBoxes*/,
                      const CanonicalTileID& canonical,
                      const bool reversal) override {
        auto bucket = std::make_shared<BucketType>(layout, layerPropertiesMap, zoom, overscaling);
        if(features.size() > 0) {
            for (auto & patternFeature : features) {
                if(reversal) {
                    const auto i = patternFeature.i;
                    std::unique_ptr<GeometryTileFeature> feature = std::move(patternFeature.feature);
                    const PatternLayerMap& patterns = patternFeature.patterns;
                    const GeometryCollection& geometries = feature->getGeometries();
                    
                    // need add clipper->Difference logic
                    // ========================================================================
                    const Clipper2Lib::Paths64 tilePath = { Clipper2Lib::MakePath({8192, 8192, 0, 8192, 0, 0, 8192, 0}) };

//                    std::string cooStr = "";
                    
                    Clipper2Lib::Paths64 watersPath;
                    for(const auto& geometry : geometries) {
                        Clipper2Lib::Path64 waterPath;
                        for(const auto& geometryCoordinate : geometry) {
//                            cooStr = cooStr + std::to_string(geometryCoordinate.x) + ", " + std::to_string(geometryCoordinate.y) + ", ";
                            waterPath.push_back(Clipper2Lib::Point64(geometryCoordinate.x, geometryCoordinate.y));
                        }
                        watersPath.push_back(waterPath);
                    }
                    
                    Clipper2Lib::Paths64 landPath = Clipper2Lib::Intersect(tilePath, watersPath, Clipper2Lib::FillRule::NonZero);
                    if(landPath == tilePath) {
                        landPath.clear();
                    } else {
                        landPath = Clipper2Lib::Difference(tilePath, landPath, Clipper2Lib::FillRule::NonZero);
                    }
                    
                    // 将landsPath转换为geometries
                    GeometryCollection geometry;
                    for(const auto& path : landPath) {
                        GeometryCoordinates points;
                        for(const auto& point : path) {
                            points.push_back(Point<int16_t>{static_cast<int16_t>(point.x), static_cast<int16_t>(point.y)});
                        }
                        if (path.size() > 2) {
                            points.push_back(Point<int16_t>{static_cast<int16_t>(path[0].x), static_cast<int16_t>(path[0].y)});
                        }
                        
                        geometry.push_back(points);
                    }
                    // ========================================================================

                    bucket->addFeature(*feature, geometry, patternPositions, patterns, i, canonical);
                    featureIndex->insert(geometry, i, sourceLayerID, bucketLeaderID);
                } else {
                    const auto i = patternFeature.i;
                    std::unique_ptr<GeometryTileFeature> feature = std::move(patternFeature.feature);
                    const PatternLayerMap& patterns = patternFeature.patterns;
                    const GeometryCollection& geometries = feature->getGeometries();

                    bucket->addFeature(*feature, geometries, patternPositions, patterns, i, canonical);
                    featureIndex->insert(geometries, i, sourceLayerID, bucketLeaderID);
                }
            }
        }
        else {
            if(reversal) {
                
            }
        }

        const std::string tile = nav::log::tileId(canonical, bucketLeaderID, sourceLayerID);
        
        static std::map<std::string, int> counter;
        int count = ++counter[tile];

        nav::log::i("PatternLayout", "%s (%d) Bucket:%p \n", tile.c_str(), count, bucket.get());
        
//        assert(nav::mb::bucketMap()[tile] == 0);
        nav::log::bucketMap()[tile]++;
        bucket->key = tile;
        
        bucket->nav_upload(canonical, bucketLeaderID, sourceLayerID);
        
        if (bucket->hasData()) {
            for (const auto& pair : layerPropertiesMap) {
                renderData.emplace(pair.first, LayerRenderData {bucket, pair.second});
            }
        }
    };

protected:
    std::map<std::string, Immutable<style::LayerProperties>> layerPropertiesMap;
    std::string bucketLeaderID;

    const std::unique_ptr<GeometryTileLayer> sourceLayer;
    std::vector<PatternFeature> features;
    typename LayoutPropertiesType::PossiblyEvaluated layout;

    const float zoom;
    const uint32_t overscaling;
    std::string sourceLayerID;
    bool hasPattern;
};

} // namespace mbgl
