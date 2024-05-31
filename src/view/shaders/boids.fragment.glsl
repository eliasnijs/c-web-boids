#version 300 es
precision mediump float;

in vec2 v_vel;
out vec4 out_color;

void main() {
    float r = clamp(0.3, abs(v_vel.x) / 4.0, 1.0);
    float g = 1.0;
    float b = clamp(0.3, abs(v_vel.y) / 4.0, 1.0);

    out_color = vec4(r, g, b, 1.0);
}

