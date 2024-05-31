internal void
imgui_init(GLFWwindow* window) {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	ImGui_ImplGlfw_InitForOpenGL(window, true);
#ifdef __EMSCRIPTEN__
	ImGui_ImplOpenGL3_Init("#version 100");
#else
	ImGui_ImplOpenGL3_Init("#version 130");
#endif
}

internal void
imgui_frame(Process *p) {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGui::SetNextWindowSize(ImVec2(500, 300));
	ImGui::Begin("Controls", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
	ImGui::Text("Process Information");
	ImGui::Text("Frame Time: %.3f ms | FPS: %.1f",
		    p->ctx.frame_time * 1000.0f, p->ctx.fps);
	ImGui::Separator();
	ImGui::Text("Boids controls");
	ImGui::SliderInt("Number of Boids", &p->boids_app.n, 0, 1000);
	ImGui::SliderFloat("Cohesion", &p->boids_app.p.c, 0.0f, 1.0f);
	ImGui::SliderFloat("Separation", &p->boids_app.p.s, 0.0f, 1.0f);
	ImGui::SliderFloat("Alignment", &p->boids_app.p.a, 0.0f, 1.0f);
	ImGui::SliderFloat("Separation Radius", &p->boids_app.p.s_r, 0.0f, 100.0f);
	ImGui::SliderFloat("Radius", &p->boids_app.p.r, 0.0f, 500.0f);
	ImGui::SliderFloat("Theta Max", &p->boids_app.p.theta_max, 0.0f, 3.14f);
	ImGui::SliderFloat("Max Velocity", &p->boids_app.p.max_vel, 0.0f, 1000.0f);
	ImGui::SliderFloat("Point Size", &p->boids_app.p.size, 1.0f, 10.0f);
	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

internal void
imgui_die() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}
