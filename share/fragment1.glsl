#version 330 core
// #define VAL 0.0

#ifdef GL_ES
precision mediump float;
#undef VAL
// #define VAL 1.0
#endif
#define BLUR 0.001
#define PI 3.14159265358979

uniform float u_time;
uniform vec2 u_resolution;
uniform vec4 u_mouse;
uniform sampler2D u_tex_0;
uniform sampler2D u_tex_1;
// uniform vec4 u_texsize;

float circle(vec2 uv, vec2 position, float radius, float sig){
    float k = length(uv - position) - radius;
    return smoothstep(BLUR, -BLUR, sig*k);
}

void main(){
  vec2 uv = (2.0*gl_FragCoord.xy - u_resolution)/u_resolution.x;
  vec3 color = vec3(0.0);
  float ratio = u_resolution.y/u_resolution.x;
  ivec2 isize1 = textureSize(u_tex_0, 0);
  ivec2 isize2 = textureSize(u_tex_1, 0);
  vec2 s1 = vec2(float(isize1.x),float(isize1.y));
  vec2 s2 = vec2(float(isize2.x),float(isize2.y));
  float diag = 2.0*length(vec2(1.0,ratio));
  vec2 off1 = (u_resolution - s1)/2.0;
  vec2 off2 = (u_resolution - s2)/2.0;
  vec3 tex2 = texture(u_tex_0, (gl_FragCoord.xy - off1)/s1).xyz;
  vec3 tex1 = texture(u_tex_1, (gl_FragCoord.xy - off2)/s2).xyz;
  float time = clamp((u_time - u_mouse.z)/2.0, 0.0, 1.0);
  time = (time-0.5)*PI;
  float func = diag*(1.0+sin(time))/2.0;
  float alpha = circle(uv, (2.0*u_mouse.xy - u_resolution)/u_resolution.x, func, 2.0*(u_mouse.w-0.5));
  color += tex1*alpha + tex2*(1.0 - alpha);
  gl_FragColor = vec4(color,  1.0);
}
