

#if 1

varying vec2 TexCoords;

uniform sampler2D ssao;

const vec2 texelSize = 1.0 / vec2(2048., 1080.) / 2.;

void main() 
{
    float result = 0.0;

    for (int x = -2; x < 2; ++x) 
    {
        for (int y = -2; y < 2; ++y) 
        {
            vec2 offset = vec2(float(x), float(y)) * texelSize;
            result += texture2D(ssao, TexCoords + offset).r;
        }
    }

    result /= (4.0 * 4.0);

    gl_FragColor.r = result;

    // gl_FragColor = vec4(result);
}  

#else

uniform sampler2D ssao;
varying vec2 TexCoords;

const vec2 texelSize = 1.0 / vec2(2048., 1080.) / 2.;

#define SEG 2

void main()
{
    float color = 0.0;
    float total = 0.0;

    for(int i = -SEG; i <= SEG; ++i)
    {
        for(int j = -SEG; j <= SEG; ++j)
        {
            float w = (1.1 - sqrt(float(i*i + j*j)) / 8.0);
            w *= w;

            vec2 offset = vec2(float(i), float(j)) * texelSize;
            color += texture2D(ssao, TexCoords + offset).r * w;

            total += w;
        }
    }

    color /= total;

    gl_FragColor.r = color;
}

#endif