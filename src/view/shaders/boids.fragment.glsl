#version 300 es
precision mediump float;

in vec2 v_vel;
in vec2 v_uv;

out vec4 out_color;

uniform sampler2D tex;
uniform float u_color_mode;
uniform float u_max_vel;

vec3 hue_to_rgb(float hue) {
    float r = clamp(abs(hue * 6.0 - 3.0) - 1.0, 0.0, 1.0);
    float g = clamp(2.0 - abs(hue * 6.0 - 2.0), 0.0, 1.0);
    float b = clamp(2.0 - abs(hue * 6.0 - 4.0), 0.0, 1.0);
    return vec3(r, g, b);
}

void main() {
    float hue;
    if (u_color_mode > 0.5) {
        float t = clamp(length(v_vel) / (u_max_vel * 1.41422), 0.0, 1.0);
        hue = (1.0 - pow(t, 0.3)) * (2.0 / 3.0);
    } else {
        hue = (atan(v_vel.y, v_vel.x) + 3.14159265359) / (2.0 * 3.14159265359);
    }
    out_color = vec4(hue_to_rgb(hue), 1.0);
}
