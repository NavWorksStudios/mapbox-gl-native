#pragma once

#include <mbgl/style/layer_impl.hpp>

#include <vector>
#include <memory>

namespace mbgl {

std::string makeLayoutKey(const style::Layer::Impl& impl);

} // namespace mbgl
