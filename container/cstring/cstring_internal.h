
#ifndef CSTRING_INTERNAL_H
#define CSTRING_INTERNAL_H

#include <container/cstring.h>
#include <core/common.h>
#include <core/compiler.h>
#include <core/errno.h>
#include <core/panic.h>
#include <libc/mem.h>
#include <libc/string.h>

#if CONFIG_CSTRING_VALIDATION_LAYER
# define CSTRING_MAGIC 0xdeadbeef00000002
#endif

#define cstring_entry(ptr) container_of(ptr, struct cstring, data)

struct cstring {
	uint64 size;
#if CONFIG_CSTRING_VALIDATION_LAYER
	uint64 magic;
#endif
	__aligned(8) const char data[];
};

#endif /* CSTRING_INTERNAL_H */

