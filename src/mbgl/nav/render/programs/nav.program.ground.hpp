
#include "mbgl/nav/render/programs/nav.program.hpp"

namespace mbgl {
const char* nav_programs_ground_fragmentShader();
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

varying vec3 v_fragPos;
varying vec3 v_normal;
varying vec3 v_ao_normal;
varying vec4 v_lightSpacePos;

void main()
{
    vec4 pos = vec4(a_pos, 0., 1.);

    // ssao
    v_fragPos = vec3(u_model_view_matrix * pos) / 32.;

    v_normal = vec3(-a_normal_ed.x, -a_normal_ed.y, a_normal_ed.z);
    v_ao_normal = vec3(u_normal_matrix * vec4(v_normal, a_normal_ed.w));

    // shadow
    v_lightSpacePos = u_light_matrix * pos;

    gl_Position = u_matrix * pos;
}

)"; }

const char* fragmentShader() {
    return mbgl::nav_programs_ground_fragmentShader();
}

} // namespace ssao
} // namespace programs
} // nav
