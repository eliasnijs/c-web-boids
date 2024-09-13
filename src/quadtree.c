
#define MAX_DEPTH   8  // TODO(Elias): set based on boids range
#define MAX_PER_CTU 4

enum Quadrant {
    QUADRANT_NE = 0,
    QUADRANT_SE = 1,
    QUADRANT_SW = 2,
    QUADRANT_NW = 3,
};

enum CTU_State {
	CTU_LEAF,
	CTU_NODE
};

struct CTU {
	int16v2	  o;
	int16v2	  size;

	CTU_State state;

	int32     n;
	union {
		CTU *children[4];
		struct {
			int32 capacity;
			int32 *indices;
		};
	};
};

struct QuadTree {
	CTU	root;
};

/////////////////////////////////////////////////////////////////////
//// Utility functions

internal Quadrant
get_quadrant(int16v2 o, int16v2 size, vec2 pos) {
	int16 mx = o.x + size.x / 2;
	int16 my = o.y + size.y / 2;

	if (pos.x >= mx) {
		return (pos.y >= my) ? QUADRANT_NE : QUADRANT_SE;
	} else {
		return (pos.y >= my) ? QUADRANT_NW : QUADRANT_SW;
	}
}



/////////////////////////////////////////////////////////////////////
//// Implementation

internal void
qt_init(QuadTree *t, int16 size) {
	t->root.size = (int16v2){size, size};
}

internal bool32
qt_insert(QuadTree *t, int32 index, Boid *bs, Arena *a) {
	CTU   *node = &t->root;
	int32 depth = 0;

	bool32 traversing = true;
	while (traversing) {
		printf("inserting (%d), currently at depth %d\n", index, depth);
		if (depth == MAX_DEPTH) {
			if (node->capacity == node->n) {
				int32 *old_indices = node->indices;
				node->capacity = node->capacity * 2;
				uint64 bufsize = sizeof(int32) * node->capacity;
				node->indices = (int32 *)arena_alloc(a, bufsize);
				memcpy(node->indices, old_indices, node->n);
			}
			node->indices[node->n] = index;
			node->n++;
			return true;
		}
		if (node->state == CTU_LEAF) {
			if (node->n < 4) {
				node->indices[node->n] = index;
				node->n++;
				return true;
			} else {
				int16 mx = node->size.x / 2;
				int16 my = node->size.y / 2;

				CTU *children = (CTU *)arena_alloc(a, (uint64)(4 * sizeof(CTU)));
				memset(children, 0, 4 * sizeof(CTU));
				for (int32 i = 0; i < 4; ++i) {
					children[i].size = int16v2_n(mx, my);
					node->capacity = 4;
					node->indices = (int32 *)arena_alloc(a, (uint64)(sizeof(int32) * 4));
				}
				children[QUADRANT_NE].o = int16v2_n(
					node->o.x + mx,
					node->o.y + my);
				children[QUADRANT_SE].o = int16v2_n(
					node->o.x + mx,
					node->o.y);
				children[QUADRANT_SW].o = int16v2_n(
					node->o.x,
					node->o.y);
				children[QUADRANT_NW].o = int16v2_n(
					node->o.x,
					node->o.y + my);

				for (int32 i = 0; i < 4; ++i) {
					vec2 pos_ = bs[i].pos;
					Quadrant quadrant = get_quadrant(node->o, node->size, pos_);
					CTU *child = node->children[quadrant];
					child->indices[child->n++] = node->indices[i];
				}
			}
		}

		Quadrant quadrant = get_quadrant(node->o, node->size, bs[index].pos);
		node = node->children[quadrant];
		depth++;
		traversing = false;
	}

	return false;
}





