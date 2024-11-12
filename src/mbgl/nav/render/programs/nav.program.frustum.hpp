
#include "mbgl/nav/render/programs/nav.program.hpp"

namespace mbgl {
const char* nav_programs_geo_fragmentShader();
}

namespace nav {
namespace programs {
namespace frustum {

const char* vertexShader() { return R"(

attribute vec3 a_pos;
uniform mat4 u_matrix;

void main()
{
    vec4 pos = vec4(a_pos, 1.);
    gl_Position = u_matrix * pos;

    // 防字段丢失，无实际意义
    vec4 out_pos = u_matrix * pos;
}

)"; }

const char* fragmentShader() { return R"(

void main() {
    gl_FragColor = vec4(1.0, 0.0, 0.0, 0.5);
}

)"; }

} // namespace frustum
} // namespace programs
} // nav
