// #version 330
// #define VAL 0.0
#ifdef GL_ES
precision mediump float;
#undef VAL
// #define VAL 1.0
#endif
#define BLUR 0.01
uniform float u_time;
uniform vec2 u_resolution;

float circle(vec2 uv, vec2 position, float radius, float sig){
    float k = length(uv - position) - radius;
    return smoothstep(BLUR, -BLUR, sig*k);
}

void main(){
  vec2 uv = (2.0*gl_FragCoord.xy - u_resolution)/u_resolution.x;
    vec3 color = vec3(0.0);
    float time = u_time;
    float si = 2.0*mod(floor(time), 2.0) - 1.0;
    time = 2.0*fract(time);
    color += circle(uv, vec2(0.0), time, si);
    gl_FragColor = vec4(color,  1.0);
}
