
varying vec2 TexCoords;

uniform sampler2D u_ssaoBlur;
uniform vec3 u_color;

void main()
{             
    // retrieve data from gbuffer
    float ao = texture2D(u_ssaoBlur, TexCoords).r;
    gl_FragColor = vec4(vec3(.2), 1. - ao);

    // gl_FragColor = vec4(u_color, 1.) * (1. - ao);
}