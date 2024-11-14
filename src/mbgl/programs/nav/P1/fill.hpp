
namespace nav {
namespace p1 {

static const char* navFragment(const char* ) { return R"(

uniform lowp float u_spotlight;
uniform lowp float u_render_time;
uniform lowp float u_water_wave;
uniform lowp float u_water_data_z_scale;
uniform lowp float u_clip_region;
uniform lowp float u_focus_region;
        
varying lowp vec3 v_pos;
varying lowp vec2 v_texture_pos;

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

// -------------- material flow shader animation ---------------
        
float spot_light(vec2 uv, vec2 C, float r, float b) {
    return clamp(.2 / clamp(length(uv-C)-r, 0., 1.), 0., b) * .2;
}

vec3 color_flow(lowp vec2 fragCoord, lowp vec2 resolution) {
    lowp float time = u_render_time * .01;
    lowp vec2 uv = (fragCoord*2. - resolution.xy) / resolution.y;
    lowp vec3 rgb = cos(time * 31. + uv.xyx + vec3(1.0,2.0,4.0)) * .2;

    lowp vec3 spots;
    for(int i = 0; i < 10; i++){
        lowp float n = float(i);
        lowp float s = sin(n * time);

        lowp float c = 2. * (n/14.-.5) * resolution.x / resolution.y;
        lowp vec2 p = vec2(c + cos(n + time * 5.), sin(time * n / 11.));

        lowp float r = abs(0.01 * s);
        lowp float b = (s * 11. + 13.) / 2.;
        
        spots += spot_light(uv, p, r, b);
    }
    
    return rgb + spots;
}

// -------------- dot dance shader animation ---------------

//created by TRASHTRASH aka Joshua deLorimier

#define iter 40.0
#define scaleSpeed 3.0
#define satSpeed 4.2

// Dave Hoskins - https://www.shadertoy.com/view/4djSRW
//noise
float N2(vec2 p) {
    vec3 p3  = fract(vec3(p.xyx) * vec3(443.897, 441.423, 437.195));
    p3 += dot(p3, p3.yzx + 19.19);
    return fract((p3.x + p3.y) * p3.z);
}

float grid_color( vec2 fragCoord, lowp vec2 resolution ) {
    float iTime = u_render_time * .2;

    //create coordinates
    vec2 uv = (fragCoord - 0.5*resolution.xy)/resolution.y;
    
    //iterate to make grid
    uv*=iter;

    //give ID's for each square
    vec2 gv=fract(uv);//-0.5; 去掉后间距变大
    vec2 id=floor(uv);

    //random values
    float ran = N2(id);

    //offset for each grid
    vec2 d = abs(gv) - (abs(sin((iTime*scaleSpeed)*ran)*0.5)-0.05);

    //draw the square
    float rect = min(max(d.x, d.y),0.0) + length(max(d, 0.));
    float r = step(0., rect);

    //combine square and offset to the color var
    return abs((1.-r) * sin((iTime*satSpeed)*ran));
}

// -------------- main ---------------

void main() {

#ifdef HAS_UNIFORM_u_color
    highp vec4 color=u_color;
#endif

#ifdef HAS_UNIFORM_u_opacity
    lowp float opacity=u_opacity;
#endif

    float distance = pow(v_pos.x, 2.) + pow(v_pos.z, 2.);

    if (u_water_wave > 0.) { // 动态水面波光

        // point light
        const lowp vec3 cameraPos=vec3(0.,500.,0.);
        const lowp vec3 lightPos=vec3(0.,2000.,4000.);
        const lowp float specular=1.5; // 镜面反射强度
        const lowp float shininess=4.; // 反光度

        lowp vec3 lightDir=normalize(v_pos-lightPos);
        lowp vec3 viewDir=normalize(cameraPos-v_pos);
        lowp vec3 reflectDir=reflect(lightDir,vec3(0.,1.,0.));
        lowp float brighten=max(specular*pow(max(dot(viewDir,reflectDir),0.0),shininess), 0.);

        float radial_fadeout = clamp(1. - distance / u_clip_region, 0., 1.);
        radial_fadeout = pow(radial_fadeout, 3.) * u_water_wave;

        const vec2 texture_size = vec2(3000.);
        vec2 coord = vec2(
            mod(v_texture_pos.x * u_water_data_z_scale, texture_size.x),
            mod(v_texture_pos.y * u_water_data_z_scale, texture_size.y));

        lowp float gridcolor = grid_color(coord, texture_size);
        gridcolor = pow(gridcolor, 15.);

        gl_FragColor = color;
        gl_FragColor.rgb += (gridcolor + brighten) * radial_fadeout * .2;
        gl_FragColor *= opacity;

    } else if (u_spotlight > 0.) { // 五彩地面

        float fadeout = clamp(1. - distance / (u_focus_region * .3), 0., u_spotlight);
        fadeout = pow(fadeout, 3.);

        vec3 colorflow = color_flow(gl_FragCoord.xy, vec2(3000.));
        gl_FragColor.rgb = mix(color.rgb, colorflow, fadeout) * opacity; // 距离屏幕中心点越近，越亮
        gl_FragColor.a = color.a * opacity;

    } else {

        gl_FragColor = color * opacity;

    }

    float radial_fadeout = min(distance / (u_clip_region * 2.), 1.) * .2;
    gl_FragColor.rgb -= radial_fadeout;
        
#ifdef OVERDRAW_INSPECTOR
    gl_FragColor=vec4(1.0);
#endif

}

)"; }

} // namespace p1
} // namespace nav
