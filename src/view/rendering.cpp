
typedef struct gpu_context_t GpuContext;
struct gpu_context_t {
	uint32 boids_program;
	uint32 boids_vao_id;
	uint32 boids_vbo_id;

	uint32 lines_program;
	uint32 lines_vao_id;
	uint32 lines_vbo_id;

	Texture tex;
};

internal bool
gpu_init(GpuContext *gpu) {
	bool32 is_success = false;

	is_success = load_glprogram(
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

	is_success = load_glprogram(
	    "src/view/shaders/boids.vertex.glsl",
	    "src/view/shaders/lines.fragment.glsl",
	    &gpu->lines_program);
	if (!is_success) {
		print_error("Failed to load lines shader program");
		return false;
	}
	glUseProgram(gpu->lines_program);
	glGenVertexArrays(1, &gpu->lines_vao_id);
	glGenBuffers(1, &gpu->lines_vbo_id);

	return true;
}

internal void
gpu_die(GpuContext *gpu) {
	texture_die(&gpu->tex);
	glDeleteBuffers(1, &gpu->boids_vbo_id);
	glDeleteVertexArrays(1, &gpu->boids_vao_id);
	/* glDeleteBuffers(1, &gpu->lines_vbo_id); */
	/* glDeleteVertexArrays(1, &gpu->lines_vao_id); */
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
	glUniform1f(u_window_width, window_width);
	glUniform1f(u_window_height, window_height);
	glUniform1f(u_point_size, app->p.size);
	glDrawArrays(GL_POINTS, 0, app->n);

	if (app->n > 2) {
		vec2 *lines = (vec2 *)calloc(app->n * 6, sizeof(vec2));

		for (int32 i = 0; i < app->n; ++i) {
			Boid *b = &(app->bs[i]);
			vec2 pos1 = {}; float32 d1 = FLOAT32_MAX;
			vec2 pos2 = {}; float32 d2 = FLOAT32_MAX;
			for (int32 i = 0; i < app->n; ++i) {
				Boid *b_checking = &(app->bs[i]);
				if (b == b_checking) {
					continue;
				}
				float32 d = vec2_dist(b->pos, b_checking->pos);
				if (d < d1) {
					pos2 = pos1;
					d2   = d1;
					pos1 = b_checking->pos;
					d1   = d;
				} else if (d < d2) {
					pos2 = b_checking->pos;
					d2   = d;
				}
			}

			lines[i*6 + 0] = b->pos;  // Edge 1: boid -> nearest1
			lines[i*6 + 1] = pos1;
			lines[i*6 + 2] = pos1;    // Edge 2: nearest1 -> nearest2
			lines[i*6 + 3] = pos2;
			lines[i*6 + 4] = pos2;    // Edge 3: nearest2 -> boid
			lines[i*6 + 5] = b->pos;
		}

		glBindVertexArray(gpu->lines_vao_id);
		glBindBuffer(GL_ARRAY_BUFFER, gpu->lines_vbo_id);
		glBufferData(GL_ARRAY_BUFFER, app->n * 6 * sizeof(vec2), lines, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float32), (void *)0);
		glEnableVertexAttribArray(0);

		glUseProgram(gpu->lines_program);
		GLint u_window_width  = glGetUniformLocation(gpu->lines_program, "u_window_width");
		GLint u_window_height = glGetUniformLocation(gpu->lines_program, "u_window_height");
		glUniform1f(u_window_width, window_width);
		glUniform1f(u_window_height, window_height);

		glDrawArrays(GL_LINES, 0, app->n * 6);


		free(lines);
	}

}






