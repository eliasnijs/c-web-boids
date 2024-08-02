
struct Texture {
	uint32 id;
	uint32 w, h, c;
};

internal bool32
load_texture_param(char *path, Texture *texture, int32 wrap_s, int32 wrap_t,
		     int32 min_filter, int32 mag_filter) {
	uint32 t;
	glGenTextures(1, &t);
	glBindTexture(GL_TEXTURE_2D, t);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_s);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_t);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_filter);

	int32 w, h, c;
	uint8 *data = stbi_load(path, &w, &h, &c, 0);
	if (!data) {
		print_error("failed to load texture");
		return false;
	}

	switch (c) {
	case 3: {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	} break;
	case 4: {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	} break;
	case 1: {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, w, h, 0, GL_RED, GL_UNSIGNED_BYTE, data);
	} break;
	default: {
		print_error("unsupported number of channels");
		return false;
	} break;
	}

	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(data);

	texture->id = t;
	texture->w = w;
	texture->h = h;
	texture->c = c;
	return true;
}

#define TEXTURE_DEFAULT_WRAP_S		GL_REPEAT
#define TEXTURE_DEFAULT_WRAP_T		GL_REPEAT
#define TEXTURE_DEFAULT_MIN_FILTER	GL_LINEAR
#define TEXTURE_DEFAULT_MAG_FILTER	GL_LINEAR

internal bool32
load_texture(char *path, Texture *texture)
{
	return load_texture_param(path, texture,
				  TEXTURE_DEFAULT_WRAP_S,
				  TEXTURE_DEFAULT_WRAP_T,
				  TEXTURE_DEFAULT_MIN_FILTER,
				  TEXTURE_DEFAULT_MAG_FILTER);
}


internal void
use_texture(Texture *texture, uint32 slot)
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, texture->id);
}

internal void
texture_die(Texture *texture)
{
	glDeleteTextures(1, &texture->id);
}

