
#ifndef CORE_COMMON_H
#define CORE_COMMON_H

#include <core/compiler.h>
#include <core/types.h>

#define __cmp_once(x, y, op)            \
	({                              \
		typeof(x) __x = (x);    \
		typeof(y) __y = (y);    \
		__x op __y ? __x : __y; \
	})

#define max(x, y) __cmp_once(x, y, >)
#define min(x, y) __cmp_once(x, y, <)

#define sign(x)                                   \
	({                                        \
		typeof(x) __x = (x);              \
		__x > 0 ? 1 : (__x < 0 ? -1 : 0); \
	})

#define swap(a, b)                   \
	do {                         \
		typeof(a) tmp = (a); \
		(a) = (b);           \
		(b) = (tmp);         \
	} while (0)

#define container_of(ptr, type, member) \
        ((type *)((uint64)(const void *)(ptr) - offsetof(type, member)))

#define ARRAY_SIZE(x) (sizeof(x) / sizeof(*(x)))

#define __VA_CLEAR(...)
#define __VA_SAME(...) __VA_ARGS__
#define __IF_EMPTY(...) __VA_SAME __VA_OPT__(()__VA_CLEAR)
#define __IF_NOT_EMPTY(...) __VA_CLEAR __VA_OPT__(()__VA_SAME)

#define ARG_OR(val, ...) __IF_EMPTY(__VA_ARGS__)(val) __VA_ARGS__
#define ARG_OR_NULL(...) ARG_OR(0, __VA_ARGS__)

#endif /* CORE_COMMON_H */

