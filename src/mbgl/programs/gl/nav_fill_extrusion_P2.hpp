
namespace nav {
namespace programs {
namespace p2 {

struct FillExtrusionProgram {
    
    static const char* navVertex(const char* ) { return R"(
    
        uniform highp mat4 u_matrix;
        uniform highp mat4 u_model_matrix;
    
        uniform highp vec3 u_camera_pos;
        uniform lowp vec3 u_lightcolor;
        uniform highp vec3 u_lightpos;

        uniform lowp float u_lightintensity;
    
        uniform lowp float u_opacity;
//        uniform lowp float u_spotlight;
        uniform float u_clip_region;
//        uniform float u_focus_region;
    
        attribute highp vec2 a_pos;
        attribute highp vec4 a_normal_ed;
    
        varying vec4 v_color;
                
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
            vec3 normal = a_normal_ed.xyz;
        
            // height
            base = max(0.0, base);
            height = max(base, height);
            float lowp t = mod(normal.x, 2.0);
    
            // position
            float lowp z = t > 0. ? height : base;
            vec4 pos = vec4(a_pos, z, 1.);
            gl_Position = u_matrix * pos;
    
            // distance clipping
            float distance = pow(gl_Position.x, 2.) + pow(gl_Position.z, 2.);
            if (distance > u_clip_region) {
                gl_Position.x = gl_Position.y = gl_Position.z = -1.e100;
                return;
            }
    
            // Ambient Lighting
            const float ambient = .9;
    
            // Diffuse Lighting
            highp vec3 norm = normalize(normal);
            highp vec3 lightDir = normalize(vec3(gl_Position) - u_lightpos);
            float diffuse = max(0., dot(norm, lightDir)) * .5;
    
            // Specular Lighting
            const float indensity = .8; // 强度
            const float shininess = 2.; // 反射率
            highp vec3 verPos = (u_model_matrix * pos).xyz;
            highp vec3 viewDir = normalize(u_camera_pos - verPos);
            highp vec3 reflectDir = reflect(lightDir, norm); // 反射向量
            float specular = indensity * pow(max(0., dot(viewDir, reflectDir)), shininess); // power(max(0,dot(N,H)),shininess)

            const highp vec3 specTone = vec3(1., .95, .7);
            highp vec3 specLight = vec3(.8) + specTone * (specular + diffuse);
    
            v_color = color * vec4(u_lightcolor * ambient * specLight, 1.) * u_opacity * .95;
        }
        
    )"; }
    
    static const char* navFragment(const char* ) { return R"(
        varying vec4 v_color;
    
        void main() {
            gl_FragColor = v_color;    
    
        #ifdef OVERDRAW_INSPECTOR
            gl_FragColor=vec4(1.0);
        #endif
        }
        
    )"; }
    
};

} // namespace gl
} // namespace programs
} // nav
