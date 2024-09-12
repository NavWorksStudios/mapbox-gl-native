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

FillExtrusionSSAOProgram::LayoutUniformValues FillExtrusionSSAOProgram::layoutUniformValues(
    const mat4& matrix, const mat4& model_matrix, const TransformState& state,
    float opacity, const EvaluatedLight& light, float verticalGradient, bool isReflection) {
    return {
        uniforms::matrix::Value( matrix ),
        uniforms::matrix::Value( model_matrix ),
        uniforms::opacity::Value( opacity ),
        uniforms::camera_pos::Value( state.getCameraWorldPosition() ),
        ssao_uniforms::lightcolor::Value( lightColor(light) ),
        ssao_uniforms::lightpos::Value( lightPosition(light, state) ),
        ssao_uniforms::lightintensity::Value( lightIntensity(light) ),
        ssao_uniforms::vertical_gradient::Value( verticalGradient ),
        uniforms::spotlight::Value( nav::runtime::spotlight::value() ),
        uniforms::render_time::Value( nav::runtime::rendertime::value() ),
        uniforms::clip_region::Value( nav::display::clip_region() ),
        uniforms::focus_region::Value( nav::display::focus_region() ),
        uniforms::is_reflection::Value( isReflection ),
    };
}

} // namespace mbgl
