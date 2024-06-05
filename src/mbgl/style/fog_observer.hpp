#pragma once

namespace mbgl {
namespace style {

class Fog;

class FogObserver {
public:
    virtual ~FogObserver() = default;

    virtual void onFogChanged(const Fog&) {}
};

} // namespace style
} // namespace mbgl
