
varying vec2 TexCoords;

uniform sampler2D depthMap;

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // Back to NDC 
    return (2.0 * 10.0 * 75.0) / (75.0 + 10.0 - z * (75.0 - 10.0));    
}

void main()
{
    float depthValue = texture2D(depthMap, TexCoords).r;
    // gl_FragColor = vec4(vec3(LinearizeDepth(depthValue) / 75.0), 1.0); // perspective
    gl_FragColor = vec4(vec3(depthValue), 1.0);
}
