
#ifndef _UTEST_INTERNAL_H_
#define _UTEST_INTERNAL_H_

#include <core/common.h>
#include <core/compiler.h>
#include <core/types.h>

struct __utest {
	const char *name;
	void (*func)(void);
	uint64 magic;
	bool skip;
};

#define __UTEST_MAGIC 0xDEADBEEFBADDADUL
#define __UTEST_ATTR __used __aligned(1) __section(".utest")

#define __UTEST_FUNC_NAME(__name) __UTEST_FUNCTION_##__name
#define __UTEST_STRUCT_NAME(__name) __UTEST_WITH_THIS_NAME_ALREADY_DEFINED_##__name

#define __UTEST_IMPL(__name, __skip)					\
	void __UTEST_FUNC_NAME(__name)(void);				\
									\
	struct __utest __UTEST_STRUCT_NAME(__name) __UTEST_ATTR = {	\
		.name = #__name,					\
		.func = &__UTEST_FUNC_NAME(__name),			\
		.magic = __UTEST_MAGIC,					\
		.skip = __skip						\
	};								\
									\
	void __UTEST_FUNC_NAME(__name)(void)

#define __assert_fail(...) __assert_fail_impl(__FILE__, __LINE__, ARG_OR_NULL(__VA_ARGS__))
#define __assert_bool(exp, real, ...) __assert_bool_impl(exp, real, __FILE__, __LINE__, ARG_OR_NULL(__VA_ARGS__))
#define __assert_equal(exp, real, eq, ...) __assert_eq_impl(exp, real, eq, __FILE__, __LINE__, ARG_OR_NULL(__VA_ARGS__))
#define __assert_gt(border, val, greater, equal, ...) __assert_gt_impl(border, val, greater, equal, __FILE__, __LINE__, ARG_OR_NULL(__VA_ARGS__))
#define __assert_ptr(exp, real, eq, ...) __assert_ptr_impl(exp, real, eq, __FILE__, __LINE__, ARG_OR_NULL(__VA_ARGS__))
#define __assert_str(exp, real, eq, ...) __assert_mem_impl(exp, real, (uint64)-1, eq, __FILE__, __LINE__, ARG_OR_NULL(__VA_ARGS__))
#define __assert_mem(exp, real, size, eq, ...) __assert_mem_impl(exp, real, size, eq, __FILE__, __LINE__, ARG_OR_NULL(__VA_ARGS__))
#define __assert_sign(exp, real, eq, ...) __assert_sign_impl(exp, real, eq, __FILE__, __LINE__, ARG_OR_NULL(__VA_ARGS__))

extern __cold __noret void __assert_fail_impl(const char *file, uint line, const char *format, ...);
extern void __assert_bool_impl(bool exp, bool real, const char *file, uint line, const char *format, ...);
extern void __assert_eq_impl(int64 exp, int64 real, bool eq, const char *file, uint line, const char *format, ...);
extern void __assert_gt_impl(int64 border, int64 val, bool greater, bool equal, const char *file, uint line, const char *format, ...);
extern void __assert_ptr_impl(const void *exp, const void *real, bool eq, const char *file, uint line, const char *format, ...);
extern void __assert_mem_impl(const char *exp, const char *real, uint64 size, bool eq, const char *file, uint line, const char *format, ...);
extern void __assert_sign_impl(int64 exp, int64 real, bool eq, const char *file, uint line, const char *format, ...);

#endif /* _UTEST_INTERNAL_H_ */

