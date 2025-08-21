
namespace nav {
namespace p2 {

static const char* navVertex(const char* ) { return R"(

uniform mat4 u_matrix;
uniform mat4 u_model_matrix;
uniform mat4 u_model_matrix_p20;
uniform lowp float u_base;

uniform lowp vec4 u_palette_color;
uniform lowp float u_palette_lightness;

uniform vec2 u_texsize;
uniform float u_textype;

uniform sampler2D u_image;
uniform sampler2D u_image0;
uniform sampler2D u_image1;
        
attribute vec2 a_pos;

varying vec3 v_world_pos;
varying vec2 v_uv;

#ifndef HAS_UNIFORM_u_color
    uniform lowp float u_color_t;
    attribute vec4 a_color;
    varying vec4 color;
#else
    uniform vec4 u_color;
#endif

#ifndef HAS_UNIFORM_u_opacity
    uniform lowp float u_opacity_t;
    attribute lowp vec2 a_opacity;
    varying lowp float opacity;
#else
    uniform lowp float u_opacity;
#endif

#ifndef HAS_UNIFORM_u_gapwidth
    uniform lowp float u_gapwidth_t;
    attribute mediump vec2 a_gapwidth;
#else
    uniform mediump float u_gapwidth;
#endif

#ifndef HAS_UNIFORM_u_offset
    uniform lowp float u_offset_t;
    attribute lowp vec2 a_offset;
#else
    uniform lowp float u_offset;
#endif

#ifndef HAS_UNIFORM_u_width
    uniform lowp float u_width_t;
    attribute mediump vec2 a_width;
#else
    uniform mediump float u_width;
#endif
        
void main() {
#ifndef HAS_UNIFORM_u_color
    color=unpack_mix_color(a_color,u_color_t);
#else
    highp vec4 color=u_color;
#endif

#ifndef HAS_UNIFORM_u_opacity
    opacity=unpack_mix_vec2(a_opacity,u_opacity_t);
#else
    lowp float opacity=u_opacity;
#endif

#ifndef HAS_UNIFORM_u_gapwidth
    mediump float gapwidth=unpack_mix_vec2(a_gapwidth,u_gapwidth_t);
#else
    mediump float gapwidth=u_gapwidth;
#endif

#ifndef HAS_UNIFORM_u_offset
    lowp float offset=unpack_mix_vec2(a_offset,u_offset_t);
#else
    lowp float offset=u_offset;
#endif

#ifndef HAS_UNIFORM_u_width
    mediump float width=unpack_mix_vec2(a_width,u_width_t);
#else
    mediump float width=u_width;
#endif
    
    vec4 pos = vec4(a_pos, u_base, 1.);
    gl_Position = u_matrix * pos;

    v_world_pos = vec3(u_model_matrix * pos);

    const float scaleFactor = 1. / 1.;
    v_uv = vec2(u_model_matrix_p20 * pos);
    v_uv = fract(v_uv / 102400.) * 102400.;
    v_uv /= u_texsize * scaleFactor;

#ifndef HAS_UNIFORM_u_color
    // 灰阶色变换主题色
    if (u_palette_lightness > 0.) {
        lowp float lightness = color.r / u_palette_lightness;
        color = vec4(u_palette_color.rgb * lightness, color.a);
    }
#endif

}

)"; }
 
static const char* navFragment(const char* ) { return R"(
    
uniform float u_textype;    // 纹理类型
uniform vec3 u_camera_pos;  // 主相机位置
uniform vec3 u_lightcolor;  // 平行光色
uniform vec3 u_lightpos;    // 平行光位置
uniform float u_tex_viability;  // 贴图留存系数

uniform sampler2D u_image;  // 纹理图-diffuse
uniform sampler2D u_image0; // 纹理图-normal
uniform sampler2D u_image1; // 纹理图-reflection

varying vec3 v_world_pos;
varying vec2 v_uv;

#ifndef HAS_UNIFORM_u_color
    varying vec4 color;
#else
    uniform vec4 u_color;
#endif

#ifndef HAS_UNIFORM_u_opacity
    varying lowp float opacity;
#else
    uniform lowp float u_opacity;
#endif

// -------------- main ---------------

void main() {

#ifdef HAS_UNIFORM_u_color
    vec4 color=u_color;
#endif

#ifdef HAS_UNIFORM_u_opacity
    lowp float opacity=u_opacity;
#endif

    if (u_textype > 0.) {
        if (u_textype == 1.) { // 水

            const float Material_ambient = .8; // 环境光
            const float Material_diffuse = .2; // 漫反射
            const float Material_specular = .1; // 镜面反射
            const float Material_shininess = 5.; // 镜面反射率

            // Ambient Lighting
            const float ambient = Material_ambient;

            // Diffuse Lighting
            vec3 norm = texture2D(u_image0, v_uv).rgb;
            norm = normalize(norm * 2.0 - 1.0); // z > 0.
            vec3 lightDir = normalize(u_lightpos);
            float diffuse = max(dot(norm, lightDir), 0.) * Material_diffuse;

            // Specular Lighting
            vec3 viewDir = normalize(u_camera_pos - v_world_pos);
            vec3 reflectDir = reflect(-lightDir, norm);
            float contast_dot = (max(dot(viewDir, reflectDir), 0.) - .5) * 2. + .5;
            float specular = pow(contast_dot, Material_shininess) * Material_specular;
            specular = clamp(specular, 0., 1.);

            vec3 baselight = color.rgb * (ambient + diffuse) * (1. - specular);
            vec3 specularlight = vec3(.92, .97, .97) * specular;

            gl_FragColor = vec4(baselight + specularlight, 1.) * opacity;
     
        } else if (u_textype == 2.) { // 草

            const float Material_ambient = .5; // 环境光
            const float Material_diffuse = .5; // 漫反射

            vec4 tex_color = texture2D(u_image, v_uv);

            // Ambient Lighting
            const float ambient = Material_ambient;

            // Diffuse Lighting
            vec3 norm = texture2D(u_image0, v_uv).rgb;
            norm = normalize(norm * 2.0 - 1.0);
            vec3 lightDir = normalize(u_lightpos);
            float diffuse = max(dot(norm, lightDir), 0.0) * Material_diffuse;

            gl_FragColor = vec4(tex_color.rgb * (ambient + diffuse), .5) * opacity;

        } else {

            gl_FragColor = color * opacity;

        }

        gl_FragColor = (gl_FragColor * u_tex_viability + color * (1.0 - u_tex_viability)) * opacity;

    } else {

        gl_FragColor = color * opacity;

    }
    
#ifdef OVERDRAW_INSPECTOR
    gl_FragColor=vec4(1.0);
#endif

}

)"; }

} // namespace p2
} // namespace nav
