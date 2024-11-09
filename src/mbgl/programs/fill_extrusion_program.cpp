#include <mbgl/programs/fill_extrusion_program.hpp>
#include <mbgl/renderer/image_atlas.hpp>
#include <mbgl/renderer/cross_faded_property_evaluator.hpp>
#include <mbgl/tile/tile_id.hpp>
#include <mbgl/map/transform_state.hpp>
#include <mbgl/util/mat3.hpp>

#include "mbgl/nav/nav.runtime.hpp"

namespace mbgl {

using namespace style;

static_assert(sizeof(FillExtrusionLayoutVertex) == 12, "expected FillExtrusionLayoutVertex size");

std::array<float, 3> lightColor(const EvaluatedLight& light) {
    const auto color = light.get<LightColor>();
    return {{ color.r, color.g, color.b }};
}

std::array<float, 3> lightPos(const EvaluatedLight& light, const TransformState& state) {
    auto lightPos = light.get<LightPosition>().getCartesian();
    if (light.get<LightAnchor>() == LightAnchorType::Map) {
        return lightPos;
    } else {
        mat3 lightMat;
        matrix::identity(lightMat);
        matrix::rotate(lightMat, lightMat, -state.getBearing());
        std::array<float, 3> result;
        matrix::transformMat3f(result, lightPos, lightMat);
        return result;
    }
}

float lightIntensity(const EvaluatedLight& light) {
    return light.get<LightIntensity>();
}

FillExtrusionProgram::LayoutUniformValues FillExtrusionProgram::layoutUniformValues(
    const mat4& matrix, const mat4& model_matrix, const TransformState& state,
    float opacity, const EvaluatedLight& light, float verticalGradient, bool renderReflection) {
    nav::runtime::sunlight::setPos(lightPos(light, state));
    
    return {
        uniforms::matrix::Value( matrix ),
        uniforms::model_matrix::Value( model_matrix ),
        uniforms::opacity::Value( opacity ),
        uniforms::camera_pos::Value( state.getCameraPosition() ),
        uniforms::lightcolor::Value( lightColor(light) ),
        uniforms::lightpos::Value( nav::runtime::sunlight::pos() ),
        uniforms::lightintensity::Value( lightIntensity(light) ),
        uniforms::vertical_gradient::Value( verticalGradient ),
        uniforms::spotlight::Value( nav::runtime::spotlight::value() ),
        uniforms::render_time::Value( nav::runtime::rendertime::value() ),
        uniforms::clip_region::Value( nav::display::clip_region() ),
        uniforms::focus_region::Value( nav::display::focus_region() ),
        uniforms::render_reflection::Value( renderReflection ),
    };
}

FillExtrusionPatternProgram::LayoutUniformValues
FillExtrusionPatternProgram::layoutUniformValues(mat4 matrix,
                                                 Size atlasSize,
                                                 const CrossfadeParameters& crossfade,
                                                 const UnwrappedTileID& tileID,
                                                 const TransformState& state,
                                                 const float opacity,
                                                 const float heightFactor,
                                                 const float pixelRatio,
                                                 const EvaluatedLight& light,
                                                 const float verticalGradient,
                                                 const bool renderReflection) {
    const auto tileRatio = 1 / tileID.pixelsToTileUnits(1, state.getIntegerZoom());
    int32_t tileSizeAtNearestZoom = util::tileSize * state.zoomScale(state.getIntegerZoom() - tileID.canonical.z);
    int32_t pixelX = tileSizeAtNearestZoom * (tileID.canonical.x + tileID.wrap * state.zoomScale(tileID.canonical.z));
    int32_t pixelY = tileSizeAtNearestZoom * tileID.canonical.y;
    
    nav::runtime::sunlight::setPos(lightPos(light, state));

    return {
        uniforms::matrix::Value( matrix ),
        uniforms::opacity::Value( opacity ),
        uniforms::scale::Value( {{pixelRatio, tileRatio, crossfade.fromScale, crossfade.toScale}} ),
        uniforms::texsize::Value( atlasSize ),
        uniforms::fade::Value( crossfade.t ),
        uniforms::pixel_coord_upper::Value( std::array<float, 2>{{ float(pixelX >> 16), float(pixelY >> 16) }} ),
        uniforms::pixel_coord_lower::Value( std::array<float, 2>{{ float(pixelX & 0xFFFF), float(pixelY & 0xFFFF) }} ),
        uniforms::height_factor::Value( heightFactor ),
        uniforms::lightcolor::Value( lightColor(light) ),
        uniforms::lightpos::Value( nav::runtime::sunlight::pos() ),
        uniforms::lightintensity::Value( lightIntensity(light) ),
        uniforms::vertical_gradient::Value( verticalGradient ),
        uniforms::spotlight::Value( nav::runtime::spotlight::value() ),
        uniforms::render_reflection::Value( renderReflection ),
        uniforms::render_time::Value( nav::runtime::rendertime::value() ),
        uniforms::clip_region::Value( nav::display::clip_region() ),
        uniforms::focus_region::Value( nav::display::focus_region() ),
    };
}

FillExtrusionShadowDepthProgram::LayoutUniformValues
FillExtrusionShadowDepthProgram::layoutUniformValues(const mat4& matrix,
                                                     const mat4& model_view_matrix,
                                                     const mat4& normal_matrix) {
    return {
        uniforms::matrix::Value( matrix ),
        uniforms::model_view_matrix::Value( model_view_matrix ),
        uniforms::normal_matrix::Value( normal_matrix ),
    };
}

FillExtrusionGeoProgram::LayoutUniformValues
FillExtrusionGeoProgram::layoutUniformValues(const mat4& matrix,
                                             const mat4& model_view_matrix,
                                             const mat4& normal_matrix,
                                             const mat4& light_matrix,
                                             const vec3f& light_dir) {
    return {
        uniforms::matrix::Value( matrix ),
        uniforms::model_view_matrix::Value( model_view_matrix ),
        uniforms::normal_matrix::Value( normal_matrix ),
        uniforms::light_matrix::Value( light_matrix ),
        uniforms::light_dir::Value( light_dir ),
    };
}

} // namespace mbgl
