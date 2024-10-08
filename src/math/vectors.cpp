
typedef struct vec2 vec2;
struct vec2 {
	float32 x;
	float32 y;
};

internal vec2
vec2_add(vec2 a, vec2 b) {
	return (vec2){ a.x + b.x, a.y + b.y };
}

internal vec2
vec2_sub(vec2 a, vec2 b) {
	return (vec2){ a.x - b.x, a.y - b.y };
}

internal vec2
vec2_div(vec2 a, float32 s) {
	return (vec2){ a.x / s, a.y / s };
}

internal vec2
vec2_mul(vec2 a, float32 s) {
	return (vec2){ a.x * s, a.y * s };
}

internal float32
vec2_dot(vec2 a, vec2 b) {
	return a.x*b.x + a.y*b.y;
}

internal float32
vec2_mag(vec2 a) {
	return sqrt(a.x*a.x + a.y*a.y);
}

internal float32
vec2_mag_sq(vec2 a) {
	return a.x*a.x + a.y*a.y;
}

internal float32
vec2_angle(vec2 a, vec2 b) {
	return acos(vec2_dot(a, b) / (vec2_mag(a) * vec2_mag(b)));
}

internal float32
vec2_dist(vec2 a, vec2 b) {
	return vec2_mag(vec2_sub(a, b));
}

internal float32
vec2_dist_sq(vec2 a, vec2 b) {
	return vec2_mag_sq(vec2_sub(a, b));
}


struct int16v2 {
	int16 x;
	int16 y;
};

internal int16v2
int16v2_n(int16 x, int16 y) {
	return {x, y};
}


