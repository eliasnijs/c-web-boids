
// next power-of-two >= ceil(sqrt(MAX_BOIDS))
#define BOIDS_TEX_SIZE 256

typedef struct gpu_context_t GpuContext;
struct gpu_context_t {
	uint32 render_program;
	uint32 update_program;
	Texture sprite_tex;

	// ping-pong boid state textures: RGBA32F, BOIDS_TEX_SIZE x BOIDS_TEX_SIZE
	// boid i at texel (i % BOIDS_TEX_SIZE, i / BOIDS_TEX_SIZE)
	// each texel = (pos.x, pos.y, vel.x, vel.y)
	uint32 boids_tex[2];
	uint32 fbo[2];

	// empty VAO for attribute-less draws
	uint32 empty_vao;

	int32 current;
};

internal bool
gpu_init(GpuContext *gpu, BoidsApplication *app) {
	// --- render program ---
	if (!load_glprogram(
	        "src/view/shaders/boids.vertex.glsl",
	        "src/view/shaders/boids.fragment.glsl",
	        &gpu->render_program)) {
		print_error("Failed to load render shader program");
		return false;
	}
	glUseProgram(gpu->render_program);
	glUniform1i(glGetUniformLocation(gpu->render_program, "tex"),         0);
	glUniform1i(glGetUniformLocation(gpu->render_program, "u_boids_tex"), 1);

	// --- update program ---
	if (!load_glprogram(
	        "src/view/shaders/boids_update.vertex.glsl",
	        "src/view/shaders/boids_update.fragment.glsl",
	        &gpu->update_program)) {
		print_error("Failed to load update shader program");
		return false;
	}
	glUseProgram(gpu->update_program);
	glUniform1i(glGetUniformLocation(gpu->update_program, "u_boids_tex"), 0);

	// --- sprite texture ---
	if (!load_texture("res/texture.jpeg", &gpu->sprite_tex)) {
		print_error("failed to load texture");
		return false;
	}

	// --- boid state textures + FBOs ---
	// pack initial boid data: each texel = (pos.x, pos.y, vel.x, vel.y)
	float32 *init_data = (float32 *)calloc(BOIDS_TEX_SIZE * BOIDS_TEX_SIZE * 4, sizeof(float32));
	for (int i = 0; i < app->n; ++i) {
		init_data[i*4+0] = app->bs[i].pos.x;
		init_data[i*4+1] = app->bs[i].pos.y;
		init_data[i*4+2] = app->bs[i].vel.x;
		init_data[i*4+3] = app->bs[i].vel.y;
	}

	glGenTextures(2, gpu->boids_tex);
	glGenFramebuffers(2, gpu->fbo);
	for (int i = 0; i < 2; ++i) {
		glBindTexture(GL_TEXTURE_2D, gpu->boids_tex[i]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, BOIDS_TEX_SIZE, BOIDS_TEX_SIZE, 0,
		             GL_RGBA, GL_FLOAT, i == 0 ? init_data : NULL);

		glBindFramebuffer(GL_FRAMEBUFFER, gpu->fbo[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
		                       GL_TEXTURE_2D, gpu->boids_tex[i], 0);
	}
	free(init_data);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// --- empty VAO for attribute-less draws ---
	glGenVertexArrays(1, &gpu->empty_vao);

	gpu->current = 0;
	return true;
}

internal void
gpu_die(GpuContext *gpu) {
	texture_die(&gpu->sprite_tex);
	glDeleteTextures(2, gpu->boids_tex);
	glDeleteFramebuffers(2, gpu->fbo);
	glDeleteVertexArrays(1, &gpu->empty_vao);
	glprogram_die(gpu->render_program);
	glprogram_die(gpu->update_program);
}

internal void
render(GpuContext *gpu, BoidsApplication *app, vec2 mouse, float time) {
	int32 cur  = gpu->current;
	int32 next = 1 - cur;
	Param *p   = &app->p;

	// --- Update pass: render to next boid texture ---
	glBindFramebuffer(GL_FRAMEBUFFER, gpu->fbo[next]);
	glViewport(0, 0, BOIDS_TEX_SIZE, BOIDS_TEX_SIZE);

	glUseProgram(gpu->update_program);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gpu->boids_tex[cur]);

	glUniform1f(glGetUniformLocation(gpu->update_program, "u_window_width"),   window_width);
	glUniform1f(glGetUniformLocation(gpu->update_program, "u_window_height"),  window_height);
	glUniform1f(glGetUniformLocation(gpu->update_program, "u_n"),              (float)app->n);
	glUniform1f(glGetUniformLocation(gpu->update_program, "u_tex_size"),       (float)BOIDS_TEX_SIZE);
	glUniform1f(glGetUniformLocation(gpu->update_program, "u_r"),              p->r);
	glUniform1f(glGetUniformLocation(gpu->update_program, "u_theta_max"),      p->theta_max);
	glUniform1f(glGetUniformLocation(gpu->update_program, "u_c"),              p->c);
	glUniform1f(glGetUniformLocation(gpu->update_program, "u_s_r"),            p->s_r);
	glUniform1f(glGetUniformLocation(gpu->update_program, "u_s"),              p->s);
	glUniform1f(glGetUniformLocation(gpu->update_program, "u_a"),              p->a);
	glUniform1f(glGetUniformLocation(gpu->update_program, "u_max_vel"),        p->max_vel);
	glUniform1f(glGetUniformLocation(gpu->update_program, "u_mouseG_enabled"), (float)p->mouseG_enabled);
	glUniform1f(glGetUniformLocation(gpu->update_program, "u_mouseG"),         p->mouseG);
	glUniform2f(glGetUniformLocation(gpu->update_program, "u_mouse"),          mouse.x, mouse.y);
	glUniform1f(glGetUniformLocation(gpu->update_program, "u_cavity_enabled"), (float)p->cavity_enabled);
	glUniform2f(glGetUniformLocation(gpu->update_program, "u_cavity_center"),  p->cavity_x, p->cavity_y);
	glUniform1f(glGetUniformLocation(gpu->update_program, "u_cavity_r"),       p->cavity_r);
	glUniform1f(glGetUniformLocation(gpu->update_program, "u_cavity_strength"), p->cavity_strength);
	glUniform1f(glGetUniformLocation(gpu->update_program, "u_cavity_n"),         (float)p->cavity_n);
	glUniform1f(glGetUniformLocation(gpu->update_program, "u_cavity_shape"),     (float)p->cavity_shape);
	glUniform1f(glGetUniformLocation(gpu->update_program, "u_cavity_amplitude"), p->cavity_amplitude);
	glUniform1f(glGetUniformLocation(gpu->update_program, "u_cavity_frequency"), p->cavity_frequency);
	glUniform1f(glGetUniformLocation(gpu->update_program, "u_time"),             time);

	glBindVertexArray(gpu->empty_vao);
	glDrawArrays(GL_TRIANGLES, 0, 3);

	// --- Render pass: draw to screen ---
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, window_width, window_height);

	glUseProgram(gpu->render_program);
	glActiveTexture(GL_TEXTURE0);
	use_texture(&gpu->sprite_tex, 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, gpu->boids_tex[next]);

	glUniform1f(glGetUniformLocation(gpu->render_program, "u_window_width"),  window_width);
	glUniform1f(glGetUniformLocation(gpu->render_program, "u_window_height"), window_height);
	glUniform1f(glGetUniformLocation(gpu->render_program, "u_point_size"),    p->size);
	glUniform1f(glGetUniformLocation(gpu->render_program, "u_color_mode"),    (float)p->color_mode);
	glUniform1f(glGetUniformLocation(gpu->render_program, "u_max_vel"),       p->max_vel);
	glUniform1f(glGetUniformLocation(gpu->render_program, "u_tex_size"),      (float)BOIDS_TEX_SIZE);

	glBindVertexArray(gpu->empty_vao);
	glDrawArrays(GL_POINTS, 0, app->n);

	gpu->current = next;
}
