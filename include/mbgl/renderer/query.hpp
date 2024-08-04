#pragma once

#include <mbgl/util/optional.hpp>
#include <mbgl/style/filter.hpp>

#include <vector>

#include "mbgl/nav/nav.stringid.hpp"

namespace mbgl {

/**
 * Options for query rendered features.
 */
class RenderedQueryOptions {
public:
    RenderedQueryOptions(optional<std::vector<nav::stringid>> layerIDs_ = {},
                         optional<style::Filter> filter_ = {})
        : layerIDs(std::move(layerIDs_)),
          filter(std::move(filter_)) {}

    /** layerIDs to include in the query */
    optional<std::vector<nav::stringid>> layerIDs;

    optional<style::Filter> filter;
};

/**
 * Options for query source features
 */
class SourceQueryOptions {
public:
    SourceQueryOptions(optional<std::vector<nav::stringid>> sourceLayers_ = {},
                       optional<style::Filter> filter_ = {})
        : sourceLayers(std::move(sourceLayers_)),
          filter(std::move(filter_)) {}

    // Required for VectorSource, ignored for GeoJSONSource
    optional<std::vector<nav::stringid>> sourceLayers;

    optional<style::Filter> filter;
};

} // namespace mbgl
