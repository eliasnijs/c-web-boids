
#define MAX_DEPTH   8 // TODO(Elias): set based on boids range
#define MAX_PER_CTU 4

enum Quadrant {
    QUADRANT_NE = 0,
    QUADRANT_SE = 1,
    QUADRANT_SW = 2,
    QUADRANT_NW = 3,
};
global_variable char *QUADRANT_STRING_TABLE[] = {
	"North-East",
	"South-East",
	"South-West",
	"North-West"
};

enum CTU_State {
	CTU_LEAF,
	CTU_NODE
};

struct CTU {
	vec2	  o;
	int16	  size;

	CTU_State state;

	union {
		CTU *children;
		struct {
			uint32 capacity;
			uint32 n;
			uint32 *indices;
		};
	};
};

struct QuadTree {
	CTU	root;
};

struct Square {
    vec2    origin;
    float32 size;
};

/////////////////////////////////////////////////////////////////////
//// Utility functions

internal Quadrant
get_quadrant(vec2 o, int16 size, vec2 pos) {
	int16 mx = o.x + size / 2;
	int16 my = o.y + size / 2;
	if (pos.x >= mx) {
		return (pos.y >= my) ? QUADRANT_NE : QUADRANT_SE;
	} else {
		return (pos.y >= my) ? QUADRANT_NW : QUADRANT_SW;
	}
}



/////////////////////////////////////////////////////////////////////
//// Implementation

internal void
qt_init(QuadTree *t, int16 size, Arena *a) {
	CTU *root = &t->root;
	root->state	= CTU_LEAF;
	root->o		= {0, 0};
	root->size	= size;
	root->n		= 0;
	root->capacity	= 4;
	root->indices	= (uint32 *)arena_alloc(a, sizeof(uint32) * 4);
}

internal bool32
qt_insert(QuadTree *t, uint32 index, Boid *bs, Arena *a) {
	CTU   *node = &t->root;
	uint32 depth = 0;

	uint32 traversing = 1;
	while (traversing) {
		if (depth == MAX_DEPTH) {
			if (node->capacity == node->n) {
				uint32 *old_indices = node->indices;
				node->capacity = node->capacity * 2;
				uint64 bufsize = sizeof(uint32) * node->capacity;
				node->indices = (uint32 *)arena_alloc(a, bufsize);
				memcpy(node->indices, old_indices, node->n * sizeof(uint32));
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
				int16 m_ = node->size / 2;

				CTU *children = (CTU *)arena_alloc(a, 4 * sizeof(CTU));
				for (int32 i = 0; i < 4; ++i) {
					children[i].size     = m_;
					children[i].state    = CTU_LEAF;
					children[i].n        = 0;
					children[i].capacity = MAX_PER_CTU;
					children[i].indices  = (uint32 *)arena_alloc(a, sizeof(int32) * MAX_PER_CTU);
				}
				children[QUADRANT_NE].o = {node->o.x + m_, node->o.y + m_};
				children[QUADRANT_SE].o = {node->o.x + m_, node->o.y};
				children[QUADRANT_NW].o = {node->o.x, node->o.y + m_};
				children[QUADRANT_SW].o = {node->o.x, node->o.y};

				for (int32 i = 0; i < node->n; ++i) {
					uint32 index_ = node->indices[i];
					Quadrant q = get_quadrant(node->o, node->size,
								  bs[index_].pos);
					CTU *child = &children[q];
					child->indices[child->n++] = index_;
				}

				node->children = children;
				node->state = CTU_NODE;
			}
		}

		Quadrant quadrant = get_quadrant(node->o, node->size, bs[index].pos);
		node = &node->children[quadrant];
		depth++;
		traversing++;
	}

	return false;
}

