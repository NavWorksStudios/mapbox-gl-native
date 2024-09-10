
attribute vec3 aPos;
attribute vec3 aNormal;

varying vec3 FragPos;
varying vec3 Normal;

uniform mat4 modelViewMat;
uniform mat4 modelViewProjMat;
uniform mat4 normalMat;

void main()
{
    Normal = vec3(normalMat * vec4(aNormal, 0.0));
    FragPos = vec3(modelViewMat * vec4(aPos, 1.0));
    gl_Position = modelViewProjMat * vec4(aPos, 1.0);
}