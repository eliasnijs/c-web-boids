
typedef struct gpu_context_t GpuContext;
struct gpu_context_t {
	uint32 boids_program;
	uint32 boids_vao_id;
	uint32 boids_vbo_id;
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
	glGenVertexArrays(1, &gpu->boids_vao_id);
	glGenBuffers(1, &gpu->boids_vbo_id);
	return is_success;
}

internal void
gpu_die(GpuContext *gpu) {
	glprogram_die(gpu->boids_program);
}

internal void
push_data_to_gpu() {
}

internal void
render(GpuContext *gpu, BoidsApplication *app) {
	// TODO(Elias): This is a temporary solution, is super inefficient
	float32 data[MAX_BOIDS][5] = {};
	for (int32 i = 0; i < app->n; i++) {
		float32 r = Clamp(0.1, abs(app->bs[i].vel.x) / 2.0f, 1.0f);
		float32 g = 1.0;
		float32 b = Clamp(0.1, abs(app->bs[i].vel.y) / 2.0f, 1.0f);
		float32 x = app->bs[i].pos.x / 128.0f - 1.0f;
		float32 y = app->bs[i].pos.y / 128.0f - 1.0f;
		float32 args[5] = {x, y, r, g, b};
		memcpy(data[i], args, sizeof(args));
	}

	glBindVertexArray(gpu->boids_vao_id);
	glBindBuffer(GL_ARRAY_BUFFER, gpu->boids_vbo_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float32), (void *)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float32), (void *)(2 * sizeof(float32)));
	glEnableVertexAttribArray(1);

	glUseProgram(gpu->boids_program);
	glDrawArrays(GL_POINTS, 0, app->n);
}
