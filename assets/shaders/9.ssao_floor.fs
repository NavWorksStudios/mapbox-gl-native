
varying vec3 vFragPos;
varying vec3 vNormal;

const float NEAR = 0.1; // 投影矩阵的近平面
const float FAR = 50.0; // 投影矩阵的远平面

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // 回到NDC
    return (2.0 * NEAR * FAR) / (FAR + NEAR - z * (FAR - NEAR));
}

void main()
{
    // store the fragment position vector in the first gbuffer texture
    gl_FragData[0].xyz = vFragPos;
    gl_FragData[0].a = LinearizeDepth(gl_FragCoord.z);

    // also store the per-fragment normals into the gbuffer
    gl_FragData[1].xyz = normalize(vNormal);
    gl_FragData[1].a = 1.0;

    // and the diffuse per-fragment color
    gl_FragData[2].rgb = vec3(.95);
    gl_FragData[2].a = 1.0;
}