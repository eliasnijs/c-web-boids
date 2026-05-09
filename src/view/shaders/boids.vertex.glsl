#version 300 es
precision highp float;

uniform float u_window_width;
uniform float u_window_height;
uniform float u_point_size;
uniform float u_tex_size;
uniform sampler2D u_boids_tex;

out vec2 v_vel;
out vec2 v_uv;

void main() {
    int ts = int(u_tex_size);
    vec4 boid = texelFetch(u_boids_tex, ivec2(gl_VertexID % ts, gl_VertexID / ts), 0);
    vec2 pos  = boid.xy;
    v_vel     = boid.zw;

    gl_PointSize = u_point_size;
    float x = (pos.x / u_window_width)  * 2.0 - 1.0;
    float y = (pos.y / u_window_height) * 2.0 - 1.0;
    gl_Position = vec4(x, -y, 0.0, 1.0);
    v_uv = pos / vec2(u_window_width, u_window_height);
}
