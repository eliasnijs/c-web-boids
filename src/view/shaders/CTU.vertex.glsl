#version 300 es
precision mediump float;

layout (location = 0) in vec2 aPos;

uniform vec2 u_origin;
uniform float u_size;
uniform vec2 u_window_size;

out vec2 vTexCoord;

void main() {
    vec2 pos = aPos * u_size + u_origin;
    vec2 norm_pos = (pos / u_window_size) * 2.0 - 1.0;
    norm_pos.y = -norm_pos.y;
    gl_Position = vec4(norm_pos, 0.0, 1.0);

    vTexCoord = aPos; // This will be (0,0) to (1,1) across the quad
}
