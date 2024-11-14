
namespace nav {
namespace p2 {
 
static const char* navFragment(const char* ) { return R"(

uniform lowp float u_spotlight;
uniform lowp float u_render_time;
uniform lowp float u_water_wave;
uniform lowp float u_water_data_z_scale;
uniform lowp float u_clip_region;
uniform lowp float u_focus_region;
    
uniform float u_textype;    // 纹理类型
uniform vec2 u_texsize;     // 纹理图尺寸:宽高
uniform vec3 u_camera_pos;  // 主相机位置
uniform vec3 u_lightcolor;  // 平行光色
uniform vec3 u_lightpos;    // 平行光位置

uniform sampler2D u_image;  // 纹理图-diffuse
uniform sampler2D u_image0; // 纹理图-normal
uniform sampler2D u_image1; // 纹理图-reflection
        
varying lowp vec3 v_pos;
varying lowp vec3 v_world_pos;
varying lowp vec2 v_texture_pos;
varying highp vec4 v_world_pixel_coord;

#ifndef HAS_UNIFORM_u_color
    varying highp vec4 color;
#else
    uniform highp vec4 u_color;
#endif

#ifndef HAS_UNIFORM_u_opacity
    varying lowp float opacity;
#else
    uniform lowp float u_opacity;
#endif

// -------------- main ---------------

void main() {

#ifdef HAS_UNIFORM_u_color
    highp vec4 color=u_color;
#endif

#ifdef HAS_UNIFORM_u_opacity
    lowp float opacity=u_opacity;
#endif

    if(u_textype > 0.) {

        // texture uv
        vec2 uv = vec2(
            mod(v_world_pixel_coord.x, u_texsize[0]) / u_texsize[0],
            mod(v_world_pixel_coord.y, u_texsize[1]) / u_texsize[1]
        );

        vec3 tex_color = texture2D(u_image, uv).rgb;

        // diffuse
        vec3 norm = texture2D(u_image0, uv).rgb;
        norm = normalize(norm * 2.0 - 1.0);
        vec3 lightDir = normalize(u_lightpos);
        tex_color = tex_color * max(dot(norm, lightDir), 0.0);

        // specular
        if(u_textype > 2.) {
            vec3 specular = texture2D(u_image1, uv).rgb;
            vec3 viewDir = normalize(v_world_pos - u_camera_pos);
            vec3 reflectDir = reflect(-lightDir, norm);
            tex_color += specular * pow(max(dot(viewDir, reflectDir), 0.0), 2.);
        }

        gl_FragColor = vec4(tex_color, 1.0) * opacity;
 
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
