
varying vec2 TexCoords;

uniform sampler2D ssao;

void main() 
{
    vec2 texelSize = 1.0 / vec2(2048., 1080.);
    float result = 0.0;
    for (int x = -2; x < 2; ++x) 
    {
        for (int y = -2; y < 2; ++y) 
        {
            vec2 offset = vec2(float(x), float(y)) * texelSize;
            result += texture2D(ssao, TexCoords + offset).r;
        }
    }
    gl_FragColor = vec4(result / (4.0 * 4.0));
}  