#pragma once

#include <mbgl/programs/program.hpp>
#include <mbgl/programs/attributes.hpp>
#include <mbgl/programs/uniforms.hpp>
#include <mbgl/programs/textures.hpp>
#include <mbgl/util/geometry.hpp>
#include <mbgl/util/mat4.hpp>
#include <mbgl/util/size.hpp>
#include <mbgl/style/layers/fill_extrusion_layer_properties.hpp>
#include <mbgl/style/style.hpp>
#include <mbgl/renderer/render_light.hpp>

#include <string>

namespace mbgl {

class ImagePosition;
class UnwrappedTileID;
class TransformState;
template <class> class Faded;

namespace ssao_uniforms {
MBGL_DEFINE_UNIFORM_VECTOR(float, 3, lightpos);
MBGL_DEFINE_UNIFORM_VECTOR(float, 3, lightcolor);
MBGL_DEFINE_UNIFORM_SCALAR(float,    lightintensity);
MBGL_DEFINE_UNIFORM_SCALAR(float,    vertical_gradient);
MBGL_DEFINE_UNIFORM_SCALAR(float,    height_factor);
} // namespace uniforms

using FillExtrusionSSAOLayoutAttributes = TypeList<
    attributes::pos,
    attributes::normal_ed>;

using FillExtrusionSSAOUniforms = TypeList<
    uniforms::matrix,
    uniforms::mv_matrix,
    uniforms::normal_matrix>;

class FillExtrusionSSAOProgram : public Program<
    FillExtrusionSSAOProgram,
    gfx::PrimitiveType::Triangle,
    FillExtrusionSSAOLayoutAttributes,
    FillExtrusionSSAOUniforms,
    TypeList<>,
    style::FillExtrusionPaintProperties>
{
public:
    using Program::Program;

    static LayoutVertex layoutVertex(Point<int16_t> p, 
                                     double nx, double ny, double nz, unsigned short t, int16_t edgeDistance) {
        const auto factor = pow(2, 13); // 8192

        return LayoutVertex {
            {{
                p.x,
                p.y
            }},
            {{
                // Multiply normal vector components by 2^14 to pack them into integers
                // We pack a bool (`t`) into the x component indicating whether it is an upper or lower vertex
                static_cast<int16_t>(floor(nx * factor) * 2 + t),
                static_cast<int16_t>(ny * factor * 2),
                static_cast<int16_t>(nz * factor * 2),
                // The edgedistance attribute is used for wrapping fill_extrusion patterns
                edgeDistance
            }}
        };
    }

    static LayoutUniformValues layoutUniformValues(const mat4&, const mat4&, const mat4&);
};


using FillExtrusionSSAOLayoutVertex = FillExtrusionSSAOProgram::LayoutVertex;
using FillExtrusionSSAOAttributes = FillExtrusionSSAOProgram::AttributeList;


class FillExtrusionSSAOLayerPrograms final : public LayerTypePrograms {
public:
    FillExtrusionSSAOLayerPrograms(gfx::Context& context, const ProgramParameters& programParameters)
        : fillExtrusion(context, programParameters) {
    }
    FillExtrusionSSAOProgram fillExtrusion;
};

const char* vertexShader();
const char* fragmentShader();

} // namespace mbgl
