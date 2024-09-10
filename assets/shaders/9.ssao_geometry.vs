
attribute vec3 aPos;
attribute vec3 aNormal;
attribute vec2 aTexCoords;

varying vec3 FragPos;
varying vec2 TexCoords;
varying vec3 Normal;

uniform bool invertedNormals;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

mat3 to_mat3(mat4 m) {
    mat3 result;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            result[i][j] = m[i][j];
        }
    }
    return result;
}

mat3 inverse(mat3 m) {
    float determinant = m[0][0] * (m[1][1] * m[2][2] - m[1][2] * m[2][1]) -
                        m[0][1] * (m[1][0] * m[2][2] - m[1][2] * m[2][0]) +
                        m[0][2] * (m[1][0] * m[2][1] - m[1][1] * m[2][0]);

    if (determinant == 0.0) {
        // 矩阵不可逆，返回单位矩阵或错误处理
        return mat3(1.0);
    }

    float invDeterminant = 1.0 / determinant;

    return mat3(
        (m[1][1] * m[2][2] - m[1][2] * m[2][1]) * invDeterminant,
        -(m[0][1] * m[2][2] - m[0][2] * m[2][1]) * invDeterminant,
        (m[0][1] * m[1][2] - m[0][2] * m[1][1]) * invDeterminant,
        -(m[1][0] * m[2][2] - m[1][2] * m[2][0]) * invDeterminant,
        (m[0][0] * m[2][2] - m[0][2] * m[2][0]) * invDeterminant,
        -(m[0][0] * m[1][2] - m[0][2] * m[1][0]) * invDeterminant,
        (m[1][0] * m[2][1] - m[1][1] * m[2][0]) * invDeterminant,
        -(m[0][0] * m[2][1] - m[0][1] * m[2][0]) * invDeterminant,
        (m[0][0] * m[1][1] - m[0][1] * m[1][0]) * invDeterminant
    );
}

mat3 transpose(mat3 m) {
    return mat3(
        m[0][0], m[1][0], m[2][0],
        m[0][1], m[1][1], m[2][1],
        m[0][2], m[1][2], m[2][2]
    );
}

void main()
{
    vec4 viewPos = view * model * vec4(aPos, 1.0);
    FragPos = viewPos.xyz; 
    TexCoords = aTexCoords;
    
    mat3 normalMatrix = transpose(inverse(to_mat3(view * model)));
    Normal = normalMatrix * (invertedNormals ? -aNormal : aNormal);
    
    gl_Position = projection * viewPos;
}