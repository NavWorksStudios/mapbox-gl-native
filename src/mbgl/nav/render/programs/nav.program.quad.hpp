
#include "mbgl/nav/render/programs/nav.program.hpp"

namespace nav {
namespace programs {
namespace quad {


static const char* vertexShader() { return R"(

attribute vec3 aPos;
attribute vec2 aTexCoords;
varying vec2 TexCoords;

void main() {
    gl_Position = vec4(aPos, 1.0);
    TexCoords = aTexCoords;
}

)"; }

static const char* standardFragmentShader() { return R"(

uniform float u_opacity;
uniform sampler2D u_buffer;
varying vec2 TexCoords;

void main() {
    gl_FragColor = texture2D(u_buffer, TexCoords);
    gl_FragColor.a *= u_opacity;
}

)"; }


static const char* redChannelFragmentShader() { return R"(

uniform float u_opacity;
uniform sampler2D u_buffer;
varying vec2 TexCoords;

void main() {
    float result = texture2D(u_buffer, TexCoords).r;
    gl_FragColor = vec4(vec3(1.) * result, u_opacity);
}

)"; }


static const char* blurFragmentShader() { return R"(

uniform sampler2D u_buffer;
uniform vec2 u_offset;
varying vec2 TexCoords;

float kawaseBlur(vec2 uv) {    
    float color = texture2D(u_buffer, uv).r;
    color += texture2D(u_buffer, uv + vec2(+u_offset.x, +u_offset.y)).r;
    color += texture2D(u_buffer, uv + vec2(+u_offset.x, -u_offset.y)).r;
    color += texture2D(u_buffer, uv + vec2(-u_offset.x, +u_offset.y)).r;
    color += texture2D(u_buffer, uv + vec2(-u_offset.x, -u_offset.y)).r;
    return color / 5.;
}

void main() {
    vec3 color = vec3(0.2, 0.28, 0.29);
    float alpha = kawaseBlur(TexCoords);
    gl_FragColor = vec4(color, alpha);
}

)"; }


} // namespace quad
} // namespace programs
} // nav
