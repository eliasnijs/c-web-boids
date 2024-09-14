
typedef struct gpu_context_t GpuContext;
struct gpu_context_t {
	uint32 boids_program;
	uint32 boids_vao_id;
	uint32 boids_vbo_id;

	uint32 CTU_program;
	uint32 CTU_vao_id;
	uint32 CTU_vbo_id;
};

internal bool
gpu_init(GpuContext *gpu) {
	bool32 is_success = load_glprogram("src/view/shaders/boids.vertex.glsl",
					   "src/view/shaders/boids.fragment.glsl",
					   &gpu->boids_program);
	if (!is_success) {
		print_error("Failed to load boids shader program");
		return false;
	}

	glGenVertexArrays(1, &gpu->boids_vao_id);
	glGenBuffers(1,      &gpu->boids_vbo_id);

	is_success = load_glprogram("src/view/shaders/CTU.vertex.glsl",
				    "src/view/shaders/CTU.fragment.glsl",
				    &gpu->CTU_program);
	if (!is_success) {
		print_error("Failed to load CTU shader program");
		return false;
	}

	glGenVertexArrays(1, &gpu->CTU_vao_id);
	glGenBuffers(1,      &gpu->CTU_vbo_id);

	return true;
}

internal void
gpu_die(GpuContext *gpu) {
	glDeleteBuffers(1, &gpu->boids_vbo_id);
	glDeleteVertexArrays(1, &gpu->boids_vao_id);
	glprogram_die(gpu->boids_program);
}

internal void
qt_render(CTU *root, GLint uo, GLint us) {
	if (!root) {
		return;
	}
	if (root->state == CTU_LEAF) {
		glUniform2f(uo, root->o.x, root->o.y);
		glUniform1f(us, (float32)root->size);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	} else {
		for (int32 i = 0; i < root->n; ++i) {
			qt_render(&root->children[i], uo, us);
		}
	}
}

global_variable float32 BoxGeometry[] = {
    // First triangle
    0.0f, 0.0f,
    1.0f, 0.0f,
    0.0f, 1.0f,
    // Second triangle
    1.0f, 0.0f,
    1.0f, 1.0f,
    0.0f, 1.0f
};

#define SHOW_TREE 1
internal void
render(GpuContext *gpu, BoidsApplication *app, QuadTree *T, Arena *a) {
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
	glUniform1f(u_window_width, app->p.width);
	glUniform1f(u_window_height,app->p.height);
	glUniform1f(u_point_size, app->p.size);
	glDrawArrays(GL_POINTS, 0, app->n);

#ifdef SHOW_TREE
	GLint u_window_size	= glGetUniformLocation(gpu->CTU_program, "u_window_size");
	GLint u_origin		= glGetUniformLocation(gpu->CTU_program, "u_origin" );
	GLint u_size		= glGetUniformLocation(gpu->CTU_program, "u_size");

	glBindVertexArray(gpu->CTU_vao_id);
	glBindBuffer(GL_ARRAY_BUFFER, gpu->CTU_vbo_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(BoxGeometry), BoxGeometry, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float32), (void*)0);
	glEnableVertexAttribArray(0);

	glUseProgram(gpu->CTU_program);
        glUniform2f(u_window_size, app->p.width, app->p.height);
	glUseProgram(gpu->CTU_program);
	qt_render(&T->root, u_origin, u_size);
#endif
}
