
namespace nav {
namespace p2 {

static const char* navFragment(const char* ) { return R"(
    
uniform lowp float u_zoom;
uniform lowp float u_device_pixel_ratio;
uniform lowp float u_spotlight;
uniform lowp float u_focus_region;

varying lowp vec2 v_width2;
varying lowp vec2 v_normal;
varying lowp float v_gamma_scale;
varying vec3 v_pos;
varying vec4 v_color;
varying vec4 v_traffic_color;
    
#ifndef HAS_UNIFORM_u_color
varying highp vec4 color;
#else
uniform highp vec4 u_color;
#endif

#ifndef HAS_UNIFORM_u_blur
varying lowp float blur;
#else
uniform lowp float u_blur;
#endif

#ifndef HAS_UNIFORM_u_opacity
varying lowp float opacity;
#else
uniform lowp float u_opacity;
#endif

void main() {
#ifdef HAS_UNIFORM_u_color
    highp vec4 color=u_color;
#else
    highp vec4 color;
#endif

#ifdef HAS_UNIFORM_u_blur
    lowp float blur=u_blur;
#endif

#ifdef HAS_UNIFORM_u_opacity
    lowp float opacity=u_opacity;
#endif
    
    color = v_color;

    // draw line
    lowp float dist = length(v_normal) * v_width2.s;
    lowp float blur2 = (blur + 1.0 / u_device_pixel_ratio) * v_gamma_scale;
    lowp float alpha = clamp(min(dist - (v_width2.t - blur2), v_width2.s - dist) / blur2, 0.0, 1.0);

    const float smooth = 2.; // 中心向边缘扩散平滑度
    const float width = 2.; // 越大越细
    float tc = min(pow(dist / v_width2.s * width, smooth), 1.); // 根据距离中心距离，计算颜色
    gl_FragColor = color * (alpha * opacity) * tc + v_traffic_color * (1.2 - tc);

#ifdef OVERDRAW_INSPECTOR
    gl_FragColor=vec4(1.0);
#endif
}

)"; }

} // namespace p2
} // namespace nav

