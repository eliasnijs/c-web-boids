#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/html5.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include <glad.h>
#include <glad.c>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <stb_image.h>

#define ENABLE_ASSERT 1
#define ENABLE_DEBUGLOG 1
#define ENABLE_PRINT 1
#include <base_all.h>

#include "config.h"
#include "utils/files.cpp"
#include "utils/arena.cpp"
#include "math/vectors.cpp"
#include "boids.cpp"
#include "quadtree.c"


#include "view/shaders.cpp"
#include "view/textures.cpp"
#include "view/rendering.cpp"

// TODO(Elias): !!!! Let boids be influenced by music !!!!

// TODO(Elias): Add the capability to add obstacles

typedef struct process_ctx_t ProcessContext;
struct process_ctx_t {
	/* data */
	GLFWwindow *window;
	float64 frame_time;
	float64 fps;

	/* controls */
	float32 max_fps;
	bool    is_paused;
	bool	signal_step;
};

typedef struct process_t Process;
struct process_t {
	ProcessContext		ctx;
	GpuContext		gpu;
	BoidsApplication	boids_app;
	Arena			frame_arena;
};

global_variable Process PROCESS = {};

#include "view/window.cpp"
#include "view/imgui.cpp"

internal void
frame() {
	Process *p = &PROCESS;
	float64 start_time = glfwGetTime();

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glfwPollEvents();
	handle_input(p->ctx.window);

	int width, height;
	glfwGetFramebufferSize(p->ctx.window, &width, &height);
	p->boids_app.p.width  = width;
	p->boids_app.p.height = height;

	// add to quad tree

	QuadTree *T = {0};
	qt_init(T, 2000);
	for (int32 i = 0; i < 1; ++i) {
		qt_insert(T, i, p->boids_app.bs, &p->frame_arena);
	}

	// rende quad tree

	update_boids(&p->boids_app);

	render(&p->gpu, &p->boids_app);

	imgui_frame(p);

	glfwSwapBuffers(p->ctx.window);

	p->ctx.frame_time = glfwGetTime() - start_time;
	p->ctx.fps = 1.0f / p->ctx.frame_time;
}

int32
main() {
	GLFWwindow *window = window_init();
	if (!window) {
		print_error("Failed to initialize window");
		return 1;
	}
	imgui_init(window);

	/* initialisation */
	PROCESS.ctx.window = window;
	PROCESS.ctx.max_fps = 60.0f;

	uint64 bufferl = 10LL * 1024LL * 1024LL;
	char *buffer = (char *)malloc(bufferl);
	arena_init(&PROCESS.frame_arena, buffer, bufferl);

	init_boids_app(&PROCESS.boids_app);
	if (!gpu_init(&PROCESS.gpu)) {
		print_error("Failed to initialize gpu");
		return 1;
	}

	/* main loop */
#ifdef __EMSCRIPTEN__
	emscripten_set_main_loop(frame, 0, true);
#else
	while (!glfwWindowShouldClose(window)) {
		frame();
	}
#endif

	/* cleanup */
	boids_app_die(&PROCESS.boids_app);
	gpu_die(&PROCESS.gpu);
	imgui_die();
	window_die(PROCESS.ctx.window);

	return 0;
}

