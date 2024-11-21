
#include "mbgl/nav/render/programs/nav.program.hpp"

namespace nav {
namespace programs {
namespace skybox {

const char* vertexShader() { return R"(

attribute vec3 a_pos;
uniform mat4 u_matrix;
varying vec3 TexCoords;

void main() {
    TexCoords = a_pos;
    gl_Position = u_matrix * vec4(a_pos, 1.);
}

)"; }

const char* fragmentShader() { return R"(

varying vec3 TexCoords;
uniform samplerCube u_skybox;

void main() {
    gl_FragColor = textureCube(u_skybox, TexCoords);
}

)"; }

} // namespace skybox
} // namespace programs
} // nav
