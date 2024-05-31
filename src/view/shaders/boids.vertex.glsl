#version 300 es
precision mediump float;

layout(location = 0) in vec2 a_position;
layout(location = 1) in vec2 a_vel;

uniform float u_window_width;
uniform float u_window_height;
uniform float u_point_size;

out vec2 v_vel;

void main() {
	gl_PointSize = u_point_size;
  float x = (a_position.x / u_window_width) * 2.0 - 1.0;
  float y = (a_position.y / u_window_height) * 2.0 - 1.0;
  gl_Position = vec4(x, -y, 0.0, 1.0);
	v_vel = a_vel;
}
