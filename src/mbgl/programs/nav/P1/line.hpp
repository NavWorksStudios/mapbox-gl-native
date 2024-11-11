
namespace nav {
namespace p1 {

static const char* navFragment(const char* ) { return R"(

uniform lowp float u_zoom;
uniform lowp float u_device_pixel_ratio;
uniform lowp float u_spotlight;
uniform lowp float u_focus_region;

varying lowp vec2 v_width2;
varying lowp vec2 v_normal;
varying lowp float v_gamma_scale;
varying vec3 v_pos;
varying mediump float v_condition;
varying highp vec4 v_condi_color;
    
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
    
    color = v_condi_color;

    // 中心，亮 [1, 0]
    lowp float radius = u_focus_region * .4 * (1. - .8*u_spotlight); // 开灯，光圈缩小
    lowp float distance = pow(v_pos.x, 2.) + pow(v_pos.y, 2.);
    lowp float radial_fadeout = 1.4 - min(distance / radius, 1.);

    // zoom小，亮 [1.2, 1.44]
    lowp float brightness = 1. + (20. - u_zoom) * .01;
    color.rgb *= (brightness + u_spotlight * .5) * radial_fadeout; // 开灯，提亮

    // 远处，透
    radius = u_focus_region * 20.;
    distance = pow(v_pos.x, 2.) + pow(v_pos.z, 2.);
    radial_fadeout = 1. - min(distance / radius, 1.);
    color *= radial_fadeout;

    // draw line
    lowp float dist=length(v_normal)*v_width2.s;
    lowp float blur2=(blur+1.0/u_device_pixel_ratio)*v_gamma_scale;
    lowp float alpha=clamp(min(dist-(v_width2.t-blur2),v_width2.s-dist)/blur2,0.0,1.0);
    gl_FragColor=color*(alpha*opacity);

#ifdef OVERDRAW_INSPECTOR
    gl_FragColor=vec4(1.0);
#endif
}

)"; }

} // namespace p1
} // namespace nav

