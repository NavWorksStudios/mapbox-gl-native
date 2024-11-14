
#include "mbgl/nav/render/programs/nav.program.hpp"

namespace mbgl {
const char* nav_programs_geo_fragmentShader();
}

namespace nav {
namespace programs {
namespace frustum {

const char* vertexShader() { return R"(

uniform mat4 u_matrix;
attribute vec3 a_pos;

void main() {
    gl_Position = u_matrix * vec4(a_pos, 1.);
    vec3 pos = a_pos;
    if(pos.z > 0.0) pos.z = 0.0;
    gl_Position = vec4(pos, 1.);
}

)"; }

const char* fragmentShader() { return R"(

uniform vec4 u_color;

void main() {
    gl_FragColor = u_color;
}

)"; }

} // namespace frustum
} // namespace programs
} // nav
