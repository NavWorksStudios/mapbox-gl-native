
namespace nav {
namespace p1 {

static const char* navVertex(const char* ) { return R"(

uniform mat4 u_matrix;
uniform mat4 u_model_matrix;
uniform mat4 u_model_matrix_p20;
uniform float u_base;

uniform vec4 u_palette_color;
uniform float u_palette_lightness;

uniform vec2 u_texsize;
uniform float u_textype;

uniform sampler2D u_image;
uniform sampler2D u_image0;
uniform sampler2D u_image1;
        
attribute vec2 a_pos;

varying vec3 v_pos;
varying vec2 v_texture_pos;

#ifndef HAS_UNIFORM_u_color
    uniform float u_color_t;
    attribute vec4 a_color;
    varying vec4 color;

#else
    uniform vec4 u_color;
#endif

#ifndef HAS_UNIFORM_u_opacity
    uniform float u_opacity_t;
    attribute vec2 a_opacity;
    varying float opacity;
#else
    uniform float u_opacity;
#endif

#ifndef HAS_UNIFORM_u_gapwidth
    uniform float u_gapwidth_t;
    attribute mediump vec2 a_gapwidth;
#else
    uniform mediump float u_gapwidth;
#endif

#ifndef HAS_UNIFORM_u_offset
    uniform float u_offset_t;
    attribute vec2 a_offset;
#else
    uniform float u_offset;
#endif

#ifndef HAS_UNIFORM_u_width
    uniform float u_width_t;
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
    float opacity=u_opacity;
#endif

#ifndef HAS_UNIFORM_u_gapwidth
    mediump float gapwidth=unpack_mix_vec2(a_gapwidth,u_gapwidth_t);
#else
    mediump float gapwidth=u_gapwidth;
#endif

#ifndef HAS_UNIFORM_u_offset
    float offset=unpack_mix_vec2(a_offset,u_offset_t);
#else
    float offset=u_offset;
#endif

#ifndef HAS_UNIFORM_u_width
    mediump float width=unpack_mix_vec2(a_width,u_width_t);
#else
    mediump float width=u_width;
#endif
    
    vec4 pos = vec4(a_pos, u_base, 1.);
    gl_Position = u_matrix * pos;

    v_pos = gl_Position.xyz;
    v_texture_pos=a_pos;

#ifndef HAS_UNIFORM_u_color
    // 灰阶色变换主题色
    if (u_palette_lightness > 0.) {
        float lightness = color.r / u_palette_lightness;
        color = vec4(u_palette_color.rgb * lightness, color.a);
    }
#endif

}

)"; }


static const char* navFragment(const char* ) { return R"(

uniform float u_spotlight;
uniform highp float u_render_time;
uniform float u_water_wave;
uniform float u_water_data_z_scale;
uniform float u_clip_region;
uniform float u_focus_region;
        
varying vec3 v_pos;
varying vec2 v_texture_pos;

#ifndef HAS_UNIFORM_u_color
    varying highp vec4 color;
#else
    uniform highp vec4 u_color;
#endif

#ifndef HAS_UNIFORM_u_opacity
    varying float opacity;
#else
    uniform float u_opacity;
#endif

// -------------- material flow shader animation ---------------
        
vec2 matcap(vec3 eye, vec3 normal) {
    vec3 reflected = reflect(eye, normal);
    float m = 2.8284271247461903 * sqrt( reflected.z+1.0 );
    return reflected.xy / m + 0.5;
}
        
float bubble(vec2 uv, vec2 C, float r, float b) {
    return clamp(1./clamp(length(uv-C)-r,0.,1.)/5.,0.,b);
}
        
vec3 color_flow(vec2 fragCoord) {
    const vec2 resolution = vec2(1920, 720);
    float time = u_render_time / 100. + 123.;
    vec2 uv = (2. * fragCoord - resolution.xy) / resolution.y;
    vec3 col = 0.15 * cos(time*31.+uv.xyx+vec3(1.0,2.0,4.0));

    vec3 bubbles = vec3(0);
    for(int i = 0; i < 10; i++){
        float n = float(i);
        float c = 2.*(n/14.-.5)*resolution.x/resolution.y;
        vec2 p = vec2(c+cos(n+time*5.),sin(time*n/11.));
        float r = 0.01*abs(sin(n*time));
        float b = (sin(time*n)*11.+13.)/2.;
        
        bubbles += bubble(uv,p,r,b)/10.;
    }
    
    return col+bubbles;
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

float grid_color( vec2 fragCoord, vec2 resolution ) {
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
    float opacity=u_opacity;
#endif

    float distance=pow(v_pos.x,2.)+pow(v_pos.z,2.);

    if (u_water_wave > 0.) { // 水面波光

        // point light
        const vec3 cameraPos=vec3(0.,500.,0.);
        const vec3 lightPos=vec3(0.,2000.,4000.);
        const float specular=1.5; // 镜面反射强度
        const float shininess=4.; // 反光度
        vec3 lightDir=normalize(v_pos.xyz-lightPos);
        vec3 viewDir=normalize(cameraPos-v_pos.xyz);
        vec3 reflectDir=reflect(lightDir,vec3(0.,1.,0.)); // reflect (genType I, genType N),返回反射向量
        float brighten=max(specular*pow(max(dot(viewDir,reflectDir),0.0),shininess), 0.); // power(max(0,dot(N,H)),shininess)

        float radial_fadeout=clamp(1.-distance/u_clip_region,0.,1.);
        radial_fadeout=pow(radial_fadeout,3.) * u_water_wave;

        const vec2 texture_size = vec2(8000.);
        vec2 coord = vec2(
            mod(v_texture_pos.x * u_water_data_z_scale, texture_size.x),
            mod(v_texture_pos.y * u_water_data_z_scale, texture_size.y));
        float gridcolor = grid_color(coord, texture_size);
        gridcolor = pow(gridcolor, 15.);

        gl_FragColor=color;
        gl_FragColor.rgb += (gridcolor + brighten) * radial_fadeout * .2;
        gl_FragColor*=opacity;

    } else {

        if (u_spotlight > 0.) { // 五彩地面

            const float radius = 1500000.;
            float distance = pow(v_pos.x,2.) + pow(v_pos.y,2.);
            float centerFactor = clamp(distance/radius, 1.-u_spotlight, 1.);
            centerFactor = pow(1. - centerFactor, 3.);

            gl_FragColor.rgb = mix(color.rgb, color_flow(gl_FragCoord.xy) * 1.5, centerFactor); // 距离屏幕中心点越近，越亮
            gl_FragColor.a = color.a * opacity;

        } else {

            gl_FragColor=color*opacity;

        }

    }

    float radial_fadeout=min(distance/(u_clip_region*2.),1.)*.2;
    gl_FragColor.rgb-=radial_fadeout;
        
#ifdef OVERDRAW_INSPECTOR
    gl_FragColor=vec4(1.0);
#endif

}

)"; }

} // namespace p1
} // namespace nav
