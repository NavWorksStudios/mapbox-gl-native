
namespace nav {
namespace programs {
namespace p1 {

struct FillExtrusionProgram {
    
static const char* navVertex(const char* ) { return R"(

uniform highp mat4 u_matrix;
uniform lowp vec3 u_lightcolor;
uniform lowp vec3 u_lightpos;
uniform lowp float u_lightintensity;
uniform lowp float u_vertical_gradient;
uniform lowp float u_opacity;
uniform lowp float u_spotlight;
uniform bool u_is_reflection;
uniform lowp float u_clip_region;
uniform lowp float u_focus_region;

attribute highp vec2 a_pos;
attribute lowp vec4 a_normal_ed;

varying lowp vec4 v_color;
varying lowp vec3 v_pos;
varying lowp float v_top_edge;
varying lowp float v_bottom_edge;
varying lowp float v_height;
varying lowp float v_specular;
        
#ifndef HAS_UNIFORM_u_base
uniform lowp float u_base_t;
attribute highp vec2 a_base;
#else
uniform highp float u_base;
#endif

#ifndef HAS_UNIFORM_u_height
uniform lowp float u_height_t;
attribute highp vec2 a_height;
#else
uniform highp float u_height;
#endif

#ifndef HAS_UNIFORM_u_color
uniform lowp float u_color_t;
attribute highp vec4 a_color;
#else
uniform highp vec4 u_color;
#endif

void main() {
#ifndef HAS_UNIFORM_u_base
    highp float base=unpack_mix_vec2(a_base,u_base_t);
#else
    highp float base=u_base;
#endif

#ifndef HAS_UNIFORM_u_height
    highp float height=unpack_mix_vec2(a_height,u_height_t);
#else
    highp float height=u_height;
#endif

#ifndef HAS_UNIFORM_u_color
    highp vec4 color=unpack_mix_color(a_color,u_color_t);
#else
    highp vec4 color=u_color;
#endif

    // ----------------------------- vertex position -----------------------------

    // normal
    lowp vec3 normal=a_normal_ed.xyz;

    // height
    //base=max(0.0,base);
    height = max(base, height);
    float lowp t = mod(normal.x, 2.0);

    // position
    float lowp z = (t > 0.) ? height : base;
    if (u_is_reflection) z = -z;
    gl_Position = u_matrix * vec4(a_pos, z, 1.);
    v_pos = gl_Position.xyz;

    // clipping
    lowp float distance = pow(gl_Position.x, 2.) + pow(gl_Position.z, 2.);
    if (u_is_reflection) distance *= 5.;
    if (distance > u_clip_region) {
        gl_Position.x = gl_Position.y = gl_Position.z = -1.e100;
        return;
    }

    // ----------------------------- illumination color -----------------------------

    // directional light
    float colorvalue = color.r*0.2126 + color.g*0.7152 + color.b*0.0722;
    float directional = clamp(dot(normal/16384.0, u_lightpos), 0.0, 1.0);
    directional = mix((1.0-u_lightintensity), max((1.0-colorvalue+u_lightintensity),1.0), directional);
    if (normal.y > 0.0) { // side surface vertical gradient
        float vertical_factor = clamp((height+base) * pow(height/150.0,0.5), mix(0.7,0.98,1.0-u_lightintensity), 1.0);
        directional *= ((1.0 - u_vertical_gradient) + (u_vertical_gradient * vertical_factor));
    }
    directional = directional * .6 + .4;

    // ambient light
    vec4 ambientlight = vec4(0.03, 0.03, 0.03, 1.0);
    color += ambientlight;

    // mix color
    v_color.r = clamp(color.r * directional * u_lightcolor.r, 0.3 * (1.0-u_lightcolor.r), 1.);
    v_color.g = clamp(color.g * directional * u_lightcolor.g, 0.3 * (1.0-u_lightcolor.g), 1.);
    v_color.b = clamp(color.b * directional * u_lightcolor.b, 0.3 * (1.0-u_lightcolor.b), 1.);
    v_color.a = 1.;

    // 远近透明
    lowp float radial_fadeout=clamp((1.-distance/u_clip_region) * 2., 0., 1.);
    v_color *= u_opacity * radial_fadeout * (u_is_reflection ? .2 : .9);

    // ----------------------------- building detail -----------------------------

    // 建筑物上下边缘渐变描边增亮
    //
    // |-----| 1.0
    // |     | 0.0
    // |     |
    // |     |
    // |     | 0.0
    // |_____| 1.0
    //
    lowp float tall = height - base;
    v_top_edge = 1. - min(300./tall, 1.2);
    v_bottom_edge = min(20./tall, .6);
    v_height = (t > 0.) ? 1. : 0.;

    // specular
    const lowp float indencity = 1.5; // 镜面反射强度
    const lowp float shininess = 1.; // 反光度
    const lowp vec3 cameraPos = vec3(0., 500., 0.);
    const lowp vec3 lightPos = vec3(0., 1000., 2000.);
    lowp vec3 lightDir = normalize(gl_Position.xyz - lightPos);
    lowp vec3 viewDir = normalize(cameraPos - gl_Position.xyz);
    lowp vec3 reflectDir = reflect(lightDir, vec3(0.,1.,0.)); // reflect (genType I, genType N),返回反射向量
    v_specular = indencity * pow(max(dot(viewDir, reflectDir), 0.0), shininess); // power(max(0,dot(N,H)),shininess)
}

)"; }
    
static const char* navFragment(const char* ) { return R"(

uniform lowp float u_focus_region;
uniform lowp float u_spotlight;

varying lowp vec4 v_color;
varying lowp vec3 v_pos;
varying lowp float v_top_edge;
varying lowp float v_bottom_edge;
varying lowp float v_height;
varying lowp float v_specular;

void main() {
    if (v_color.a > 0.) {
        lowp float brighten = v_specular; // 楼顶 镜面反射
        if (v_height < 1.) { // 楼体发光
            brighten = max(
                (v_height - v_top_edge) / (1. - v_top_edge), // 上边缘
                (1.-v_height / v_bottom_edge) // 下边缘
            );
        }

        brighten = pow(brighten,3.);

        // 如果开聚光灯，屏幕中心透明
        lowp float radial_alpha = 1.;
        if (u_spotlight > 0.) {
            lowp float distance=pow(v_pos.x,2.)+pow(v_pos.z,2.);
            radial_alpha=clamp(distance/u_focus_region+.1,1.-u_spotlight,1.);
        }

        gl_FragColor.rgb = v_color.rgb * (.8 + brighten);
        gl_FragColor.a = v_color.a;
        gl_FragColor *= radial_alpha;
    }

#ifdef OVERDRAW_INSPECTOR
    gl_FragColor=vec4(1.0);
#endif
}

)"; }
    
};

} // namespace gl
} // namespace programs
} // nav

