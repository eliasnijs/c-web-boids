
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
	glPointSize(5.0);
	glBegin(GL_POINTS);
	for (int32 i = 0; i < app->n; i++) {
		float32 r = Clamp(0.1, abs(app->bs[i].vel.x) / 2.0f, 1.0f);
		float32 g = 1 - r;
		float32 b = 0.1f;
		glColor3f(r, g, b);
		float32 x = app->bs[i].pos.x / 128.0f - 1.0f;
		float32 y = app->bs[i].pos.y / 128.0f - 1.0f;
		glVertex2f(x, y);
	}
	glEnd();
}

