internal bool8
has_influence(Boid *self, Boid *other, Param *p) {
	vec2 v_rel = vec2_sub(self->pos, other->pos);
	return vec2_mag_sq(v_rel) < (p->r * p->r)
		&& (vec2_angle(other->vel, v_rel) < p->theta_max
		    || vec2_angle(other->vel, v_rel) > (2*3.14) - p->theta_max);
}

internal void
update_boid_(CTU *node, Boid *self, Boid *boids, Param *p,
	     vec2 *coh, vec2 *sep, vec2 *ali, int32 *n_in_range)
{
	vec2 nearest_point = {
		Clamp(node->o.x, self->pos.x, node->o.x + node->size),
		Clamp(node->o.y, self->pos.y, node->o.y + node->size),
	};
	if (vec2_dist_sq(self->pos, nearest_point) >= (p->r * p->r)) {
		return;
	}

	if (node->state == CTU_LEAF) {
		for (int32 i = 0; i < node->n; ++i) {
			Boid *other = &boids[node->indices[i]];
			if (has_influence(self, other, p)) {
				*coh = vec2_add(*coh, other->pos);
				*ali = vec2_add(*ali, other->vel);
				if (vec2_dist_sq(self->pos, other->pos) < (p->s_r * p->s_r)) {
					*sep = vec2_sub(*sep, vec2_sub(other->pos, self->pos));
				}
				++(*n_in_range);
			}
		}
		return;
	}

	for (int32 i = 0; i < 4; ++i) {
		CTU *child = &node->children[i];
		update_boid_(child, self, boids, p, coh, sep, ali, n_in_range);
	}
}

internal void
update_boid(BoidsApplication *app, Boid *b, QuadTree *T)
{
	Param *p  = &app->p;

	vec2 coh, sep, ali;
	coh = sep = ali = {0, 0};

	int32 n_in_range = 0;

	update_boid_(&T->root, b, app->bs, p, &coh, &sep, &ali, &n_in_range);

	printf("%d\n", n_in_range);
	if (n_in_range > 0) {
		// process the forces
		coh = vec2_div(coh, n_in_range);
		coh = vec2_sub(coh, b->pos);
		coh = vec2_mul(coh, p->c);

		sep = vec2_mul(sep, p->s);

		ali = vec2_div(ali, n_in_range);
		ali = vec2_sub(ali, b->vel);
		ali = vec2_mul(ali, p->a);

		// accumulate forces
		b->vel = vec2_add(b->vel, coh);
		b->vel = vec2_add(b->vel, sep);
		b->vel = vec2_add(b->vel, ali);
		b->vel.x = Clamp(-p->max_vel, b->vel.x, p->max_vel);
		b->vel.y = Clamp(-p->max_vel, b->vel.y, p->max_vel);
	}

	// update position
	b->pos = vec2_add(b->pos, b->vel);

	// reflect on border
	if (b->pos.x <= 0) {
		b->vel.x = Abs(b->vel.x);
	} else if (b->pos.x > Min(p->width, p->height)) {
		b->vel.x = -Abs(b->vel.x);
	}
	if (b->pos.y <= 0) {
		b->vel.y = Abs(b->vel.y);
	} else if (b->pos.y > Min(p->width, p->height)) {
		b->vel.y = -Abs(b->vel.y);
	}
}



internal void
update_boids(BoidsApplication *app, QuadTree *T) {
	for (int32 i = 0; i < app->n; ++i) {
		update_boid(app, &(app->bs[i]), T);
	}
}

internal void
init_boid(Boid *b, Param *p) {
	b->pos.x = rand() % p->width;
	b->pos.y = rand() % p->height;
	b->vel.x = (rand() % 100 - 50) / 50.0f;
	b->vel.y = (rand() % 100 - 50) / 50.0f;
}

internal void
init_boids_app(BoidsApplication *app) {
	Param *p = &app->p;
	app->n		= 1000;
	p->r		= 50;
	p->theta_max	= 3.14 / 5;
	p->c		= 0.01;
	p->s_r		= 10;
	p->s		= 0.1;
	p->a		= 0.1;
	p->max_vel	= 10.0;
	p->size		= 1.0f;
	p->width	= 800;
	p->height	= 600;

	app->bs = (Boid *)calloc(MAX_BOIDS, sizeof(Boid));
	for (int32 i = 0; i < MAX_BOIDS; ++i) {
		init_boid(&app->bs[i], p);
	}
}

internal void
boids_app_die(BoidsApplication *app) {
	free(app->bs);
}



