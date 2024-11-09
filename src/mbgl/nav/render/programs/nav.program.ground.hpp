
#include "mbgl/nav/render/programs/nav.program.hpp"

namespace mbgl {
const char* nav_programs_geo_fragmentShader();
}

namespace nav {
namespace programs {
namespace ground {

const char* vertexShader() { return R"(

attribute vec2 a_pos;
attribute vec4 a_normal_ed;

uniform mat4 u_matrix;
uniform mat4 u_model_view_matrix;
uniform mat4 u_normal_matrix;
uniform mat4 u_light_matrix;

varying vec3 v_aospace_normal;
varying vec3 v_aospace_pos;

varying vec3 v_lightspace_normal;
varying vec4 v_lightspace_pos;

void main()
{
    vec4 pos = vec4(a_pos, 0., 1.);
    gl_Position = u_matrix * pos;

    vec4 outward_normal = vec4(-a_normal_ed.x, -a_normal_ed.y, a_normal_ed.z, a_normal_ed.w);

    // ssao
    v_aospace_normal = vec3(u_normal_matrix * outward_normal);
    v_aospace_pos = vec3(u_model_view_matrix * pos) / 32.;

    // shadow
    v_lightspace_normal = vec3(outward_normal);
    v_lightspace_pos = u_light_matrix * pos;
}

)"; }

const char* fragmentShader() {
    return mbgl::nav_programs_geo_fragmentShader();
}

} // namespace ssao
} // namespace programs
} // nav
