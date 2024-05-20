internal void
handle_input(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS
	    || glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
}

internal void
simple_error_callback(int32 error, const char* description) {
	fprintf(stderr, "GLFW error: %s\n", description);
}

internal void
framebuffer_size_callback(GLFWwindow* window, int32 width, int32 height) {
	window_width = width;
	window_height = height;
	glViewport(0, 0, width, height);
}

internal GLFWwindow *
window_init() {
	glfwSetErrorCallback(simple_error_callback);
	if (!glfwInit()) {
		print_error("Failed to initialize GLFW");
		return 0x0;
	}

	/* TODO(Elias): Implement OpenGL versioning */
	/* glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); */
	/* glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6); */

	GLFWwindow* window = glfwCreateWindow(window_width, window_height,
					      title, NULL, NULL);

	if (!window) {
		print_error("Failed to create window");
		glfwTerminate();
		return 0x0;
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		print_error("Failed to initialize GLAD");
		glfwTerminate();
		return 0x0;
	}

	return window;
}

internal void
window_die(GLFWwindow* window) {
	glfwDestroyWindow(window);
	glfwTerminate();
}
