
namespace nav {
namespace programs {
namespace p2 {

struct FillExtrusionProgram {
    
    static const char* navVertex(const char* ) { return R"(
    
        uniform highp mat4 u_matrix;
        uniform highp mat4 u_model_matrix;
    
        uniform lowp vec3 u_camera_pos;
        uniform lowp vec3 u_lightcolor;
        uniform lowp vec3 u_lightpos;
        uniform lowp float u_lightintensity;
        uniform lowp float u_vertical_gradient;
    
        uniform lowp float u_opacity;
        uniform lowp float u_spotlight;
        uniform lowp float u_clip_region;
        uniform lowp float u_focus_region;
    
        attribute highp vec2 a_pos;
        attribute lowp vec4 a_normal_ed;
    
        varying lowp vec4 v_color;
        varying lowp float v_height;
                
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
            lowp vec3 normal = a_normal_ed.xyz;
        
            // height
            base = max(0.0, base);
            height = max(base, height);
            float lowp t = mod(normal.x, 2.0);
    
            // position
            float lowp z = t > 0. ? height : base;
            vec4 modelpos = vec4(a_pos, z, 1.);
            gl_Position = u_matrix * modelpos;
    
            // distance clipping
            lowp float distance = pow(gl_Position.x, 2.) + pow(gl_Position.z, 2.);
            if (distance > u_clip_region) {
                gl_Position.x = gl_Position.y = gl_Position.z = -1.e100;
                return;
            }
    
            // Ambient Lighting
            const float ambient = .7;
    
            // Diffuse Lighting
            lowp vec3 norm = normalize(normal);
            lowp vec3 lightDir = normalize(u_lightpos);
            lowp float diffuse = dot(norm, lightDir) * .5;
    
            // Specular Lighting
            const lowp float indensity = .8; // 强度
            const lowp float shininess = 2.; // 反射率
            lowp vec3 world_pos = (u_model_matrix * modelpos).xyz;
            lowp vec3 viewDir = normalize(u_camera_pos - world_pos);
            lowp vec3 reflectDir = reflect(-lightDir, norm); // reflect (genType I, genType N),返回反射向量
            lowp float specular = indensity * pow(dot(viewDir, reflectDir), shininess); // power(max(0,dot(N,H)),shininess)
    
            v_color = color * vec4(u_lightcolor * (ambient + diffuse + specular), 1.) * u_opacity;
        }
        
    )"; }
    
    static const char* navFragment(const char* ) { return R"(
    
        uniform lowp float u_focus_region;
        uniform lowp float u_spotlight;
    
        varying lowp vec4 v_color;
        varying lowp float v_height;
    
        void main() {
//            if (v_color.a > 0.) {
//                // 如果开聚光灯，屏幕中心透明
//                lowp float radial_alpha = 1.;
//                if (u_spotlight > 0.) {
//                    lowp float distance=pow(v_pos.x,2.)+pow(v_pos.z,2.);
//                    radial_alpha=clamp(distance/u_focus_region+.1,1.-u_spotlight,1.);
//                }
//
//                gl_FragColor = v_color * radial_alpha;
//            }
    
            gl_FragColor = v_color;
//            if (v_height < 1.) gl_FragColor.rgb *= pow(v_height, .2);
    
    
        #ifdef OVERDRAW_INSPECTOR
            gl_FragColor=vec4(1.0);
        #endif
        }
        
    )"; }
    
};

} // namespace gl
} // namespace programs
} // nav
