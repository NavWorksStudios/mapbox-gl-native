varying vec3 vFragPos;
varying vec3 vNormal;

uniform vec3 lAmbient;
uniform vec3 lPosition;
uniform vec3 lDiffuse;
uniform vec3 lSpecular;

uniform vec3 kAmbient;
uniform vec3 kDiffuse;
uniform vec3 kSpecular;
uniform float kShininess;

void main()
{
  vec3 normal = normalize(vNormal);
  vec3 ambient = lAmbient * kAmbient;
  float diffuseC = dot(normal, normalize(lPosition - vFragPos));
  vec3 diffuse = max(0.0, diffuseC) * kDiffuse * lDiffuse;

  vec3 specular = vec3(0.0);
  float specPreExp = 0.0;
  if (diffuseC >= 0.0) {
    vec3 reflection = reflect(normalize(vFragPos - lPosition), normal);
    specPreExp = dot(normalize(reflection), normalize(-vFragPos));
    specular = max(0.0, pow(specPreExp, kShininess)) * lSpecular * kSpecular;
  }

  // gl_FragColor = vec4(ambient + diffuse + specular, 1.0);
  gl_FragData[0] = vec4(ambient + diffuse + specular, 1.0);

  // store the fragment position vector in the first gbuffer texture
  gl_FragData[1].xyz = vFragPos;
  gl_FragData[1].a = 1.0;
}
