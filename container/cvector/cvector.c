
#include "cvector_internal.h"

#if CONFIG_CVECTOR_VALIDATION_LAYER
extern void cvector_panic(const char *message);

#else
__noret INTERNAL
void cvector_panic(const char *message)
{
	tlu_panic(message);
}
#endif

INTERNAL
uint64 allocation_grid_upper(uint64 size)
{
	return max(7u, size * 2);
}

INTERNAL
uint64 allocation_grid_lower(uint64 size)
{
	return min(7u, size / 2);
}

INTERNAL
bool cvector_needs_expand(uint64 size, uint64 allocated)
{
	return size == allocated;
}

INTERNAL
bool cvector_needs_shrink(uint64 size, uint64 allocated)
{
	return allocated >= 7u && allocated / size >= 4;
}

INTERNAL
struct cvector *reallocate(struct cvector *old, uint type_size, uint64 new_alloc)
{
	struct cvector *vec;

	vec = malloc(new_alloc * type_size + sizeof(struct cvector));
	if (unlikely(vec == NULL))
		return NULL;

	tlu_memcpy(vec, old, old->size * type_size + sizeof(struct cvector));
	vec->allocated = new_alloc;

	free(old);
	return vec;
}

INTERNAL
void set_magic(struct cvector *cvector)
{
#if CONFIG_CVECTOR_VALIDATION_LAYER
	cvector->magic = CVECTOR_MAGIC;
#else
	(void)cvector;
#endif
}

INTERNAL
void check_magic(const struct cvector *cvector)
{
#if CONFIG_CVECTOR_VALIDATION_LAYER
	if (cvector->magic != CVECTOR_MAGIC) {
		cvector_panic("cvector: invalid pointer (magic check fail)");
	}
#else
	(void)cvector;
#endif
}

INTERNAL
void check_range(const struct cvector *cvector, uint64 idx)
{
	if (idx >= cvector->size) {
		cvector_panic("cvector: out of range");
	}
}

void cvector_init(void)
{
}

void cvector_fini(void)
{
}

void *__cvector_create(uint type_size, uint64 size, cvector_create_flags_t flags)
{
	struct cvector *cvector;
	uint64 alloc;

	if (flags & CVECTOR_CREATE_EXACT_SIZE)
		alloc = size;
	else
		alloc = allocation_grid_upper(size);


	cvector = malloc(type_size * alloc + sizeof(struct cvector));
	if (unlikely(cvector == NULL))
		return NULL;

	set_magic(cvector);
	cvector->allocated = alloc;
	if (flags & CVECTOR_CREATE_ONLY_PREALLOC)
		cvector->size = 0;
	else
		cvector->size = size;

	if (flags & CVECTOR_CREATE_ZERO)
		tlu_memzero(cvector->data, type_size * cvector->size);

	return cvector->data;
}

void cvector_destroy(void *ptr)
{
	struct cvector *cvector = cvector_entry(ptr);

	check_magic(cvector);
	free(cvector);
}

void *__cvector_copy(const void *pother, uint type_size, cvector_copy_flags_t flags)
{
	struct cvector *other = cvector_entry(pother);
	struct cvector *copy;
	uint64 alloc;
	uint64 size;

	check_magic(other);
	alloc = other->allocated;
	size = other->size;

	if (flags & CVECTOR_COPY_EXACT_SIZE)
		alloc = size;
	if (flags & CVECTOR_COPY_EMPTY)
		size = 0;

	copy = malloc(type_size * alloc + sizeof(struct cvector));
	if (unlikely(copy == NULL))
		return NULL;

	set_magic(copy);
	copy->allocated = alloc;
	copy->size = size;

	tlu_memcpy(copy->data, other->data, size * type_size);
	return copy->data;
}

void *__cvector_create_from(const void *begin, const void *end, uint type_size, cvector_create_from_flags_t flags)
{
	struct cvector *cvector;
	uint64 alloc;
	uint64 size = (end - begin) / type_size;

	if (unlikely(begin > end))
		cvector_panic("cvector: end iterator less than begin iterator");

	if (flags & CVECTOR_CREATE_FROM_EXACT_SIZE)
		alloc = size;
	else
		alloc = allocation_grid_upper(size);

	cvector = malloc(type_size * alloc + sizeof(struct cvector));
	if (unlikely(cvector == NULL))
		return NULL;

	set_magic(cvector);
	cvector->allocated = alloc;
	cvector->size = size;

	tlu_memcpy(cvector->data, begin, size * type_size);
	return cvector->data;
}

void *__cvector_at(void *ptr, uint64 idx, void *ret)
{
	struct cvector *cvector = cvector_entry(ptr);

	check_magic(cvector);
	if (idx != cvector->size)
		check_range(cvector, idx);

	return ret;
}

void *__cvector_insert(void *vvptr, uint type_size, void *pos, cvector_insert_flags_t flags)
{
	void **vptr = vvptr;
	struct cvector *cvector = cvector_entry(*vptr);
	uint64 idx = (pos - *vptr) / type_size;

	check_magic(cvector);
	if (unlikely(cvector_needs_expand(cvector->size, cvector->allocated))) {
		struct cvector *vec;
		uint64 new_alloc;
		uint64 len;
		void *new_pos;

		if (unlikely(flags & CVECTOR_INSERT_NO_EXPAND))
			return (void *)ENOMODIFY;

		if (flags & CVECTOR_INSERT_EXPAND_EXACT_SIZE)
			new_alloc = cvector->size + 1;
		else
			new_alloc = allocation_grid_upper(cvector->size);

		vec = malloc(new_alloc * type_size + sizeof(struct cvector));
		if (unlikely(vec == NULL))
			return (void *)ENOMEM;

		/**
		 *
		 * before:
		 *                          end - pos
		 *                      <----------------->
		 * [metadata][0][1] ... [pos] ... [end - 1]
		 *   ^        ^           ^
		 *   cvector  *vptr       inserting before here
		 *
		 * after:
		 * [metadata][0][1] ... [pos][pos + 1] ... [end - 1]
		 *                        ^
		 *                        new entry
		 */
		tlu_memcpy(vec, cvector, pos - (void *)cvector);
		new_pos = vec->data + idx * type_size;
		len = (uint64)cvector->data + cvector->size * type_size - (uint64)pos;
		tlu_memcpy(new_pos + type_size, pos, len);

		vec->allocated = new_alloc;
		vec->size++;
		*vptr = vec->data;
		free(cvector);

		return new_pos;
	}

	if (cvector->size != idx)
		check_range(cvector, idx);
	tlu_memmove(pos + type_size, pos, (cvector->size - idx) * type_size);
	cvector->size++;

	return pos;
}

void *__cvector_erase(void *vvptr, uint type_size, void *pos, cvector_erase_flags_t flags)
{
	void **vptr = vvptr;
	struct cvector *cvector = cvector_entry(*vptr);
	uint64 idx = (pos - *vptr) / type_size;
	bool shrink = false;

	check_magic(cvector);
	check_range(cvector, idx);

	if (flags & CVECTOR_ERASE_NO_SHRINK)
		shrink = false;
	else if (flags & CVECTOR_ERASE_FORCE_SHRINK)
		shrink = true;
	else if (cvector_needs_shrink(cvector->size, cvector->allocated))
		shrink = true;

	if (shrink) {
		struct cvector *vec;
		uint64 new_size = cvector->size - 1;
		uint64 len;
		void *new_pos;

		vec = malloc(new_size * type_size + sizeof(struct cvector));
		if (unlikely(vec == NULL))
			return NULL;

		/**
		 *
		 * before:
		 *     pos - cvector          end - (pos + 1) = end - pos - 1
		 * <------------------->      <-------------------->
		 * [metadata][0][1] ... [pos][pos + 1] ... [end - 1]
		 *   ^        ^           ^
		 *   cvector  *vptr       removing value
		 *
		 * after:
		 * [metadata][0][1] ... [pos + 1] ... [end - 1]
		 */
		tlu_memcpy(vec, cvector, pos - (void *)cvector);
		new_pos = vec->data + idx * type_size;
		len = (uint64)cvector->data + new_size * type_size - (uint64)pos;
		tlu_memcpy(new_pos, pos + type_size, len);

		vec->allocated = new_size;
		vec->size--;
		*vptr = vec->data;
		free(cvector);

		return new_pos;
	}

	cvector->size--;
	tlu_memmove(pos, pos + type_size, (cvector->size - idx) * type_size);

	return pos;
}

int __cvector_extend(void *vvptr, void *vextend, uint type_size, cvector_extend_flags_t flags)
{
	void **vptr = vvptr;
	struct cvector *orig = cvector_entry(*vptr);
	struct cvector *extend = cvector_entry(vextend);

	check_magic(orig);
	check_magic(extend);

	if (orig->size + extend->size > orig->allocated) {
		uint64 new_alloc;
		uint64 move_size;
		struct cvector *vec;

		if (unlikely(flags & CVECTOR_EXTEND_NO_EXPAND))
			return ENOMODIFY;

		if (flags & CVECTOR_EXTEND_EXPAND_EXACT_SIZE)
			new_alloc = orig->size + extend->size;
		else
			new_alloc = allocation_grid_upper(orig->size + extend->size);

		vec = malloc(new_alloc * type_size + sizeof(struct cvector));
		if (unlikely(vec == NULL))
			return ENOMEM;

		move_size = orig->size * type_size + sizeof(struct cvector);
		tlu_memcpy(vec, orig, move_size);
		tlu_memcpy((void *)vec + move_size, extend->data, extend->size * type_size);

		vec->size = orig->size + extend->size;
		vec->allocated = new_alloc;
		*vptr = vec->data;
		free(orig);

		return 0;
	}

	tlu_memcpy(orig->data + orig->size * type_size, extend->data, extend->size * type_size);
	orig->size += extend->size;

	return 0;
}

int __cvector_shrink(void *vvptr, uint type_size)
{
	void **vptr = vvptr;
	struct cvector *cvector = cvector_entry(*vptr);

	cvector = reallocate(cvector, type_size, allocation_grid_lower(cvector->size));
	if (unlikely(cvector == NULL))
		return ENOMEM;

	*vptr = cvector->data;
	return EOK;
}

bool cvector_empty(const void *ptr)
{
	return cvector_size(ptr) == 0;
}

uint64 cvector_size(const void *ptr)
{
	const struct cvector *cvector = cvector_entry(ptr);

	check_magic(cvector);
	return cvector->size;
}

uint64 cvector_capacity(const void *ptr)
{
	const struct cvector *cvector = cvector_entry(ptr);

	check_magic(cvector);
	return cvector->allocated;
}

