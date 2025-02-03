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

#ifdef NO_CONTROL_PANEL
	/* ImGui::SetNextWindowSize(ImVec2(450, 50)); */
	/* ImGui::SetNextWindowPos(ImVec2(window_width - 450 - margin, */
	/* 			       window_height - 50 - margin)); */
	/* ImGui::Begin("Process Information", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse); */
	/* ImGui::Text("Frame Time: %.3f ms | FPS: %.1f", */
	/* 	    p->ctx.frame_time * 1000.0f, p->ctx.fps); */
	/* ImGui::End(); */
	/* ImGui::SetNextWindowSize(ImVec2(450, 250)); */
	/* ImGui::Begin("Controls", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize); */
	/* ImGui::SliderInt("Number of Boids", &p->boids_app.n, 0, MAX_BOIDS); */
	/* ImGui::SliderFloat("Cohesion", &p->boids_app.p.c, 0.0f, 1.0f); */
	/* ImGui::SliderFloat("Separation", &p->boids_app.p.s, 0.0f, 1.0f); */
	/* ImGui::SliderFloat("Alignment", &p->boids_app.p.a, 0.0f, 1.0f); */
	/* ImGui::SliderFloat("Separation Radius", &p->boids_app.p.s_r, 0.0f, 100.0f); */
	/* ImGui::SliderFloat("Radius", &p->boids_app.p.r, 0.0f, 500.0f); */
	/* ImGui::SliderFloat("Theta Max", &p->boids_app.p.theta_max, 0.0f, 3.14f/2.0f); */
	/* ImGui::SliderFloat("Max Velocity", &p->boids_app.p.max_vel, 1.0f, 20.0f); */
	/* ImGui::SliderFloat("Point Size", &p->boids_app.p.size, 1.0f, 10.0f); */
	/* ImGui::SliderFloat("mouseG", &p->boids_app.p.mouseG, 0.0f, 100.0f); */
	/* ImGui::End(); */
#else
	ImGui::SetNextWindowSize(ImVec2(500, 340));
	ImGui::Begin("Controls", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
	ImGui::Text("Process Information");
	ImGui::Text("Frame Time: %.3f ms | FPS: %.1f",
		    p->ctx.frame_time * 1000.0f, p->ctx.fps);
	ImGui::Separator();
	ImGui::Text("Boids controls");
	ImGui::SliderInt("Number of Boids", &p->boids_app.n, 0, MAX_BOIDS);
	ImGui::SliderFloat("Cohesion", &p->boids_app.p.c, 0.0f, 1.0f);
	ImGui::SliderFloat("Separation", &p->boids_app.p.s, 0.0f, 1.0f);
	ImGui::SliderFloat("Alignment", &p->boids_app.p.a, 0.0f, 1.0f);
	ImGui::SliderFloat("Separation Radius", &p->boids_app.p.s_r, 0.0f, 100.0f);
	ImGui::SliderFloat("Radius", &p->boids_app.p.r, 0.0f, 500.0f);
	ImGui::SliderFloat("Theta Max", &p->boids_app.p.theta_max, 0.0f, 3.14f/2.0f);
	ImGui::SliderFloat("Max Velocity", &p->boids_app.p.max_vel, 1.0f, 20000.0f, "%.3f", ImGuiSliderFlags_Logarithmic);

	ImGui::Checkbox("mouseG", (bool *)&p->boids_app.p.mouseG_enabled);
	if (p->boids_app.p.mouseG_enabled) {
		ImGui::SliderFloat("mouseG Strength", &p->boids_app.p.mouseG, 1.0f, 700.0f, "%.3f", ImGuiSliderFlags_Logarithmic);
	}

	ImGui::SliderFloat("Point Size", &p->boids_app.p.size, 1.0f, 10.0f);
	ImGui::End();
#endif

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

internal void
imgui_die() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}
