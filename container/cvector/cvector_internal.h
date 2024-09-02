
#ifndef CVECTOR_INTERNAL_H
#define CVECTOR_INTERNAL_H

#include <container/cvector.h>
#include <core/common.h>
#include <core/compiler.h>
#include <core/errno.h>
#include <core/panic.h>
#include <libc/mem.h>

#if CONFIG_CVECTOR_VALIDATION_LAYER
# define CVECTOR_MAGIC 0xdeadbeefdeadbeef
#endif

#define cvector_entry(ptr) container_of(ptr, struct cvector, data)

struct cvector {
	uint64 capacity;
	uint64 size;
#if CONFIG_CVECTOR_VALIDATION_LAYER
	uint64 magic;
#endif
	__aligned(8) uint8 data[];
};

#endif /* CVECTOR_INTERNAL_H */

