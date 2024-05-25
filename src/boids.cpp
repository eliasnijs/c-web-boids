#define MAX_BOIDS 1000

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
};


typedef struct boids_application_t BoidsApplication;
struct boids_application_t {
	Param p;
	int32 n;
	Boid *bs;
};

internal bool8
has_influence(Boid *b_j, Boid *b_i, Param *p) {
	vec2 v_rel = vec2_sub(b_j->pos, b_i->pos);
	return (vec2_mag(v_rel) < p->r)
		&& (vec2_angle(b_i->vel, v_rel) < p->theta_max);

}

internal void
get_influencers(Boid *b, Boid *bs, int n, Param *p,
		Boid *influencers[], int *m) {
	int32 k = 0;
	for (int32 i = 0; i < n; ++i) {
		if (b != &bs[i] && has_influence(b, &bs[i], p)) {
			influencers[k] = &bs[i];
			++k;
		}
	}
	*m = k;
}

internal vec2
cohesion(Boid *b, Boid *influencers[], int m, Param *p) {
	vec2 v = {0, 0};
	if (m == 0) {
		return v;
	}
	for (int32 i = 0; i < m; ++i) {
		v = vec2_add(v, influencers[i]->pos);
	}
	v = vec2_div(v, m);
	v = vec2_sub(v, b->pos);
	v = vec2_mul(v, p->c);
	return v;
}

internal vec2
seperation(Boid *b, Boid *influencers[], int m, Param *p) {
	vec2 v = {0, 0};
	for (int32 i = 0; i < m; ++i) {
		Boid *b_i = influencers[i];
		if (vec2_dist(b->pos, b_i->pos) < p->s_r) {
			v = vec2_sub(v, vec2_sub(b_i->pos, b->pos));
		}
	}
	v = vec2_mul(v, p->s);
	return v;
}

internal vec2
alignment(Boid *b, Boid *influencers[], int m, Param *p) {
	vec2 v = {0, 0};
	if (m == 0) {
		return v;
	}
	for (int32 i = 0; i < m; ++i) {
		v = vec2_add(v, influencers[i]->vel);
	}
	v = vec2_div(v, m);
	v = vec2_sub(v, b->vel);
	v = vec2_mul(v, p->a);
	return v;
}

internal void
update_boid(BoidsApplication *app, Boid *b) {
	// BUG(Elias): Boids are going out of bounds on the right and top
	// borders.

	Boid *bs = app->bs;
	Param *p = &app->p;
	int32 n = app->n;

	Boid *influencers[n];
	int32 m;
	get_influencers(b, bs, n, p, influencers, &m);

	b->vel = vec2_add(b->vel, cohesion(b, influencers, m, p));
	b->vel = vec2_add(b->vel, seperation(b, influencers, m, p));
	b->vel = vec2_add(b->vel, alignment(b, influencers, m, p));
	b->vel.x = Clamp(-p->max_vel, b->vel.x, p->max_vel);
	b->vel.y = Clamp(-p->max_vel, b->vel.y, p->max_vel);


	b->pos = vec2_add(b->pos, b->vel);

	if (b->pos.x < 0) {
		b->vel.x = Abs(b->vel.x);
	} else if (b->pos.x > window_width) {
		b->vel.x = -Abs(b->vel.x);
	}
	if (b->pos.y < 0) {
		b->vel.y = Abs(b->vel.y);
	} else if (b->pos.y > window_height) {
		b->vel.y = -Abs(b->vel.y);
	}

}

internal void
update_boids(BoidsApplication *app) {
	for (int32 i = 0; i < app->n; ++i) {
		update_boid(app, &(app->bs[i]));
	}
}

internal void
init_boid(Boid *b, Param *p) {
	b->pos.x = rand() % window_width;
	b->pos.y = rand() % window_height;
	b->vel.x = (rand() % 100 - 50) / 50.0;
	b->vel.y = (rand() % 100 - 50) / 50.0;
}

internal void
init_boids_app(BoidsApplication *app) {
	Param *p = &app->p;
	app->n = 100;
	p->r = 50;
	p->theta_max = 3.14 / 4;
	p->c = 0.01;
	p->s_r = 10;
	p->s = 0.1;
	p->a = 0.1;
	p->max_vel = 5;

	app->bs = (Boid *)calloc(MAX_BOIDS, sizeof(Boid));
	for (int32 i = 0; i < MAX_BOIDS; ++i) {
		init_boid(&app->bs[i], p);
	}
}

internal void
boids_app_die(BoidsApplication *app) {
	free(app->bs);
}



