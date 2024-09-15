
#include "cstring_internal.h"

// ----------------------------------------------------------------------------
// external functions
extern void *cstring_allocate(uint64 size);
extern void cstring_deallocate(void *ptr);
extern __noret void cstring_panic(enum cstring_errorcodes errorcode);

extern void *malloc(uint64 size);
extern void free(void *ptr);

__weak void *cstring_allocate(uint64 size)
{
	return malloc(size);
}

__weak void cstring_deallocate(void *ptr)
{
	free(ptr);
}

__weak __noret void cstring_panic(enum cstring_errorcodes errorcode)
{
	const char *messages[] = {
		[CSTRING_PANIC_INVALID_POINTER] = "cstring: invalid pointer (magic check failed)",
		[CSTRING_PANIC_OUT_OF_RANGE] = "cstring: out of range",
	};
	tlu_panic(messages[errorcode]);
}

// ----------------------------------------------------------------------------
// internal functions
void set_magic(struct cstring *cstring)
{
#if CONFIG_CSTRING_VALIDATION_LAYER
	cstring->magic = CSTRING_MAGIC;
#else
	(void)cstring;
#endif
}

void check_magic(const struct cstring *cstring)
{
#if CONFIG_CSTRING_VALIDATION_LAYER
	if (cstring->magic != CSTRING_MAGIC) {
		cstring_panic(CSTRING_PANIC_INVALID_POINTER);
	}
#else
	(void)cstring;
#endif
}

void check_range(const struct cstring *cstring, uint64 idx)
{
	if (unlikely(idx >= cstring->size)) {
		cstring_panic(CSTRING_PANIC_OUT_OF_RANGE);
	}
}

uint64 allocation_grid(uint64 size)
{
	return max(7u, size * 3u / 2u);
}

// ----------------------------------------------------------------------------
// api functions
void cstring_init(void)
{}

void cstring_fini(void)
{}

const char *cstring_create(const char *orig)
{
	struct cstring *cstring;
	uint64 size = tlu_strlen(orig);

	cstring = cstring_allocate(size + sizeof(struct cstring));
	if (unlikely(cstring == NULL))
		return (void *)ENOMEM;

	set_magic(cstring);
	cstring->size = size;

	tlu_memcpy((void *)cstring->data, orig, size);

	return cstring->data;
}

void cstring_destroy(const char *ptr)
{
	struct cstring *cstring = cstring_entry(ptr);

	check_magic(cstring);
	cstring_deallocate(cstring);
}

const char *cstring_copy(const char *ptr_other)
{
	struct cstring *other = cstring_entry(ptr_other);
	struct cstring *cstring;

	check_magic(other);
	cstring = cstring_allocate(other->size + sizeof(struct cstring));
	if (unlikely(cstring == NULL))
		return (void *)ENOMEM;

	set_magic(cstring);
	cstring->size = other->size;

	tlu_memcpy((void *)cstring->data, ptr_other, other->size);

	return cstring->data;
}

char cstring_at(const char *ptr, uint64 idx)
{
	struct cstring *cstring = cstring_entry(ptr);

	check_magic(cstring);
	check_range(cstring, idx);

	return ptr[idx];
}

uint64 cstring_size(const char *ptr)
{
	struct cstring *cstring = cstring_entry(ptr);

	check_magic(cstring);
	return cstring->size;
}
