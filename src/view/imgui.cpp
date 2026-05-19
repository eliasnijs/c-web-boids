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
	ImGui::SetNextWindowSize(ImVec2(500, 510));
	ImGui::Begin("Controls", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
	ImGui::Text("Process Information");
	ImGui::Text("Frame Time: %.3f ms | FPS: %.1f",
		    p->ctx.frame_time * 1000.0f, p->ctx.fps);
	ImGui::Separator();
	ImGui::Text("Boids controls");
	ImGui::SliderInt("Number of Boids", &p->boids_app.n, 0, MAX_BOIDS, "%d", ImGuiSliderFlags_Logarithmic);
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

	ImGui::Separator();
	ImGui::Text("Resonating Cavity");
	ImGui::Checkbox("Cavity Enabled", (bool *)&p->boids_app.p.cavity_enabled);
	if (p->boids_app.p.cavity_enabled) {
		const char* shapes[] = { "Polygon", "Lemniscate" };
		ImGui::Combo("Shape", &p->boids_app.p.cavity_shape, shapes, 2);
		ImGui::SliderFloat("Cavity X", &p->boids_app.p.cavity_x, 0.0f, (float)window_width);
		ImGui::SliderFloat("Cavity Y", &p->boids_app.p.cavity_y, 0.0f, (float)window_height);
		ImGui::SliderFloat("Cavity Radius", &p->boids_app.p.cavity_r, 10.0f, 500.0f);
		ImGui::SliderFloat("Cavity Strength", &p->boids_app.p.cavity_strength, 0.0f, 500.0f, "%.1f", ImGuiSliderFlags_Logarithmic);
		ImGui::SliderFloat("Resonance Amplitude", &p->boids_app.p.cavity_amplitude, 0.0f, 500.0f, "%.1f", ImGuiSliderFlags_Logarithmic);
		ImGui::SliderFloat("Resonance Frequency", &p->boids_app.p.cavity_frequency, 0.0f, 20.0f, "%.2f Hz");
		if (p->boids_app.p.cavity_shape == 0) {
			ImGui::SliderInt("Cavity Corners", &p->boids_app.p.cavity_n, 3, 32);
		}

		ImDrawList *dl = ImGui::GetBackgroundDrawList();
		float cx = p->boids_app.p.cavity_x;
		float cy = p->boids_app.p.cavity_y;
		float cr = p->boids_app.p.cavity_r;
		ImU32 col = IM_COL32(255, 255, 255, 120);

		if (p->boids_app.p.cavity_shape == 0) {
			int cn = p->boids_app.p.cavity_n;
			float angle_step = 2.0f * 3.14159265f / (float)cn;
			for (int i = 0; i < cn; ++i) {
				float a0 = i       * angle_step;
				float a1 = (i + 1) * angle_step;
				dl->AddLine(ImVec2(cx + cr * cosf(a0), cy + cr * sinf(a0)),
				            ImVec2(cx + cr * cosf(a1), cy + cr * sinf(a1)),
				            col, 1.5f);
			}
		} else {
			int samples = 256;
			for (int i = 0; i < samples; ++i) {
				float t0 = 2.0f * 3.14159265f * (float)i       / (float)samples;
				float t1 = 2.0f * 3.14159265f * (float)(i + 1) / (float)samples;
				auto lemni = [&](float t) -> ImVec2 {
					float s = sinf(t), c = cosf(t);
					float d = 1.0f + s * s;
					return ImVec2(cx + cr * c / d, cy + cr * s * c / d);
				};
				dl->AddLine(lemni(t0), lemni(t1), col, 1.5f);
			}
		}
	}

	ImGui::Separator();
	ImGui::Text("Rendering");
	const char* color_modes[] = { "Direction", "Speed" };
	ImGui::Combo("Color Mode", &p->boids_app.p.color_mode, color_modes, 2);
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
