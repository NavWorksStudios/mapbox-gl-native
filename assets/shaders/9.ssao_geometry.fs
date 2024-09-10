
varying vec2 TexCoords;
varying vec3 FragPos;
varying vec3 Normal;

void main()
{    
    // store the fragment position vector in the first gbuffer texture
    gl_FragData[0].xyz = FragPos;
    // also store the per-fragment normals into the gbuffer
    gl_FragData[1].xyz = normalize(Normal);
    // and the diffuse per-fragment color
    gl_FragData[2].rgb = vec3(0.95);
}