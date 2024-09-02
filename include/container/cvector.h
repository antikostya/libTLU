
#ifndef CONTAINER_CVECTOR_H
#define CONTAINER_CVECTOR_H

#include <core/compiler.h>
#include <core/errno.h>
#include <core/types.h>

// -----------------------------------------------------------------------------
// macro
#define CVECTOR_CAPACITY_AUTO (uint64)-1

enum {
	CVECTOR_PANIC_INVALID_POINTER = 1,
	CVECTOR_PANIC_OUT_OF_RANGE = 2,
};

// -----------------------------------------------------------------------------
// external
extern void *cvector_allocate(uint64 size);
extern void cvector_deallocate(void *ptr);
extern __noret void cvector_panic(int errorcode);

// -----------------------------------------------------------------------------
// initialization
void cvector_init(void);
void cvector_fini(void);

// -----------------------------------------------------------------------------
// construction
#define cvector_create(type, size, capacity, fill) __cvector_create(type, size, capacity, fill)
void cvector_destroy(void *vector);

#define cvector_copy(other, capacity) __cvector_copy(other, capacity)

// -----------------------------------------------------------------------------
// element access
#define cvector_at(vector, idx) __cvector_at(vector, idx)
#define __cvector_rat(vector, idx) __cvector_at(vector, cvector_size(vector) - idx - 1)

#define cvector_front(vector) __cvector_at(vector, 0)
#define cvector_back(vector) __cvector_rat(vector, 0)

// -----------------------------------------------------------------------------
// capacity
uint64 cvector_size(const void *vector);
#define cvector_empty(vector) (cvector_size(vector) == 0)
uint64 cvector_capacity(const void *vector);

// -----------------------------------------------------------------------------
// modifiers
#define cvector_insert(vector, pos, value) __cvector_insert(vector, pos, value)
#define cvector_push_back(vector, value) __cvector_push_back(vector, value)
#define cvector_push_front(vector, value) __cvector_push_front(vector, value)

#define cvector_erase(vector, pos) __cvector_erase(vector, pos)
#define cvector_pop_back(vector) __cvector_erase(vector, cvector_rbegin(vector))
#define cvector_pop_front(vector) __cvector_erase(vector, cvector_begin(vector))

// -----------------------------------------------------------------------------
// resizing
#define cvector_expand(pvector, capacity) __cvector_expand(pvector, sizeof(**(pvector)), capacity)
#define cvector_shrink(pvector) __cvector_expand(pvector, sizeof(**(pvector)), cvector_size(*(pvector)))

// -----------------------------------------------------------------------------
// iterators
#define cvector_begin(vector) (vector)
#define cvector_end(vector) ((vector) + cvector_size(vector))

#define cvector_rbegin(vector) ((vector) + cvector_size(vector) - 1)
#define cvector_rend(vector) ((vector) - 1)

// -----------------------------------------------------------------------------
// traverse
#define cvector_for_each(vector, iter) __cvector_for_each(vector, iter)

// ============================================================================
// internal macros
#define __cvector_create(type, size, capacity, fill)					\
	({										\
		type *vec = __cvector_create_impl(sizeof(type), size, capacity);	\
		type *iter;								\
		if (likely(!PTR_ERR(vec))) {						\
			cvector_for_each(vec, iter) {					\
				*iter = (typeof(*iter))fill;				\
			}								\
		}									\
		vec;									\
	})

#define __cvector_copy(other, capacity) \
	(typeof(other))__cvector_copy_impl(other, sizeof(*(other)), capacity)

#define __cvector_at(vector, idx) *(typeof(vector))__cvector_at_impl(vector, idx, (vector) + idx)

#define __cvector_insert(vector, pos, value)						\
	({										\
		assert_same_ptr_type(vector, pos);					\
		typeof(pos) ret = __cvector_insert_impl(vector, sizeof(*(vector)), pos);\
		if (likely(!PTR_ERR(ret))) {						\
			*ret = (value);							\
		}									\
		ret;									\
	})

#define __cvector_push_back(vector, value) \
	__cvector_insert(vector, cvector_end(vector), value)

#define __cvector_push_front(vector, value) \
	__cvector_insert(vector, cvector_begin(vector), value)

#define __cvector_erase(vector, pos)							\
	({										\
		assert_same_ptr_type(vector, pos);					\
		(typeof(pos))__cvector_erase_impl(vector, sizeof(*(vector)), pos);	\
	})

#define __cvector_for_each(vector, iter)		\
	for (assert_same_ptr_type(vector, iter),	\
	     (iter) = cvector_begin(vector); 		\
	     (iter) != cvector_end(vector);		\
	     (iter)++)

extern void *__cvector_create_impl(uint type_size, uint64 size, uint64 capacity);
extern void *__cvector_copy_impl(void *other, uint type_size, uint64 capacity);
extern void *__cvector_at_impl(const void *vector, uint64 idx, void *ret);
extern void *__cvector_insert_impl(void *vector, uint type_size, void *pos);
extern void *__cvector_erase_impl(void *vector, uint type_size, void *pos);

#endif /* CONTAINER_CVECTOR_H */

