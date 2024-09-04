
#include "cvector_internal.h"

// ----------------------------------------------------------------------------
// external functions
extern void *cvector_allocate(uint64 size);
extern void cvector_deallocate(void *ptr);
extern void cvector_panic(int errorcode);

extern void *malloc(uint64);
extern void free(void *);

__weak void *cvector_allocate(uint64 size)
{
	return malloc(size);
}

__weak void cvector_deallocate(void *ptr)
{
	free(ptr);
}

__weak __noret void cvector_panic(int errorcode)
{
	const char *messages[] = {
		[CVECTOR_PANIC_INVALID_POINTER] = "cvector: invalid pointer (magic check failed)",
		[CVECTOR_PANIC_OUT_OF_RANGE] = "cvector: out of range",
	};
	tlu_panic(messages[errorcode]);
}

// ----------------------------------------------------------------------------
// internal functions
void set_magic(struct cvector *cvector)
{
#if CONFIG_CVECTOR_VALIDATION_LAYER
	cvector->magic = CVECTOR_MAGIC;
#else
	(void)cvector;
#endif
}

void check_magic(const struct cvector *cvector)
{
#if CONFIG_CVECTOR_VALIDATION_LAYER
	if (cvector->magic != CVECTOR_MAGIC) {
		cvector_panic(CVECTOR_PANIC_INVALID_POINTER);
	}
#else
	(void)cvector;
#endif
}

void check_range(const struct cvector *cvector, uint64 idx)
{
	if (unlikely(idx >= cvector->size)) {
		cvector_panic(CVECTOR_PANIC_OUT_OF_RANGE);
	}
}

uint64 allocation_grid(uint64 size)
{
	return max(7u, size * 3u / 2u);
}

// ----------------------------------------------------------------------------
// api functions
void cvector_init(void)
{}

void cvector_fini(void)
{}

void *__cvector_create_impl(uint type_size, uint64 size, uint64 capacity)
{
	struct cvector *cvector;

	if (unlikely(size > capacity))
		return (void *)EINVAL;
	if (capacity == CVECTOR_CAPACITY_AUTO)
		capacity = allocation_grid(size);

	cvector = cvector_allocate(capacity * type_size + sizeof(struct cvector));
	if (unlikely(cvector == NULL))
		return (void *)ENOMEM;

	set_magic(cvector);
	cvector->capacity = capacity;
	cvector->size = size;

	return cvector->data;
}

void cvector_destroy(void *ptr)
{
	struct cvector *cvector = cvector_entry(ptr);

	check_magic(cvector);
	cvector_deallocate(cvector);
}

void *__cvector_copy_impl(void *ptr_other, uint type_size, uint64 capacity)
{
	struct cvector *other = cvector_entry(ptr_other);
	struct cvector *copy;

	check_magic(other);
	if (unlikely(capacity < other->size))
		return (void *)EINVAL;
	if (capacity == CVECTOR_CAPACITY_AUTO)
		capacity = allocation_grid(other->size);

	copy = cvector_allocate(type_size * capacity + sizeof(struct cvector));
	if (unlikely(copy == NULL))
		return (void *)ENOMEM;

	set_magic(copy);
	copy->capacity = capacity;
	copy->size = other->size;
	tlu_memcpy(copy->data, other->data, other->size * type_size);

	return copy->data;
}

void *__cvector_at_impl(const void *ptr, uint64 idx, void *ret)
{
	struct cvector *cvector = cvector_entry(ptr);

	check_magic(cvector);
	check_range(cvector, idx);

	return ret;
}

uint64 cvector_size(const void *ptr)
{
	struct cvector *cvector = cvector_entry(ptr);

	check_magic(cvector);
	return cvector->size;
}

uint64 cvector_capacity(const void *ptr)
{
	struct cvector *cvector = cvector_entry(ptr);

	check_magic(cvector);
	return cvector->capacity;
}

void *__cvector_insert_impl(void *ptr, uint type_size, void *pos)
{
	struct cvector *cvector = cvector_entry(ptr);
	uint64 idx = (pos - ptr) / type_size;

	check_magic(cvector);
	if (unlikely(cvector->size == cvector->capacity)) {
		return (void *)ENOSPACE;
	}

	if (cvector->size != idx) {
		check_range(cvector, idx);
		tlu_memmove(pos + type_size, pos, (cvector->size - idx) * type_size);
	}
	cvector->size++;

	return pos;
}

void *__cvector_erase_impl(void *ptr, uint type_size, void *pos)
{
	struct cvector *cvector = cvector_entry(ptr);
	uint64 idx = (pos - ptr) / type_size;

	check_magic(cvector);
	check_range(cvector, idx);

	cvector->size--;
	tlu_memmove(pos, pos + type_size, (cvector->size - idx) * type_size);

	return pos;
}

int __cvector_expand(void *ptr, uint type_size, uint64 capacity)
{
	void **vptr = ptr;
	struct cvector *old = cvector_entry(*vptr);
	struct cvector *vec;

	check_magic(old);

	if (unlikely(capacity < old->size))
		return EINVAL;
	else if (capacity == old->capacity)
		return 0;
	else if (capacity == CVECTOR_CAPACITY_AUTO)
		capacity = allocation_grid(capacity);

	vec = cvector_allocate(capacity * type_size + sizeof(struct cvector));
	if (unlikely(vec == NULL))
		return ENOMEM;

	tlu_memcpy(vec, old, old->size * type_size + sizeof(struct cvector));
	vec->capacity = capacity;

	*vptr = vec->data;
	cvector_deallocate(old);
	return 0;
}

