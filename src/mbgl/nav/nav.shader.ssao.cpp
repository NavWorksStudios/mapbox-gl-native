//
//  nav.shader.ssao.cpp
//
//  Created by BernieZhao on 2024/9/9.
//

#include "mbgl/nav/nav.shader.ssao.hpp"


namespace nav {
namespace shader {
namespace ssao {

const char* phong_vert() {  return R"(

attribute vec3 positionIn;
attribute vec3 normalIn;

uniform mat4 modelViewMat;
uniform mat4 modelViewProjMat;
uniform mat4 normalMat;

varying vec3 normalV;
varying vec3 positionV;
varying vec3 normTest;

void main()
{
  normTest = normalIn;
  normalV = vec3(normalMat * vec4(normalIn, 0.0));
  positionV = vec3(modelViewMat * vec4(positionIn, 1.0));
  gl_Position = modelViewProjMat * vec4(positionIn, 1.0);
}


)"; }

const char* phong_frag() {  return R"(

varying vec3 normalV;
varying vec3 positionV;

varying vec3 normTest;

uniform vec3 lAmbient;

uniform vec3 lPosition;
uniform vec3 lDiffuse;
uniform vec3 lSpecular;

uniform vec3 kAmbient;
uniform vec3 kDiffuse;
uniform vec3 kSpecular;
uniform float kShininess;

uniform float doSSAO;

void main()
{
  vec3 normal = normalize(normalV);
  vec3 ambient = lAmbient * kAmbient;
  float diffuseC = dot(normal, normalize(lPosition - positionV));
  vec3 diffuse = max(0.0, diffuseC) * kDiffuse * lDiffuse;

  vec3 specular = vec3(0.0);
  float specPreExp = 0.0;
  if (diffuseC >= 0.0) {
    vec3 reflection = reflect(normalize(positionV - lPosition), normal);
    specPreExp = dot(normalize(reflection), normalize(-positionV));
    specular = max(0.0, pow(specPreExp, kShininess)) * lSpecular * kSpecular;
  }

  if (doSSAO > 0.5) {
    gl_FragData[0] = vec4(ambient + diffuse + specular, 1.0);
    // Scale and bias normal from [-1, 1] to [0, 1]
    vec3 scaledNormal = 0.5 * (normal + vec3(1.0));
    gl_FragData[1] = vec4(scaledNormal, 0.0);
  }
  else {
    gl_FragData[0] = vec4(ambient + diffuse + specular, 1.0);
  }
}


)"; }

const char* ssao_vert() {  return R"(

attribute vec2 positionIn;

varying vec2 texCoord;

void main()
{
  // Scale and bias [-1, 1] to [0, 1]
  texCoord = (positionIn * vec2(0.5)) + vec2(0.5);
  gl_Position = vec4(positionIn, 0.0, 1.0);
}


)"; }

const char* ssao_frag() {  return R"(

varying vec2 texCoord;

uniform sampler2D depthTex;
uniform sampler2D normTex;
uniform sampler2D randomTex;
uniform vec2 randomTexCoordScale;

uniform mat4 projMat;
uniform mat4 invProjMat;

uniform vec3 sampleOffsets[16];

uniform float sampleRadius;

void main()
{
  // Holds an occlusion factor for this fragment, to be output at the end
  float occlusion = 0.0;

  // Construct a position for the rendered fragment
  float depth = texture(depthTex, texCoord).r;
  vec3 screenPosition = vec3(texCoord, depth);
  // Go from [0, 1] to [-1, 1]
  vec3 clipPosition = (2.0 * screenPosition) - vec3(1.0);

  // Undo the projection to get back to view coordinates
  vec4 viewPosition = invProjMat * vec4(clipPosition, 1.0);
  viewPosition /= viewPosition.w;

  vec3 normal = texture(normTex, texCoord).xyz;
  // Scale and bias
  normal = (2.0 * normal) - vec3(1.0);
  normal = normalize(normal);

  // Construct our rotation matrix (used to transform sample offsets) based on a random vector lookup
  vec3 randomVector = texture(randomTex, texCoord * randomTexCoordScale).xyz;
  // Go from [0, 1] to [-1, 1], normalizing along the way
  randomVector = normalize((2.0 * randomVector) - vec3(1.0));

  vec3 tangent = normalize(randomVector - normal * dot(normal, randomVector));
  vec3 bitangent = cross(normal, tangent);

  mat3 orientMat = mat3(tangent, bitangent, normal);

  for (int i = 0; i < 16; i++) {
    // Construct our view-space location to sample
    vec3 testSample = (orientMat * sampleOffsets[i]) * sampleRadius + viewPosition.xyz;

    // Transform the sample location into clip coords for depth comparison
    vec4 clipSample = projMat * vec4(testSample, 1.0);
    clipSample.xy /= clipSample.w;
    // Scale and bias to screen coords, [-1, 1] -> [0, 1]
    vec2 screenSample = (clipSample.xy + vec2(1.0)) * 0.5;

    float lookupDepth = texture(depthTex, screenSample).x;
    float rangeCheck = abs(lookupDepth - depth) > sampleRadius ? 0.0 : 1.0;
    occlusion += (lookupDepth < depth ? 1.0 : 0.0) * rangeCheck;
  }

  // Normalize occlusion factor
  occlusion /= 16;

  // Subtract from 1 to give a direct scale factor for lighting
  occlusion = 1.0 - occlusion;

  gl_FragColor = vec4(occlusion, occlusion, occlusion, 1.0);
}


)"; }

const char* blur_vert() {  return R"(

attribute vec2 positionIn;

varying vec2 texCoord;

void main()
{
  // Scale and bias [-1, 1] to [0, 1]
  texCoord = (positionIn * vec2(0.5)) + vec2(0.5);
  gl_Position = vec4(positionIn, 0.0, 1.0);
}


)"; }

const char* blur_frag() {  return R"(

varying vec2 texCoord;

uniform sampler2D aoTex;
uniform sampler2D colorTex;
uniform vec2 invRes;

void main()
{
  float scaleFactor = 0.0;

  for (int x = 0; x < 4; x++) {
    for (int y = 0; y < 4; y++) {
      vec2 offset = invRes * vec2(x, y);
      vec2 realTexCoord = texCoord + offset;
      scaleFactor += texture(aoTex, realTexCoord).r;
    }
  }

  // Average out the accumulated scaleFactor
  scaleFactor /= 16.0;

  // Use the blurred occlusion value to scale the input color texture
  vec3 pixValue = scaleFactor * texture(colorTex, texCoord).rgb;

  gl_FragColor = vec4(pixValue, 1.0);
}

)"; }


}
}
}
