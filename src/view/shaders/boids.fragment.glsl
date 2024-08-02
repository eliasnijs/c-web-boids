#version 300 es
precision mediump float;

in vec2 v_vel;
in vec2 v_uv;

out vec4 out_color;

uniform sampler2D tex;

void main() {
    float hue = (atan(v_vel.y, v_vel.x) + 3.14159265359) / (2.0 * 3.14159265359);
    float r = abs(hue * 6.0 - 3.0) - 1.0;
    float g = 2.0 - abs(hue * 6.0 - 2.0);
    float b = 2.0 - abs(hue * 6.0 - 4.0);

    out_color = mix(texture(tex, v_uv), vec4(r, g, b, 1.0), 0.5);
}
