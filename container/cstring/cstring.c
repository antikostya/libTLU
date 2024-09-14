
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

// ----------------------------------------------------------------------------
// api functions
void cstring_init(void)
{}

void cstring_fini(void)
{}
