internal void
handle_input(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS
	    || glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
}

internal void
save_window(GLFWwindow* window) {
	int32 x, y, w, h;
	glfwGetWindowPos(window, &x, &y);
	glfwGetWindowSize(window, &w, &h);
	FILE* file = fopen("hg.ini", "w");
	if (!file) {
		print_error("Failed to save window to hg.ini");
		return;
	}
	fprintf(file, "[window]\nx=%d\ny=%d\nw=%d\nh=%d\n", x, y, w, h);
	fclose(file);
}

internal void
read_window(GLFWwindow* window)
{
	FILE* file = fopen("hg.ini", "r");
	if (!file) {
		print_error("Failed to read window from hg.ini");
		return;
	}
	int32 x, y, w, h;
	fscanf(file, "[window]\nx=%d\ny=%d\nw=%d\nh=%d\n", &x, &y, &w, &h);
	fclose(file);
	glfwSetWindowPos(window, x, y);
	glfwSetWindowSize(window, w, h);
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
	read_window(window);

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
	save_window(window);
	glfwDestroyWindow(window);
	glfwTerminate();
}
