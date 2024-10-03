varying vec3 vFragPos;
varying vec3 vNormal;

void main()
{
  // gl_FragColor = vec4(1.0, 1.0, 1.0,1.0);
  gl_FragData[0] = vec4(1.0, 1.0, 1.0,1.0);
  
  // store the fragment position vector in the first gbuffer texture
  gl_FragData[1].xyz = vFragPos;
  gl_FragData[1].a = 1.0;
}
