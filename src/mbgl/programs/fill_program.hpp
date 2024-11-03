#pragma once

#include <mbgl/programs/program.hpp>
#include <mbgl/programs/attributes.hpp>
#include <mbgl/programs/uniforms.hpp>
#include <mbgl/programs/textures.hpp>
#include <mbgl/util/geometry.hpp>
#include <mbgl/util/mat4.hpp>
#include <mbgl/util/size.hpp>
#include <mbgl/style/layers/fill_layer_properties.hpp>

#include <string>

namespace mbgl {

class ImagePosition;
class UnwrappedTileID;
class TransformState;
template <class> class Faded;

using FillLayoutAttributes = PositionOnlyLayoutAttributes;

using FillUniforms = TypeList<
    uniforms::matrix,
    uniforms::model_matrix,
    uniforms::world,
    // 五彩地面及水波纹参数
    uniforms::spotlight,                // 五彩地面开启标示
    uniforms::render_time,              // 渲染周期时间
    uniforms::palette_color,            // 调色板基色
    uniforms::palette_lightness,        // 调色板参数
    uniforms::water_wave,               // 水波纹参数
    uniforms::water_data_z_scale,       // 水波纹参数
    uniforms::clip_region,              // 视口远端切割范围
    uniforms::focus_region,             // 探照灯范围
    // 水面贴图、地面贴图参数
    uniforms::texsize,                  // 贴图尺寸
    uniforms::textype                   // 贴图类别: 0.-空白，1.-水面，2.-草地1，3.-草地2
>;

using FillPatternUniforms = TypeList<
    uniforms::matrix,
    uniforms::world,
    uniforms::texsize,
    uniforms::scale,
    uniforms::fade,
    uniforms::pixel_coord_upper,
    uniforms::pixel_coord_lower>;

class FillProgram : public Program<
    FillProgram,
    gfx::PrimitiveType::Triangle,
    FillLayoutAttributes,
    FillUniforms,
    TypeList<textures::image>,
    style::FillPaintProperties>
{
public:
    using Program::Program;

    static LayoutVertex layoutVertex(Point<int16_t> p) {
        return LayoutVertex {
            {{
                p.x,
                p.y
            }}
        };
    }
};

class FillPatternProgram : public Program<
    FillPatternProgram,
    gfx::PrimitiveType::Triangle,
    FillLayoutAttributes,
    FillPatternUniforms,
    TypeList<textures::image>,
    style::FillPaintProperties>
{
public:
    using Program::Program;

    static LayoutUniformValues layoutUniformValues(mat4 matrix,
                                                   Size framebufferSize,
                                                   Size atlasSize,
                                                   const CrossfadeParameters& crossfade,
                                                   const UnwrappedTileID&,
                                                   const TransformState&,
                                                   float pixelRatio);
};

class FillOutlineProgram : public Program<
    FillOutlineProgram,
    gfx::PrimitiveType::Line,
    FillLayoutAttributes,
    FillUniforms,
    TypeList<>,
    style::FillPaintProperties>
{
public:
    using Program::Program;
};

class FillOutlinePatternProgram : public Program<
    FillOutlinePatternProgram,
    gfx::PrimitiveType::Line,
    FillLayoutAttributes,
    FillPatternUniforms,
    TypeList<
        textures::image>,
    style::FillPaintProperties>
{
public:
    using Program::Program;
};

using FillLayoutVertex = FillProgram::LayoutVertex;
using FillAttributes = FillProgram::AttributeList;

class FillLayerPrograms final : public LayerTypePrograms {
public:
    FillLayerPrograms(gfx::Context& context, const ProgramParameters& programParameters)
        : fill(context, programParameters),
          fillPattern(context, programParameters),
          fillOutline(context, programParameters),
          fillOutlinePattern(context, programParameters) {}
    FillProgram fill;
    FillPatternProgram fillPattern;
    FillOutlineProgram fillOutline;
    FillOutlinePatternProgram fillOutlinePattern;
};

} // namespace mbgl
