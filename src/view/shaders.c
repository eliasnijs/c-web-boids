internal bool
load_glshader(char *path, uint32 type, uint32 *shader_id) {
	FILE *f = fopen(path, "r");
	if (!f) {
		print_error("failed to find shader path");
		return false;
	}

	uint32 flength = flen(f);
	char *fbuffer = (char *)calloc(flength, sizeof(char));
	int32 bytesread = fread(fbuffer, flength, sizeof(char), f);
	if (bytesread == 0) {
		print_error("reading file failed");
	}

	uint32 id = glCreateShader(type);
	glShaderSource(id, 1, (const char * const *)&fbuffer, 0);
	glCompileShader(id);

	int32 success;
	glGetShaderiv(id, GL_COMPILE_STATUS, &success);
	if (!success) {
		char info[1024] = {0};
		glGetShaderInfoLog(id, 1024, 0, info);
		fprintf(stderr, "failed to compile shader component (%s):\n%s",
		       path, info);
		free(fbuffer);
		return false;
	}
	*(shader_id) = id;

	free(fbuffer);
	return true;
}

internal bool
load_glprogram(char *vertexPath, char *fragmentPath, uint32 *program_id) {
	uint32 vertex_shader, fragment_shader;
	int32 success;

	success = load_glshader(vertexPath, GL_VERTEX_SHADER, &vertex_shader);
	if (!success) {
		print_error("failed to load vertex shader");
		return false;
	}
	success = load_glshader(fragmentPath, GL_VERTEX_SHADER, &fragment_shader);
	if (!success) {
		print_error("failed to load fragment shader");
		glDeleteShader(vertex_shader);
		return false;
	}

	uint32 id = glCreateProgram();
	glAttachShader(id, vertex_shader);
	glAttachShader(id, fragment_shader);
	glLinkProgram(id);

	glGetProgramiv(id, GL_LINK_STATUS, &success);
	if (!success) {
		char info[1024] = {0};
		glGetShaderInfoLog(id, 1024, 0, info);
		print_error("failed to opengl id:\n%s", info);
		glDeleteShader(vertex_shader);
		glDeleteShader(fragment_shader);
		glDeleteProgram(id);
		return false;
	}

	*(program_id) = id;

	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);

	return true;
}

internal void
glprogram_die(uint32 program_id) {
	glDeleteProgram(program_id);
}
