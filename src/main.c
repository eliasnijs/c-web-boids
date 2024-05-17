#include <stdio.h>
#include <stdlib.h>
#include <execinfo.h>

#include <glad.h>
#include <glad.c>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define ENABLE_ASSERT 1
#define ENABLE_DEBUGLOG 1
#define ENABLE_PRINT 1
#include <base_all.h>

#include "config.h"
#include "utils/files.c"
#include "math/vectors.c"

#include "boids.c"

#include "view/shaders.c"
#include "view/rendering.c"

typedef struct process_ctx_t ProcessContext;
struct process_ctx_t {
	/* data */
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

#include "view/window.c"
#include "view/imgui.c"

int32
main() {
	GLFWwindow *window = window_init();
	if (!window) {
		print_error("Failed to initialize window");
		return 1;
	}
	imgui_init(window);

	/* initialisation */
	Process p = {0};

	ProcessContext *ctx		= &p.ctx;
	ctx->max_fps			= 60.0f;

	GpuContext *gpu			= &p.gpu;

	BoidsApplication *boids_app	= &p.boids_app;
	init_boids_app(boids_app);

	/* main loop */
	float64 start_time = glfwGetTime();
	while (!glfwWindowShouldClose(window)) {
		start_time = glfwGetTime();
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glfwPollEvents();
		handle_input(window);

		update_boids(boids_app);
		render(gpu, boids_app);

		imgui_frame(&p);
		glfwSwapBuffers(window);


		// limit fps
		while (glfwGetTime() - start_time < 1.0f / p.ctx.max_fps) {
			;
		}
		p.ctx.frame_time = glfwGetTime() - start_time;
		p.ctx.fps = 1.0f / p.ctx.frame_time;
	}

	/* cleanup */
	boids_app_die(boids_app);
	gpu_die(gpu);
	imgui_die();
	window_die(window);

	return 0;
}

