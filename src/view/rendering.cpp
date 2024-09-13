
typedef struct gpu_context_t GpuContext;
struct gpu_context_t {
	uint32 boids_program;
	uint32 boids_vao_id;
	uint32 boids_vbo_id;
	Texture tex;
};

internal bool
gpu_init(GpuContext *gpu) {
	bool32 is_success = load_glprogram(
	    "src/view/shaders/boids.vertex.glsl",
	    "src/view/shaders/boids.fragment.glsl",
	    &gpu->boids_program);
	if (!is_success) {
		print_error("Failed to load boids shader program");
		return false;
	}

	if (!load_texture("res/texture.jpeg", &gpu->tex)) {
		print_error("failed to load texture");
		return false;
	}

	glUseProgram(gpu->boids_program);
	glUniform1i(glGetUniformLocation(gpu->boids_program, "tex"), 0);

	glGenVertexArrays(1, &gpu->boids_vao_id);
	glGenBuffers(1, &gpu->boids_vbo_id);

	return is_success;
}

internal void
gpu_die(GpuContext *gpu) {
	texture_die(&gpu->tex);
	glDeleteBuffers(1, &gpu->boids_vbo_id);
	glDeleteVertexArrays(1, &gpu->boids_vao_id);
	glprogram_die(gpu->boids_program);
}

internal void
render(GpuContext *gpu, BoidsApplication *app) {
	GLint u_window_width  = glGetUniformLocation(gpu->boids_program, "u_window_width" );
	GLint u_window_height = glGetUniformLocation(gpu->boids_program, "u_window_height");
	GLint u_point_size    = glGetUniformLocation(gpu->boids_program, "u_point_size"   );

	glBindVertexArray(gpu->boids_vao_id);
	glBindBuffer(GL_ARRAY_BUFFER, gpu->boids_vbo_id);
	glBufferData(GL_ARRAY_BUFFER, app->n * sizeof(Boid), app->bs,
		     GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float32), (void *)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float32), (void *)(2 * sizeof(float32)));
	glEnableVertexAttribArray(1);

	glUseProgram(gpu->boids_program);
	use_texture(&gpu->tex, 0);
	glUniform1f(u_window_width, app->p.width);
	glUniform1f(u_window_height,app->p.height);
	glUniform1f(u_point_size, app->p.size);
	glDrawArrays(GL_POINTS, 0, app->n);
}
