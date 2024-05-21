#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/html5.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <glad.h>
#include <glad.c>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#define ENABLE_ASSERT 1
#define ENABLE_DEBUGLOG 1
#define ENABLE_PRINT 1
#include <base_all.h>

#include "config.h"
#include "utils/files.cpp"
#include "math/vectors.cpp"

#include "boids.cpp"

#include "view/shaders.cpp"
#include "view/rendering.cpp"

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

