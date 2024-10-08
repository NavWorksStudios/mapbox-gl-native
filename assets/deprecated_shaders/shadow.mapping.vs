
attribute vec3 aPos;
attribute vec3 aNormal;
// attribute vec2 aTexCoords;

varying vec3 vFragPos;
varying vec3 vNormal;
// varying vec2 vTexCoords;
varying vec4 vFragPosLightSpace;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat4 lightSpaceMatrix;
// uniform vec2 textureSizeD;

void main()
{
    vFragPos = vec3(model * vec4(aPos, 1.0));
    vNormal = aNormal;
    vFragPosLightSpace = lightSpaceMatrix * vec4(vFragPos, 1.0);
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
