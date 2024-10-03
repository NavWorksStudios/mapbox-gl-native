attribute vec3 positionIn;
attribute vec3 normalIn;

uniform mat4 modelViewMat;
uniform mat4 modelViewProjMat;
uniform mat4 normalMat;

varying vec3 vFragPos;
varying vec3 vNormal;

void main()
{
  vFragPos = vec3(modelViewMat * vec4(positionIn, 1.0));
  vNormal = vec3(normalMat * vec4(normalIn, 0.0));
  gl_Position = modelViewProjMat * vec4(positionIn, 1.0);
}
