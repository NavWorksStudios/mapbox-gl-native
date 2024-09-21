
attribute vec3 aPos;
attribute vec2 aTexCoords;

varying vec2 TexCoords;

void main()
{
    gl_Position = vec4(aPos, 1.0);
    TexCoords = aTexCoords;
}