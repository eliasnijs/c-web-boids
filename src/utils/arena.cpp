////////////////////////////////////////////////////////////////////////////////
//// Memory Arena

/* Memory Arena:
 * One big container, allocations always happen at the end.
 * The memory is freed all at once. (memory is persistent until
 * the arena is destroyed) */

/* Example Usage:
 *
 * | Arena arena;
 * | uint8 arena_buffer[1024];
 * | arena_init(&arena, arena_buffer, sizeof(arena_buffer));
 * |
 * | uint64 *ptr = (uint64 *)arena_alloc(&arena, sizeof(uint64));
 * | *ptr = 42;
 * |
 * | arena_reset(&arena);
 * |
 * | uint64 *ptr2 = (uint64 *)arena_alloc(&arena, sizeof(uint64));
 * | *ptr2 = 42;
 * |
 * | // ptr == ptr2
 *
 */

struct Arena {
	uint8  *buf;          /* ptr to the reserved memory bufer for the arena */
	uint64 len;           /* length of the reserved memory */
	uint64 offset_curr;   /* current offset into the bufer */
	uint64 offset_prev;   /* previous offset into the bufer */
};

/* default memory alignment, = 16, checkout (`Streaming
 * SIMD Extensions` for the why) */
#ifndef MEM_DEFAULT_ALIGNMENT
#define MEM_DEFAULT_ALIGNMENT (2*sizeof(void *))
#endif

////////////////////////////////////////////////////////////////////////////////
//// Helper Functions

/* returns if an address is on a power off 2 */
internal bool32
is_pow_of_two(uintptr_t addr) {
	return (addr & (addr - 1)) == 0;
}

/* push a pointer to the next aligned value */
internal uintptr_t
align_forward(uintptr_t p, uint64 align) {
	uintptr_t a, m;

	//Assert( is_pow_of_two(align) );

	a = (uintptr_t)align;
	/* equivalent to p % a, but faster. (works because 'a' is a power of 2) */
	m = p & (a - 1);
	if (m)
		p += a - m;

	return p;
}

////////////////////////////////////////////////////////////////////////////////
//// Memory Arena Implementation

/* initialise an arena */
internal void
arena_init(Arena *a, void *backbuffer, uint64 l) {
	a->buf = (uint8 *)backbuffer;
	a->len = l;
	a->offset_curr = 0;
	a->offset_prev = 0;
}

/* destroy an arena */
internal inline void
arena_reset(Arena *a) {
	a->offset_curr = 0;
	a->offset_prev = 0;
}

/* allocate memory in a memory arena with alignment specified */
/* IMPORTANT(Elias): memory is not guaranteed to be 0! */
internal void *
arena_alloc_align(Arena *a, uint64 len, uint64 align) {
	printf("allocating %lu bytes in arena (full? %d)\n", len, a->offset_curr + len > a->len);
	if (a->len < a->offset_curr + len) {
		printf("allocating %lu bytes in arena (full? %d)\n", len, a->offset_curr + len > a->len);
		fprintf(stderr, "error: arena out of memory, allocation failed\n");
		return 0x0;
	}

	void *ptr = 0x0;
	uintptr_t ptr_curr = (uintptr_t)a->buf + (uintptr_t)a->offset_curr;
	uintptr_t offset = align_forward(ptr_curr, align) - (uintptr_t)a->buf;

	if (offset + len <= a->len)
	{
		ptr = &a->buf[offset];
		a->offset_prev = offset;
		a->offset_curr = offset + len;
	}

	return ptr;
}

/* allocate memory in a memory arena */
/* IMPORTANT(Elias): memory is not guaranteed to be 0! */
internal void *
arena_alloc(Arena *a, uint64 len) {
	return arena_alloc_align(a, len, MEM_DEFAULT_ALIGNMENT);
}

////////////////////////////////////////////////////////////////////////////////
//// References

/* (c) This code is based on ideas and code from:
 * 1. Base [4]: Memory Management, Mr. 4th Programming by Allen Webster
 *    https://www.youtube.com/watch?v=L79vSP8yV2g
 * 2. Memory Allocation Strategies by GingerBill,
 *    https://www.gingerbill.org/series/memory-allocation-strategies/
 * 3. Handmade Hero eps034 by Casey Muratori
 *    https://www.youtube.com/watch?v=IJYTwhqfKLg
 * 4. Untangling Lifetimes: The Arena Allocator by Ryan Fleury
 *    https://www.rfleury.com/p/untangling-lifetimes-the-arena-allocator */

