
#include <container/cvector.h>
#include <utest/utest.h>
#include <utest/utils.h>

#include "../cvector_internal.h"
#include <setjmp.h>
#include <stdio.h>
#include <string.h>

#if CONFIG_CVECTOR_VALIDATION_LAYER
static jmp_buf jump_buf;
#define ASSERT_PANIC(expr)				\
	do {						\
		__cvector_expected_panic = true;	\
		if (setjmp(jump_buf) == 0) {		\
			(void)(expr);			\
		}					\
		__cvector_expected_panic = false;	\
	} while (false)
#else
# define ASSERT_PANIC(expr) /* nothing */
#endif

#define ASSERT_CVECTOR(vec, values)								\
	do {											\
		typeof(vec) iter;								\
		uint i = 0;									\
		bool ok = true;									\
		ASSERT_EQUAL(strlen(values), cvector_size(vec), "ASSERT_CVECTOR: (size)");	\
		cvector_for_each(vec, iter) {							\
			if ((int64)values[i] - '0' != (int64)*iter) {				\
				ok = false;							\
				break;								\
			}									\
			i++;									\
		}										\
		i = 0;										\
		if (!ok) {									\
			utest_print_yellow("\nASSERT_CVECTOR: expected:\n");			\
			for (uint i = 0; i < strlen(values); i++) {				\
				utest_print_blue("%c ", values[i]);				\
			}									\
			utest_print_yellow("\nASSERT_CVECTOR: got:\n");				\
			cvector_for_each(vec, iter) {						\
				if (values[i] - '0' != (int64)*iter) {				\
					utest_print_red("%ld ", (int64)*iter);			\
				} else {							\
					utest_print_blue("%ld ", (int64)*iter);			\
				}								\
			}									\
			printf("\n");								\
			ASSERT_FAIL("ASSERT_CVECTOR");						\
		}										\
	} while (false)

#define direct_cmp(a, b) ((a) != (b))

#define touch_memory(x) memset(&x, 0, sizeof(x))

static bool __cvector_expected_panic = false;

void cvector_panic(const char *message)
{
	if (__cvector_expected_panic) {
		longjmp(jump_buf, 1);
	} else {
		ASSERT_FAIL(message);
	}
}

static int64 int64_cmp(int64 a, int64 b)
{
	return a != b;
}

UTEST(cvector_create)
{
	const uint n = 31;
	int8 *v = cvector_create(int8, n, 0);
	struct cvector *cv = cvector_entry(v);

	ASSERT_EQUAL_PTR(v, cv->data);
	ASSERT_EQUAL(n, cv->size);
	ASSERT_GREATER_EQUAL(n, cv->allocated);
	check_magic(cv);

	for (uint i = 0; i < n; i++) {
		touch_memory(v[i]);
	}

	cvector_destroy(v);
}

UTEST(cvector_create2)
{
	const uint n = 10;
	uint64 *v = cvector_create(uint64, n, 0);
	struct cvector *cv = cvector_entry(v);

	ASSERT_EQUAL_PTR(v, cv->data);
	ASSERT_EQUAL(n, cv->size);
	ASSERT_GREATER_EQUAL(n, cv->allocated);
	check_magic(cv);

	for (uint i = 0; i < n; i++) {
		touch_memory(v[i]);
	}

	cvector_destroy(v);
}

UTEST(cvector_create_ext)
{
	const uint n = 10;
	char **v = cvector_create(char *, n, CVECTOR_CREATE_ZERO);
	struct cvector *cv = cvector_entry(v);

	ASSERT_EQUAL_PTR(v, cv->data);
	ASSERT_EQUAL(n, cv->size);
	ASSERT_GREATER_EQUAL(n, cv->allocated);
	check_magic(cv);

	for (uint i = 0; i < n; i++) {
		ASSERT_NULL(v[i]);
	}

	cvector_destroy(v);
}

struct test_struct {
	uint64 q;
	double w;
	char *e;
};

UTEST(cvector_create_ext2)
{
	const uint n = 7;
	struct test_struct *v = cvector_create(struct test_struct, n, CVECTOR_CREATE_EXACT_SIZE);
	struct cvector *cv = cvector_entry(v);

	ASSERT_EQUAL_PTR(v, cv->data);
	ASSERT_EQUAL(n, cv->size);
	ASSERT_EQUAL(n, cv->allocated);
	check_magic(cv);

	for (uint i = 0; i < n; i++) {
		touch_memory(v[i]);
	}

	cvector_destroy(v);
}

UTEST(cvector_create_ext3)
{
	const uint n = 10;
	double *v = cvector_create(double, n, CVECTOR_CREATE_ONLY_PREALLOC);
	struct cvector *cv = cvector_entry(v);

	ASSERT_EQUAL(0, cv->size);
	ASSERT_GREATER_EQUAL(n, cv->allocated);

	for (uint i = 0; i < n; i++) {
		touch_memory(v[i]);
	}

	cvector_destroy(v);
}

UTEST(cvector_create_ext4)
{
	int16 *v = cvector_create(int16, 0, CVECTOR_CREATE_ONLY_PREALLOC | CVECTOR_CREATE_EXACT_SIZE);
	struct cvector *cv = cvector_entry(v);

	ASSERT_EQUAL(0, cv->size);
	ASSERT_EQUAL(0, cv->allocated);

	cvector_destroy(v);
}

UTEST(cvector_copy)
{
	uint16 *v = cvector_create(uint16, 5, 0);
	uint16 *c;

	struct cvector *cv;
	struct cvector *cc;

	for (uint i = 0; i < 5; i++) {
		v[i] = i;
	}

	c = cvector_copy(v, 0);
	cv = cvector_entry(v);
	cc = cvector_entry(c);

	ASSERT_EQUAL(cv->size, cc->size);
	ASSERT_EQUAL(cv->allocated, cc->allocated);
	ASSERT_ZERO(memcmp(c, v, cv->size * sizeof(uint16)));

	cvector_destroy(v);
	cvector_destroy(c);
}

UTEST(cvector_copy_ext)
{
	int32 *v = cvector_create(int32, 13, 0);
	struct cvector *cv = cvector_entry(v);

	for (uint i = 0; i < 13; i++) {
		v[i] = i * 10 + i;
	}

	{
		int32 *c1 = cvector_copy(v, CVECTOR_COPY_EMPTY);
		struct cvector *cc1 = cvector_entry(c1);

		ASSERT_EQUAL(0, cc1->size);
		ASSERT_EQUAL(cv->allocated, cc1->allocated);

		cvector_destroy(c1);
	}
	{
		int32 *c2 = cvector_copy(v, CVECTOR_COPY_EXACT_SIZE);
		struct cvector *cc2 = cvector_entry(c2);

		ASSERT_EQUAL(cv->size, cc2->size);
		ASSERT_EQUAL(cv->size, cc2->allocated);
		ASSERT_ZERO(memcmp(c2, v, cv->size * sizeof(int32)));

		cvector_destroy(c2);
	}
	{
		int32 *c3 = cvector_copy(v, CVECTOR_COPY_EXACT_SIZE | CVECTOR_COPY_EMPTY);
		struct cvector *cc3 = cvector_entry(c3);

		ASSERT_EQUAL(0, cc3->size);
		ASSERT_EQUAL(cv->size, cc3->allocated);

		cvector_destroy(c3);
	}
	cvector_destroy(v);
}

UTEST(cvector_create_from)
{
	int64 init[] = {1, 2, 3, 4, 5};
	const uint n = ARRAY_SIZE(init);
	int64 *v = cvector_create_from(init, init + n, 0);
	struct cvector *cv = cvector_entry(v);

	ASSERT_EQUAL(n, cv->size);
	ASSERT_GREATER_EQUAL(n, cv->allocated);
	ASSERT_ZERO(memcmp(init, v, sizeof(int64) * n));

	cvector_destroy(v);
}

UTEST(cvector_create_from2)
{
	const void *begin = (const void *)200ul;
	const void *end = (const void *)100ul;

	ASSERT_PANIC(cvector_create_from(begin, end, 0));
}

UTEST(cvector_create_from3)
{
	int *a = cvector_create(int, 3, 0);
	int *b;

	a[0] = 1;
	a[1] = 2;
	a[2] = 3;

	ASSERT_CVECTOR(a, "123");
	b = cvector_create_from(cvector_begin(a), cvector_end(a), 0);
	ASSERT_CVECTOR(b, "123");

	cvector_destroy(a);
	cvector_destroy(b);
}

UTEST(cvector_create_from_ext)
{
	const uint n = 9;
	int8 *c = cvector_create(int8, n, 0);
	int8 *v;
	struct cvector *cv;

	for (uint i = 0; i < n; i++) {
		c[i] = i * 10 + i;
	}
	v = cvector_create_from(c, c + n, CVECTOR_CREATE_FROM_EXACT_SIZE);
	cv = cvector_entry(v);

	ASSERT_EQUAL(n, cv->size);
	ASSERT_EQUAL(n, cv->allocated);
	ASSERT_ZERO(memcmp(c, v, sizeof(int8) * n));

	cvector_destroy(c);
	cvector_destroy(v);
}

UTEST(cvector_create_from_list)
{
	uint8 *v0 = cvector_create_from_list(uint8, 0, {9, 8, 7});
	int *v1 = cvector_create_from_list(int, CVECTOR_CREATE_FROM_EXACT_SIZE, {1, 2, 3, 4});
	uint64 *v2 = cvector_create_from_list(uint64, 0, {3, 2, 1});
	const char **v3 = cvector_create_from_list(const char *, 0, {"lol", "kek"});

	ASSERT_CVECTOR(v0, "987");
	ASSERT_CVECTOR(v1, "1234");
	ASSERT_CVECTOR(v2, "321");

	ASSERT_EQUAL(2, cvector_size(v3));
	ASSERT_ZERO(strcmp(v3[0], "lol"));
	ASSERT_ZERO(strcmp(v3[1], "kek"));

	cvector_destroy(v0);
	cvector_destroy(v1);
	cvector_destroy(v2);
	cvector_destroy(v3);
}

UTEST(cvector_size)
{
	unsigned char *v1 = cvector_create(unsigned char, 0, 0);
	unsigned char *v2 = cvector_create(unsigned char, 4, 0);
	unsigned char *v3 = cvector_create(unsigned char, 10, CVECTOR_CREATE_ONLY_PREALLOC);

	ASSERT_EQUAL(0, cvector_size(v1));
	ASSERT_EQUAL(4, cvector_size(v2));
	ASSERT_EQUAL(0, cvector_size(v3));

	cvector_destroy(v1);
	cvector_destroy(v2);
	cvector_destroy(v3);
}

UTEST(cvector_empty)
{
	char *v1 = cvector_create(char, 0, 0);
	char *v2 = cvector_create(char, 4, 0);
	char *v3 = cvector_create(char, 10, CVECTOR_CREATE_ONLY_PREALLOC);

	ASSERT_TRUE(cvector_empty(v1));
	ASSERT_FALSE(cvector_empty(v2));
	ASSERT_TRUE(cvector_empty(v3));

	cvector_destroy(v1);
	cvector_destroy(v2);
	cvector_destroy(v3);
}


UTEST(cvector_capacity)
{
	unsigned char *v1 = cvector_create(unsigned char, 0, 0);
	unsigned char *v2 = cvector_create(unsigned char, 4, 0);
	unsigned char *v3 = cvector_create(unsigned char, 10, CVECTOR_CREATE_EXACT_SIZE);

	ASSERT_GREATER_EQUAL(0, cvector_capacity(v1));
	ASSERT_GREATER_EQUAL(4, cvector_capacity(v2));
	ASSERT_EQUAL(10, cvector_capacity(v3));

	cvector_destroy(v1);
	cvector_destroy(v2);
	cvector_destroy(v3);
}

UTEST(cvector_at)
{
	int *v = cvector_create(int, 3, CVECTOR_CREATE_EXACT_SIZE);

	v[0] = 1;
	v[1] = 2;
	v[2] = 3;

	ASSERT_EQUAL(1, cvector_at(v, 0));
	ASSERT_EQUAL(2, cvector_at(v, 1));
	ASSERT_EQUAL(3, cvector_at(v, 2));

	ASSERT_PANIC(cvector_at(v, 3));
	ASSERT_PANIC(cvector_at(v, -1));

	cvector_at(v, 1) = 123;
	ASSERT_EQUAL(123, cvector_at(v, 1));

	cvector_destroy(v);
}

UTEST(cvector_rat)
{
	int *v = cvector_create(int, 3, CVECTOR_CREATE_EXACT_SIZE);

	v[0] = 1;
	v[1] = 2;
	v[2] = 3;

	ASSERT_EQUAL(3, cvector_rat(v, 0));
	ASSERT_EQUAL(2, cvector_rat(v, 1));
	ASSERT_EQUAL(1, cvector_rat(v, 2));

	ASSERT_PANIC(cvector_rat(v, 3));
	ASSERT_PANIC(cvector_rat(v, -1));

	cvector_rat(v, 1) = 123;
	ASSERT_EQUAL(123, cvector_rat(v, 1));

	cvector_destroy(v);
}

UTEST(cvector_front)
{
	int *v1 = cvector_create(int, 0, CVECTOR_CREATE_EXACT_SIZE);
	int *v2 = cvector_create(int, 3, CVECTOR_CREATE_EXACT_SIZE);

	v2[0] = 1;
	v2[1] = 2;
	v2[2] = 3;

	ASSERT_EQUAL(1, cvector_front(v2));
	ASSERT_PANIC(cvector_front(v1));

	cvector_front(v2) = 321;
	ASSERT_EQUAL(321, cvector_front(v2));

	cvector_destroy(v1);
	cvector_destroy(v2);
}

UTEST(cvector_back)
{
	int *v1 = cvector_create(int, 0, CVECTOR_CREATE_EXACT_SIZE);
	int *v2 = cvector_create(int, 3, CVECTOR_CREATE_EXACT_SIZE);

	v2[0] = 1;
	v2[1] = 2;
	v2[2] = 3;

	ASSERT_EQUAL(3, cvector_back(v2));
	ASSERT_PANIC(cvector_front(v1));

	cvector_back(v2) = 321;
	ASSERT_EQUAL(321, cvector_back(v2));

	cvector_destroy(v1);
	cvector_destroy(v2);
}

UTEST(cvector_for_each)
{
	const int n = 15;
	int16 *v = cvector_create(int16, n, CVECTOR_CREATE_EXACT_SIZE);
	int16 *iter;
	int i;

	for (i = 0; i < n; i++) {
		v[i] = i + 1;
	}

	i = 0;
	cvector_for_each(v, iter) {
		ASSERT_EQUAL(i + 1, *iter);
		ASSERT_LESS(15, i);
		i++;
	}

	cvector_destroy(v);
}

UTEST(cvector_for_each2)
{
	int *v = cvector_create(int, 0, CVECTOR_CREATE_EXACT_SIZE);
	int *iter;
	int i = 0;

	cvector_for_each(v, iter) {
		i++;
	}
	ASSERT_ZERO(i);

	cvector_destroy(v);
}

UTEST(cvector_for_each_reverse)
{
	const int n = 15;
	int16 *v = cvector_create(int16, n, CVECTOR_CREATE_EXACT_SIZE);
	int16 *iter;
	int i;

	for (i = 0; i < n; i++) {
		v[i] = i + 1;
	}

	i = 15;
	cvector_for_each_reverse(v, iter) {
		ASSERT_EQUAL(i, *iter);
		ASSERT_GREATER_EQUAL(0, i);
		i--;
	}

	cvector_destroy(v);
}

UTEST(cvector_for_each_reverse2)
{
	int *v = cvector_create(int, 0, CVECTOR_CREATE_EXACT_SIZE);
	int *iter;
	int i = 0;

	cvector_for_each_reverse(v, iter) {
		i++;
	}
	ASSERT_ZERO(i);

	cvector_destroy(v);
}

UTEST(cvector_find)
{
	const uint n = 15;
	int16 *v = cvector_create(int16, n, CVECTOR_CREATE_EXACT_SIZE);
	int16 *iter;

	for (uint i = 0; i < n; i++) {
		v[i] = i + 1;
	}

	for (uint i = 1; i <= n; i++) {
		iter = cvector_find(v, (uint64)i, direct_cmp);
		ASSERT_EQUAL_PTR(v + i - 1, iter);
		ASSERT_EQUAL(i, *iter);
	}

	ASSERT_NULL(cvector_find(v, (uint64)123, direct_cmp));

	cvector_destroy(v);
}

UTEST(cvector_find2)
{
	const uint n = 15;
	int64 *v = cvector_create(int64, n, CVECTOR_CREATE_EXACT_SIZE);
	int64 *iter;

	for (uint i = 0; i < n; i++) {
		v[i] = i + 1;
	}

	for (uint i = 1; i <= n; i++) {
		iter = cvector_find(v, (uint8)i, int64_cmp);
		ASSERT_EQUAL_PTR(v + i - 1, iter);
		ASSERT_EQUAL(i, *iter);
	}

	ASSERT_NULL(cvector_find(v, (uint8)0, int64_cmp));

	cvector_destroy(v);
}

UTEST(cvector_find3)
{
	const char *items[] = { "1", "2", "3", "4", "5" };
	char f[] = "3";
	const char **v = cvector_create(const char *, 5, CVECTOR_CREATE_EXACT_SIZE);
	const char **iter;
	uint i;

	for (i = 0; i < 5; i++) {
		v[i] = items[i];
	}

	iter = cvector_find(v, f, strcmp);
	ASSERT_EQUAL_PTR(v + 2, iter);
	ASSERT_ZERO(strcmp(*iter, "3"));

	ASSERT_NULL(cvector_find(v, "lol", strcmp));

	cvector_destroy(v);
}

UTEST(cvector_find4)
{
	char *v = cvector_create(char, 5, CVECTOR_CREATE_EXACT_SIZE);
	char *iter;
	memcpy(v, "12321", 5);

	iter = cvector_find(v, '2', direct_cmp);
	ASSERT_EQUAL_PTR(v + 1, iter);
	ASSERT_EQUAL('2', *iter);

	ASSERT_NULL(cvector_find(v, 'x', direct_cmp));

	cvector_destroy(v);
}

UTEST(cvector_rfind)
{
	const uint n = 15;
	int16 *v = cvector_create(int16, n, CVECTOR_CREATE_EXACT_SIZE);
	int16 *iter;

	for (uint i = 0; i < n; i++) {
		v[i] = i + 1;
	}

	for (uint i = 1; i <= n; i++) {
		iter = cvector_rfind(v, (uint64)i, direct_cmp);
		ASSERT_EQUAL_PTR(v + i - 1, iter);
		ASSERT_EQUAL(i, *iter);
	}

	ASSERT_NULL(cvector_rfind(v, (uint64)123, direct_cmp));

	cvector_destroy(v);
}

UTEST(cvector_rfind2)
{
	const uint n = 15;
	int64 *v = cvector_create(int64, n, CVECTOR_CREATE_EXACT_SIZE);
	int64 *iter;

	for (uint i = 0; i < n; i++) {
		v[i] = i + 1;
	}

	for (uint i = 1; i <= n; i++) {
		iter = cvector_rfind(v, (uint8)i, int64_cmp);
		ASSERT_EQUAL_PTR(v + i - 1, iter);
		ASSERT_EQUAL(i, *iter);
	}

	ASSERT_NULL(cvector_rfind(v, (uint8)0, int64_cmp));

	cvector_destroy(v);
}

UTEST(cvector_rfind3)
{
	const char *items[] = { "1", "2", "3", "4", "5" };
	char f[] = "3";
	const char **v = cvector_create(const char *, 5, CVECTOR_CREATE_EXACT_SIZE);
	const char **iter;
	uint i;

	for (i = 0; i < 5; i++) {
		v[i] = items[i];
	}

	iter = cvector_rfind(v, f, strcmp);
	ASSERT_EQUAL_PTR(v + 2, iter);
	ASSERT_ZERO(strcmp(*iter, "3"));

	ASSERT_NULL(cvector_rfind(v, "lol", strcmp));

	cvector_destroy(v);
}

UTEST(cvector_rfind4)
{
	char *v = cvector_create(char, 5, CVECTOR_CREATE_EXACT_SIZE);
	char *iter;
	memcpy(v, "12321", 5);

	iter = cvector_rfind(v, '2', direct_cmp);
	ASSERT_EQUAL_PTR(v + 3, iter);
	ASSERT_EQUAL('2', *iter);

	ASSERT_NULL(cvector_rfind(v, 'x', direct_cmp));

	cvector_destroy(v);
}

UTEST(cvector_contains)
{
	const uint n = 15;
	int16 *v = cvector_create(int16, n, CVECTOR_CREATE_EXACT_SIZE);

	for (uint i = 0; i < n; i++) {
		v[i] = i + 1;
	}

	for (uint i = 1; i <= n; i++) {
		ASSERT_TRUE(cvector_contains(v, i, direct_cmp));
	}

	ASSERT_FALSE(cvector_contains(v, 123, direct_cmp));
	ASSERT_FALSE(cvector_contains(v, 0, direct_cmp));

	cvector_destroy(v);
}

UTEST(cvector_count)
{
	const uint n = 15;
	int16 *v = cvector_create(int16, n, CVECTOR_CREATE_EXACT_SIZE);

	for (uint i = 0; i < n; i++) {
		v[i] = i + 1;
	}

	for (uint i = 1; i <= n; i++) {
		ASSERT_EQUAL(1, cvector_count(v, (uint64)i, direct_cmp));
	}

	ASSERT_ZERO(cvector_contains(v, (uint64)123, direct_cmp));
	ASSERT_ZERO(cvector_contains(v, (uint64)0, direct_cmp));

	cvector_destroy(v);
}

UTEST(cvector_count2)
{
	const uint n = 15;
	int64 *v = cvector_create(int64, n, CVECTOR_CREATE_EXACT_SIZE);

	for (uint i = 0; i < n; i++) {
		v[i] = i + 1;
	}

	for (uint i = 1; i <= n; i++) {
		ASSERT_EQUAL(1, cvector_count(v, (uint8)i, int64_cmp));
	}

	ASSERT_EQUAL(0, cvector_count(v, (uint8)123, int64_cmp));
	ASSERT_EQUAL(0, cvector_count(v, (uint8)0, int64_cmp));

	cvector_destroy(v);
}

UTEST(cvector_count3)
{
	const char *items[] = { "1", "2", "3", "4", "5" };
	char f[] = "3";
	const char **v = cvector_create(const char *, 5, CVECTOR_CREATE_EXACT_SIZE);
	uint i;

	for (i = 0; i < 5; i++) {
		v[i] = items[i];
	}

	ASSERT_EQUAL(1, cvector_count(v, f, strcmp));

	cvector_destroy(v);
}

UTEST(cvector_count4)
{
	char *v = cvector_create(char, 5, CVECTOR_CREATE_EXACT_SIZE);
	memcpy(v, "12321", 5);

	ASSERT_EQUAL(0, cvector_count(v, '0', direct_cmp));
	ASSERT_EQUAL(2, cvector_count(v, '1', direct_cmp));
	ASSERT_EQUAL(2, cvector_count(v, '2', direct_cmp));
	ASSERT_EQUAL(1, cvector_count(v, '3', direct_cmp));
	ASSERT_EQUAL(0, cvector_count(v, '4', direct_cmp));

	cvector_destroy(v);
}

UTEST(cvector_insert)
{
	int *v = cvector_create(int, 0, CVECTOR_CREATE_EXACT_SIZE);

	int *it = cvector_insert(&v, cvector_end(v), 1, 0);
	ASSERT_EQUAL(1, *it);
	ASSERT_CVECTOR(v, "1");
	ASSERT_EQUAL(2, *cvector_insert(&v, cvector_end(v), 2, 0));
	ASSERT_CVECTOR(v, "12");
	ASSERT_EQUAL(3, *cvector_insert(&v, cvector_end(v), 3, 0));
	ASSERT_CVECTOR(v, "123");

	ASSERT_EQUAL(4, *cvector_insert(&v, cvector_begin(v), 4, 0));
	ASSERT_CVECTOR(v, "4123");
	ASSERT_EQUAL(5, *cvector_insert(&v, cvector_begin(v), 5, 0));
	ASSERT_CVECTOR(v, "54123");
	ASSERT_EQUAL(6, *cvector_insert(&v, cvector_begin(v), 6, 0));
	ASSERT_CVECTOR(v, "654123");

	cvector_destroy(v);
}

UTEST(cvector_insert2)
{
	int *v = cvector_create(int, 0, CVECTOR_CREATE_EXACT_SIZE);

	ASSERT_EQUAL(1, *cvector_insert(&v, cvector_end(v), 1, 0));
	ASSERT_CVECTOR(v, "1");

	cvector_destroy(v);
}

UTEST(cvector_insert3)
{
	int *v = cvector_create_from_list(int, CVECTOR_CREATE_FROM_EXACT_SIZE, {1, 2, 3});

	ASSERT_CVECTOR(v, "123");
	ASSERT_EQUAL(9, *cvector_insert(&v, cvector_begin(v) + 2, 9, 0));
	ASSERT_CVECTOR(v, "1293");

	cvector_destroy(v);
}

UTEST(cvector_insert_ext)
{
	int *a = cvector_create_from_list(int, CVECTOR_CREATE_FROM_EXACT_SIZE, {1, 2, 3});
	int *b = cvector_create(int, 0, CVECTOR_CREATE_EXACT_SIZE);
	int *it;

	it = cvector_insert(&a, cvector_end(a), 5, CVECTOR_INSERT_NO_EXPAND);
	ASSERT_EQUAL_PTR((void *)ENOMODIFY, it);
	it = cvector_insert(&b, cvector_end(b), 5, CVECTOR_INSERT_NO_EXPAND);
	ASSERT_EQUAL_PTR((void *)ENOMODIFY, it);

	cvector_destroy(a);
	cvector_destroy(b);
}

UTEST(cvector_insert_ext2)
{
	int *v = cvector_create(int, 0, CVECTOR_CREATE_EXACT_SIZE);

	ASSERT_EQUAL(1, *cvector_insert(&v, cvector_end(v), 1, CVECTOR_INSERT_EXPAND_EXACT_SIZE));
	ASSERT_EQUAL(cvector_size(v), cvector_capacity(v));
	ASSERT_CVECTOR(v, "1");

	ASSERT_EQUAL(2, *cvector_insert(&v, cvector_end(v), 2, CVECTOR_INSERT_EXPAND_EXACT_SIZE));
	ASSERT_EQUAL(cvector_size(v), cvector_capacity(v));
	ASSERT_CVECTOR(v, "12");

	ASSERT_EQUAL(3, *cvector_insert(&v, cvector_end(v), 3, CVECTOR_INSERT_EXPAND_EXACT_SIZE));
	ASSERT_EQUAL(cvector_size(v), cvector_capacity(v));
	ASSERT_CVECTOR(v, "123");

	cvector_destroy(v);
}

UTEST(cvector_push_back)
{
	int *v = cvector_create(int, 0, CVECTOR_CREATE_EXACT_SIZE);

	ASSERT_EQUAL(1, *cvector_push_back(&v, 1, 0));
	ASSERT_CVECTOR(v, "1");

	ASSERT_EQUAL(2, *cvector_push_back(&v, 2, 0));
	ASSERT_CVECTOR(v, "12");

	ASSERT_EQUAL(3, *cvector_push_back(&v, 3, 0));
	ASSERT_CVECTOR(v, "123");

	cvector_destroy(v);
}

UTEST(cvector_push_back2)
{
	int *v = cvector_create_from_list(int, CVECTOR_CREATE_FROM_EXACT_SIZE, {1, 2, 3});

	ASSERT_EQUAL(9, *cvector_push_back(&v, 9, 0));
	ASSERT_CVECTOR(v, "1239");

	ASSERT_EQUAL(8, *cvector_push_back(&v, 8, 0));
	ASSERT_CVECTOR(v, "12398");

	ASSERT_EQUAL(7, *cvector_push_back(&v, 7, 0));
	ASSERT_CVECTOR(v, "123987");

	cvector_destroy(v);
}

UTEST(cvector_push_front)
{
	int *v = cvector_create(int, 0, CVECTOR_CREATE_EXACT_SIZE);

	ASSERT_EQUAL(1, *cvector_push_front(&v, 1, 0));
	ASSERT_CVECTOR(v, "1");

	ASSERT_EQUAL(2, *cvector_push_front(&v, 2, 0));
	ASSERT_CVECTOR(v, "21");

	ASSERT_EQUAL(3, *cvector_push_front(&v, 3, 0));
	ASSERT_CVECTOR(v, "321");

	cvector_destroy(v);
}

UTEST(cvector_push_front2)
{
	int *v = cvector_create_from_list(int, CVECTOR_CREATE_FROM_EXACT_SIZE, {1, 2, 3});

	ASSERT_EQUAL(9, *cvector_push_front(&v, 9, 0));
	ASSERT_CVECTOR(v, "9123");

	ASSERT_EQUAL(8, *cvector_push_front(&v, 8, 0));
	ASSERT_CVECTOR(v, "89123");

	ASSERT_EQUAL(7, *cvector_push_front(&v, 7, 0));
	ASSERT_CVECTOR(v, "789123");

	cvector_destroy(v);
}

UTEST(cvector_erase)
{
	int *v = cvector_create_from_list(int, CVECTOR_CREATE_FROM_EXACT_SIZE, {1, 2, 3, 4, 5, 6});
	int *it;

	it = cvector_erase(&v, cvector_rbegin(v), 0);
	ASSERT_EQUAL_PTR(cvector_end(v), it);
	ASSERT_CVECTOR(v, "12345");

	it = cvector_erase(&v, cvector_rbegin(v), 0);
	ASSERT_EQUAL_PTR(cvector_end(v), it);
	ASSERT_CVECTOR(v, "1234");

	it = cvector_erase(&v, cvector_rbegin(v), 0);
	ASSERT_EQUAL_PTR(cvector_end(v), it);
	ASSERT_CVECTOR(v, "123");

	it = cvector_erase(&v, cvector_begin(v), 0);
	ASSERT_EQUAL_PTR(cvector_begin(v), it);
	ASSERT_CVECTOR(v, "23");

	it = cvector_erase(&v, cvector_begin(v), 0);
	ASSERT_EQUAL_PTR(cvector_begin(v), it);
	ASSERT_CVECTOR(v, "3");

	it = cvector_erase(&v, cvector_begin(v), 0);
	ASSERT_EQUAL_PTR(cvector_begin(v), it);
	ASSERT_CVECTOR(v, "");

	cvector_destroy(v);
}

UTEST(cvector_erase2)
{
	int16 *v = cvector_create_from_list(int16, CVECTOR_CREATE_FROM_EXACT_SIZE, {1, 2, 3, 4, 5, 6});
	int16 *it;

	it = cvector_erase(&v, cvector_begin(v) + 3, 0);
	ASSERT_EQUAL_PTR(cvector_begin(v) + 3, it);
	ASSERT_CVECTOR(v, "12356");

	cvector_destroy(v);
}

UTEST(cvector_erase_ext)
{
	int *v = cvector_create(int, 100, CVECTOR_CREATE_ONLY_PREALLOC);
	int *it;
	uint64 cap;

	ASSERT_EQUAL(0, cvector_size(v));
	cap = cvector_capacity(v);
	ASSERT_GREATER_EQUAL(100, cap);

	ASSERT_EQUAL(4, *cvector_insert(&v, cvector_end(v), 4, CVECTOR_INSERT_NO_EXPAND));
	ASSERT_EQUAL(cap, cvector_capacity(v));
	ASSERT_CVECTOR(v, "4");

	ASSERT_EQUAL(5, *cvector_insert(&v, cvector_end(v), 5, CVECTOR_INSERT_NO_EXPAND));
	ASSERT_EQUAL(cap, cvector_capacity(v));
	ASSERT_CVECTOR(v, "45");

	it = cvector_erase(&v, cvector_rbegin(v), CVECTOR_ERASE_NO_SHRINK);
	ASSERT_EQUAL_PTR(it, cvector_end(v));
	ASSERT_CVECTOR(v, "4");
	ASSERT_EQUAL(cap, cvector_capacity(v));

	it = cvector_erase(&v, cvector_rbegin(v), 0);
	ASSERT_EQUAL_PTR(it, cvector_end(v));
	ASSERT_CVECTOR(v, "");
	ASSERT_LESS_EQUAL(cap, cvector_capacity(v));

	cvector_destroy(v);
}

UTEST(cvector_pop_back)
{
	int *v = cvector_create_from_list(int, CVECTOR_CREATE_FROM_EXACT_SIZE, {1, 2, 3});
	int *it;

	it = cvector_pop_back(&v, 0);
	ASSERT_EQUAL_PTR(cvector_end(v), it);
	ASSERT_CVECTOR(v, "12");

	it = cvector_pop_back(&v, 0);
	ASSERT_EQUAL_PTR(cvector_end(v), it);
	ASSERT_CVECTOR(v, "1");

	it = cvector_pop_back(&v, 0);
	ASSERT_EQUAL_PTR(cvector_end(v), it);
	ASSERT_CVECTOR(v, "");

	cvector_destroy(v);
}

UTEST(cvector_pop_back2)
{
	int *v = cvector_create_from_list(int, CVECTOR_CREATE_FROM_EXACT_SIZE, {1, 2, 3, 9, 8, 7});
	int *it;

	it = cvector_pop_back(&v, 0);
	ASSERT_EQUAL_PTR(cvector_end(v), it);
	ASSERT_CVECTOR(v, "12398");

	it = cvector_pop_back(&v, 0);
	ASSERT_EQUAL_PTR(cvector_end(v), it);
	ASSERT_CVECTOR(v, "1239");

	it = cvector_pop_back(&v, 0);
	ASSERT_EQUAL_PTR(cvector_end(v), it);
	ASSERT_CVECTOR(v, "123");

	cvector_destroy(v);
}

UTEST(cvector_pop_front)
{
	int *v = cvector_create_from_list(int, CVECTOR_CREATE_FROM_EXACT_SIZE, {1, 2, 3});
	int *it;

	it = cvector_pop_front(&v, 0);
	ASSERT_EQUAL_PTR(cvector_begin(v), it);
	ASSERT_CVECTOR(v, "23");

	it = cvector_pop_front(&v, 0);
	ASSERT_EQUAL_PTR(cvector_begin(v), it);
	ASSERT_CVECTOR(v, "3");

	it = cvector_pop_front(&v, 0);
	ASSERT_EQUAL_PTR(cvector_begin(v), it);
	ASSERT_CVECTOR(v, "");

	cvector_destroy(v);
}

UTEST(cvector_pop_front2)
{
	int *v = cvector_create_from_list(int, CVECTOR_CREATE_FROM_EXACT_SIZE, {1, 2, 3, 7, 8, 9});
	int *it;

	it = cvector_pop_front(&v, 0);
	ASSERT_EQUAL_PTR(cvector_begin(v), it);
	ASSERT_CVECTOR(v, "23789");

	it = cvector_pop_front(&v, 0);
	ASSERT_EQUAL_PTR(cvector_begin(v), it);
	ASSERT_CVECTOR(v, "3789");

	it = cvector_pop_front(&v, 0);
	ASSERT_EQUAL_PTR(cvector_begin(v), it);
	ASSERT_CVECTOR(v, "789");

	cvector_destroy(v);
}

UTEST(cvector_extend)
{
	int *a = cvector_create_from_list(int, CVECTOR_CREATE_FROM_EXACT_SIZE, {1, 2, 3});
	int *b = cvector_create_from_list(int, CVECTOR_CREATE_FROM_EXACT_SIZE, {4, 5, 6});

	ASSERT_ZERO(cvector_extend(&a, b, CVECTOR_EXTEND_EXPAND_EXACT_SIZE));
	ASSERT_CVECTOR(a, "123456");

	cvector_destroy(a);
	cvector_destroy(b);
}

UTEST(cvector_extend2)
{
	int16 *a = cvector_create(int16, 4, CVECTOR_CREATE_ONLY_PREALLOC);
	int16 *b = cvector_create_from_list(int16, 0, {1, 2, 3, 4});

	ASSERT_ZERO(cvector_extend(&a, b, CVECTOR_EXTEND_NO_EXPAND));
	ASSERT_CVECTOR(a, "1234");

	cvector_destroy(a);
	cvector_destroy(b);
}

UTEST(cvector_extend3)
{
	int64 *a = cvector_create(int64, 0, CVECTOR_CREATE_EXACT_SIZE);
	int64 *b = cvector_create(int64, 0, CVECTOR_CREATE_EXACT_SIZE);

	ASSERT_ZERO(cvector_extend(&a, b, 0));
	ASSERT_CVECTOR(a, "");

	cvector_destroy(a);
	cvector_destroy(b);
}

UTEST(cvector_extend4)
{
	int *a = cvector_create_from_list(int, CVECTOR_CREATE_FROM_EXACT_SIZE, {1, 2, 3});
	int *b = cvector_create_from_list(int, CVECTOR_CREATE_FROM_EXACT_SIZE, {4, 5, 6});

	ASSERT_EQUAL(ENOMODIFY, cvector_extend(&a, b, CVECTOR_EXTEND_NO_EXPAND));
	ASSERT_CVECTOR(a, "123");

	cvector_destroy(a);
	cvector_destroy(b);
}

int main(int argc, const char **argv)
{
	(void)argc;
	cvector_init();
	unittest(argv);
	cvector_fini();
	return 0;
}

