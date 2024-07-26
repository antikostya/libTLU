
#ifndef CVECTOR_INTERNAL_H
#define CVECTOR_INTERNAL_H

#include <container/cvector.h>
#include <core/compiler.h>
#include <core/defs.h>
#include <core/errno.h>
#include <core/panic.h>
#include <libc/mem.h>

#if CONFIG_CVECTOR_VALIDATION_LAYER
# include <core/color.h>
# include <core/panic.h>
# define CVECTOR_MAGIC 0xdeadbeefdeadbeef
#endif

#define cvector_entry(ptr) container_of(ptr, struct cvector, data)

struct cvector {
	uint64 allocated;
	uint64 size;
#if CONFIG_CVECTOR_VALIDATION_LAYER
	uint64 magic;
#endif
	__aligned(8) uint8 data[];
};

extern void *malloc(uint64 size);
extern void free(void *ptr);

#ifdef CONFIG_DEBUG
# define INTERNAL __used noinline
#else
# define INTERNAL static
#endif

uint64 allocation_grid_upper(uint64 size);
uint64 allocation_grid_lower(uint64 size);
bool cvector_needs_expand(uint64 size, uint64 allocated);
bool cvector_needs_shrink(uint64 size, uint64 allocated);
struct cvector *reallocate(struct cvector *old, uint type_size, uint64 new_alloc);
void set_magic(struct cvector *cvector);
void check_magic(const struct cvector *cvector);
void check_range(const struct cvector *cvector, uint64 idx);

#endif /* CVECTOR_INTERNAL_H */

