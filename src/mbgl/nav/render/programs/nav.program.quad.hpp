
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
uniform vec4 u_bg_color;
uniform sampler2D u_buffer;
varying vec2 TexCoords;

void main() {
    gl_FragColor = texture2D(u_buffer, TexCoords);
    gl_FragColor += u_bg_color * (1. - gl_FragColor.a);
    gl_FragColor *= u_opacity;
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

vec4 kawaseBlur(vec2 uv) {    
    vec4 color = texture2D(u_buffer, uv);
    color += texture2D(u_buffer, uv + vec2(+u_offset.x, +u_offset.y));
    color += texture2D(u_buffer, uv + vec2(+u_offset.x, -u_offset.y));
    color += texture2D(u_buffer, uv + vec2(-u_offset.x, +u_offset.y));
    color += texture2D(u_buffer, uv + vec2(-u_offset.x, -u_offset.y));
    return color / 5.;
}

void main() {
    gl_FragColor = kawaseBlur(TexCoords);
}

)"; }


} // namespace quad
} // namespace programs
} // nav
