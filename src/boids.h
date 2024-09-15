#define MAX_BOIDS 2000

typedef struct boid_t Boid;
struct boid_t {
	vec2 pos;
	vec2 vel;
};


typedef struct param_t Param;
struct param_t {
	float32 r;
	float32 theta_max;

	float32 c;

	float32 s_r;
	float32 s;

	float32 a;

	float32 max_vel;
	float32 size;

	int32   width;
	int32   height;
};


typedef struct boids_application_t BoidsApplication;
struct boids_application_t {
	Param p;
	int32 n;
	Boid *bs;
};

