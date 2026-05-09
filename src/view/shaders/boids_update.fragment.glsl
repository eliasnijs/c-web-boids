#version 300 es
precision highp float;

uniform sampler2D u_boids_tex;
uniform float u_tex_size;
uniform float u_window_width;
uniform float u_window_height;
uniform float u_n;
uniform float u_r;
uniform float u_theta_max;
uniform float u_c;
uniform float u_s_r;
uniform float u_s;
uniform float u_a;
uniform float u_max_vel;
uniform float u_mouseG_enabled;
uniform float u_mouseG;
uniform vec2  u_mouse;

out vec4 out_boid;

bool has_influence(vec2 pos_j, vec2 vel_i, vec2 pos_i, float r, float theta_max) {
    vec2 v_rel = pos_j - pos_i;
    float dist = length(v_rel);
    if (dist >= r || dist < 0.0001) return false;
    float mag_i = length(vel_i);
    if (mag_i < 0.0001) return true;
    float cosA = dot(vel_i, v_rel) / (mag_i * dist);
    float angle = acos(clamp(cosA, -1.0, 1.0));
    return angle < theta_max || angle > (2.0 * 3.14159265) - theta_max;
}

void main() {
    int ts  = int(u_tex_size);
    int ix  = int(gl_FragCoord.x);
    int iy  = int(gl_FragCoord.y);
    int idx = iy * ts + ix;
    int n   = int(u_n);

    if (idx >= n) { out_boid = vec4(0.0); return; }

    vec4 self  = texelFetch(u_boids_tex, ivec2(ix, iy), 0);
    vec2 pos_i = self.xy;
    vec2 vel_i = self.zw;

    vec2 cohesion_v   = vec2(0.0);
    vec2 separation_v = vec2(0.0);
    vec2 alignment_v  = vec2(0.0);
    int m = 0;

    for (int j = 0; j < n; j++) {
        if (j == idx) continue;
        vec4 other = texelFetch(u_boids_tex, ivec2(j % ts, j / ts), 0);
        vec2 pos_j = other.xy;
        vec2 vel_j = other.zw;

        if (has_influence(pos_j, vel_i, pos_i, u_r, u_theta_max)) {
            cohesion_v   += pos_j;
            separation_v += (length(pos_j - pos_i) < u_s_r)
                            ? -(pos_j - pos_i) : vec2(0.0);
            alignment_v  += vel_j;
            m++;
        }
    }

    vec2 new_vel = vel_i;
    if (m > 0) {
        cohesion_v   = (cohesion_v  / float(m) - pos_i) * u_c;
        alignment_v  = (alignment_v / float(m) - vel_i) * u_a;
        separation_v *= u_s;
        new_vel += cohesion_v + separation_v + alignment_v;
    }

    if (u_mouseG_enabled > 0.5) {
        vec2 diff = u_mouse - pos_i;
        float dist = length(diff) + 0.001;
        new_vel += (diff / dist) * (u_mouseG / dist);
    }

    float speed = length(new_vel);
    if (speed > u_max_vel) {
        new_vel = (new_vel / speed) * u_max_vel;
    }

    vec2 new_pos = pos_i + new_vel;

    if (new_pos.x <= 0.0)                new_vel.x =  abs(new_vel.x);
    else if (new_pos.x > u_window_width) new_vel.x = -abs(new_vel.x);
    if (new_pos.y <= 0.0)                new_vel.y =  abs(new_vel.y);
    else if (new_pos.y > u_window_height) new_vel.y = -abs(new_vel.y);

    new_pos = pos_i + new_vel;

    out_boid = vec4(new_pos, new_vel);
}
