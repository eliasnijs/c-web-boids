
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

	uint32     n;
	union {
		CTU *children;
		struct {
			uint32 capacity;
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
	int16 m = o.x + size / 2;
	if (pos.x >= m) {
		return (pos.y >= m) ? QUADRANT_NE : QUADRANT_SE;
	} else {
		return (pos.y >= m) ? QUADRANT_NW : QUADRANT_SW;
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
	root->indices	= (uint32 *)arena_alloc(a, (uint64)(sizeof(uint32) * 4));
}

internal bool32
qt_insert(QuadTree *t, int32 index, Boid *bs, Arena *a) {
	CTU   *node = &t->root;
	int32 depth = 0;

	bool32 traversing = 1;
	while (traversing) {
		if (depth == MAX_DEPTH) {
			if (node->capacity == node->n) {
				uint32 *old_indices = node->indices;
				node->capacity = node->capacity * 2;
				uint64 bufsize = sizeof(int32) * node->capacity;
				node->indices = (uint32 *)arena_alloc(a, bufsize);
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
				int16 m_ = node->size / 2;

				CTU *children = (CTU *)arena_alloc(a, (uint64)(4 * sizeof(CTU)));
				for (int32 i = 0; i < 4; ++i) {
					children[i].size     = m_;
					children[i].state    = CTU_LEAF;
					children[i].n        = 0;
					children[i].capacity = 4;
					children[i].indices  = (uint32 *)arena_alloc(a, (uint32)(sizeof(int32) * 4));
				}
				children[QUADRANT_NE].o = {node->o.x + m_, node->o.y + m_};
				children[QUADRANT_SE].o = {node->o.x + m_, node->o.y};
				children[QUADRANT_SW].o = {node->o.x, node->o.y};
				children[QUADRANT_NW].o = {node->o.x, node->o.y + m_};

				for (int32 i = 0; i < 1; ++i) {
					vec2 pos_  = bs[i].pos;
					Quadrant q = get_quadrant(node->o, node->size, pos_);
					CTU *child = &children[q];
					child->indices[child->n++] = node->indices[i];
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



