#include <mbgl/programs/fill_extrusion_ssao_program.hpp>
#include <mbgl/renderer/image_atlas.hpp>
#include <mbgl/renderer/cross_faded_property_evaluator.hpp>
#include <mbgl/tile/tile_id.hpp>
#include <mbgl/map/transform_state.hpp>
#include <mbgl/util/mat3.hpp>

#include "mbgl/nav/nav.style.hpp"

namespace mbgl {

using namespace style;

static_assert(sizeof(FillExtrusionSSAOLayoutVertex) == 12, "expected FillExtrusionLayoutVertex size");

namespace ssao {
std::array<float, 3> lightColor(const EvaluatedLight& light) {
    const auto color = light.get<LightColor>();
    return {{ color.r, color.g, color.b }};
}

std::array<float, 3> lightPosition(const EvaluatedLight& light, const TransformState& state) {
    auto lightPos = light.get<LightPosition>().getCartesian();
    if (light.get<LightAnchor>() == LightAnchorType::Map) {
        return lightPos;
    } else {
        mat3 lightMat;
        matrix::identity(lightMat);
        matrix::rotate(lightMat, lightMat, -state.getBearing());
        std::array<float, 3> pos;
        matrix::transformMat3f(pos, lightPos, lightMat);
        return pos;
    }
}

float lightIntensity(const EvaluatedLight& light) {
    return light.get<LightIntensity>();
}
}

FillExtrusionSSAOProgram::LayoutUniformValues FillExtrusionSSAOProgram::layoutUniformValues(
    const mat4& matrix, const mat4& model_view_matrix, const mat4& normal_matrix) {
    return {
        uniforms::matrix::Value( matrix ),
        uniforms::mv_matrix::Value( model_view_matrix ),
        uniforms::normal_matrix::Value( normal_matrix ),
    };
}

} // namespace mbgl
