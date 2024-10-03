attribute vec3 positionIn;
uniform mat4 modelViewProjMat;

void main()
{
  gl_Position = modelViewProjMat * vec4(positionIn, 1.0);
}
