#version 330 core
#ifdef GLES
precision mediump float;
#endif

layout (location=0) in vec2 position;
// layout (location=0) in vec2 coord;


// out vec2 fragCoord;

void main(){
    gl_Position = vec4(position, 0.0, 1.0);
//     fragCoord = abs(position);
}
