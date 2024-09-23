
varying vec2 TexCoords;

uniform sampler2D ssaoBlur;

void main()
{             
    // retrieve data from gbuffer
    float ao = texture2D(ssaoBlur, TexCoords).r;

    gl_FragColor = vec4(0., 0., 0., 1. - ao);
}