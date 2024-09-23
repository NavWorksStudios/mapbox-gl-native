
attribute vec2 a_pos;
attribute vec4 a_normal_ed;

varying vec3 vFragPos;
varying vec3 vNormal;

uniform mat4 u_mv_matrix;
uniform mat4 u_matrix;
uniform mat4 u_normal_matrix;

void main()
{
    vec4 pos = vec4(a_pos, 0.0, 1.0);
    vFragPos = vec3(u_mv_matrix * pos) / 32.;
    vNormal = vec3(u_normal_matrix * vec4(-a_normal_ed.x, -a_normal_ed.y, a_normal_ed.z, a_normal_ed.w));
    gl_Position = u_matrix * pos;
}