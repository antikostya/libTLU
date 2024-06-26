
#include <core/rwonce.h>
#include <libc/mem.h>

void tlu_memset_secure(void *vptr, unsigned char c, uint64 size)
{
	unsigned char *ptr = vptr;

	for (uint64 i = 0; i < size; ++i) {
		WRITE_ONCE(ptr[i], c);
	}
}

