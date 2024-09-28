
varying vec3 FragPos;
varying vec3 Normal;


// const float NEAR = 0.1; // 投影矩阵的近平面
// const float FAR = 50.0; // 投影矩阵的远平面

// float LinearizeDepth(float depth)
// {
//     float z = depth * 2.0 - 1.0; // 回到NDC
//     return (2.0 * NEAR * FAR) / (FAR + NEAR - z * (FAR - NEAR));    
// }

void main()
{    
    // store the fragment position vector in the first gbuffer texture
    gl_FragData[0].xyz = FragPos;
    // gl_FragData[0].a = LinearizeDepth(gl_FragCoord.w);

    // also store the per-fragment normals into the gbuffer
    gl_FragData[1].xyz = normalize(Normal);

    // and the diffuse per-fragment color
    gl_FragData[2].rgb = vec3(.95);
}