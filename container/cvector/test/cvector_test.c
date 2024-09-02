
#include <container/cvector.h>
#include <utest/utest.h>
#include <utest/utils.h>

#include "../cvector_internal.h"
#include <setjmp.h>
#include <stdio.h>
#include <string.h>

#define ASSERT_PANIC(expr)				\
	do {						\
		__cvector_expected_panic = true;	\
		if (setjmp(jump_buf) == 0) {		\
			(void)(expr);			\
		}					\
		__cvector_expected_panic = false;	\
	} while (false)

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

static jmp_buf jump_buf;
static bool __cvector_expected_panic = false;

void cvector_panic(int errorcode)
{
	(void)errorcode;
	if (__cvector_expected_panic) {
		longjmp(jump_buf, 1);
	} else {
		ASSERT_FAIL();
	}
}

UTEST(cvector_create)
{
	const uint n = 31;
	int8 *v = cvector_create(int8, n, CVECTOR_CAPACITY_AUTO, 123);
	struct cvector *cv = cvector_entry(v);

	ASSERT_EQUAL_PTR(v, cv->data);
	ASSERT_EQUAL(n, cv->size);
	ASSERT_GREATER_EQUAL(n, cv->capacity);

	for (uint i = 0; i < n; i++) {
		ASSERT_EQUAL(123, v[i]);
	}

	cvector_destroy(v);
}

UTEST(cvector_create2)
{
	const uint n = 10;
	uint64 *v = cvector_create(uint64, n, 0, 0);

	ASSERT_EQUAL(EINVAL, PTR_ERR(v));
}

UTEST(cvector_create3)
{
	const uint n = 10;
	char **v = cvector_create(char *, n, n, NULL);
	struct cvector *cv = cvector_entry(v);

	ASSERT_EQUAL_PTR(v, cv->data);
	ASSERT_EQUAL(n, cv->size);
	ASSERT_EQUAL(n, cv->capacity);

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

UTEST(cvector_create4)
{
	const uint n = 7;
	struct test_struct *v = cvector_create(struct test_struct, n, n, {});
	struct cvector *cv = cvector_entry(v);

	ASSERT_EQUAL_PTR(v, cv->data);
	ASSERT_EQUAL(n, cv->size);
	ASSERT_EQUAL(n, cv->capacity);

	cvector_destroy(v);
}

UTEST(cvector_copy)
{
	uint16 *vec = cvector_create(uint16, 5, 5, 0);
	uint16 *copy;

	struct cvector *v, *c;

	for (uint i = 0; i < 5; i++) {
		vec[i] = i;
	}

	copy = cvector_copy(vec, 5);
	v = cvector_entry(vec);
	c = cvector_entry(copy);

	ASSERT_EQUAL(v->size, c->size);
	ASSERT_EQUAL(v->capacity, c->capacity);
	ASSERT_ZERO(memcmp(vec, copy, v->size * sizeof(uint16)));

	cvector_destroy(vec);
	cvector_destroy(copy);
}

UTEST(cvector_size)
{
	unsigned char *v1 = cvector_create(unsigned char, 0, CVECTOR_CAPACITY_AUTO, 0);
	unsigned char *v2 = cvector_create(unsigned char, 4, CVECTOR_CAPACITY_AUTO, 0);
	unsigned char *v3 = cvector_create(unsigned char, 0, 10, 0);

	ASSERT_EQUAL(0, cvector_size(v1));
	ASSERT_EQUAL(4, cvector_size(v2));
	ASSERT_EQUAL(0, cvector_size(v3));

	cvector_destroy(v1);
	cvector_destroy(v2);
	cvector_destroy(v3);
}

UTEST(cvector_empty)
{
	char *v1 = cvector_create(char, 0, 0, 0);
	char *v2 = cvector_create(char, 4, CVECTOR_CAPACITY_AUTO, 0);
	char *v3 = cvector_create(char, 0, 10, 0);

	ASSERT_TRUE(cvector_empty(v1));
	ASSERT_FALSE(cvector_empty(v2));
	ASSERT_TRUE(cvector_empty(v3));

	cvector_destroy(v1);
	cvector_destroy(v2);
	cvector_destroy(v3);
}


UTEST(cvector_capacity)
{
	unsigned char *v1 = cvector_create(unsigned char, 0, 0, 0);
	unsigned char *v2 = cvector_create(unsigned char, 4, CVECTOR_CAPACITY_AUTO, 0);
	unsigned char *v3 = cvector_create(unsigned char, 0, 10, 0);

	ASSERT_ZERO(cvector_capacity(v1));
	ASSERT_GREATER_EQUAL(4, cvector_capacity(v2));
	ASSERT_EQUAL(10, cvector_capacity(v3));

	cvector_destroy(v1);
	cvector_destroy(v2);
	cvector_destroy(v3);
}

UTEST(cvector_at)
{
	int *v = cvector_create(int, 3, 3, 0);

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

UTEST(cvector_front)
{
	int *v1 = cvector_create(int, 0, 0, 0);
	int *v2 = cvector_create(int, 3, 3, 0);

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
	int *v1 = cvector_create(int, 0, 0, 0);
	int *v2 = cvector_create(int, 3, 3, 0);

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
	int16 *v = cvector_create(int16, n, n, 0);
	int16 *iter;
	int i;

	for (i = 0; i < n; i++) {
		v[i] = i + 1;
	}

	i = 0;
	cvector_for_each(v, iter) {
		ASSERT_EQUAL(i + 1, *iter);
		ASSERT_LESS(n, i);
		i++;
	}

	cvector_destroy(v);
}

UTEST(cvector_for_each2)
{
	int *v = cvector_create(int, 0, 0, 0);
	int *iter;
	int i = 0;

	cvector_for_each(v, iter) {
		i++;
	}
	ASSERT_ZERO(i);

	cvector_destroy(v);
}

UTEST(cvector_insert)
{
	int *v = cvector_create(int, 0, 6, 0);

	int *it = cvector_insert(v, cvector_end(v), 1);
	ASSERT_EQUAL(1, *it);
	ASSERT_CVECTOR(v, "1");
	ASSERT_EQUAL(2, *cvector_insert(v, cvector_end(v), 2));
	ASSERT_CVECTOR(v, "12");
	ASSERT_EQUAL(3, *cvector_insert(v, cvector_end(v), 3));
	ASSERT_CVECTOR(v, "123");

	ASSERT_EQUAL(4, *cvector_insert(v, cvector_begin(v), 4));
	ASSERT_CVECTOR(v, "4123");
	ASSERT_EQUAL(5, *cvector_insert(v, cvector_begin(v), 5));
	ASSERT_CVECTOR(v, "54123");
	ASSERT_EQUAL(6, *cvector_insert(v, cvector_begin(v), 6));
	ASSERT_CVECTOR(v, "654123");

	ASSERT_EQUAL_PTR((void *)ENOSPACE, cvector_insert(v, cvector_begin(v), 1));

	cvector_destroy(v);
}

UTEST(cvector_insert2)
{
	int *v = cvector_create(int, 0, 0, 0);

	ASSERT_EQUAL_PTR((void *)ENOSPACE, cvector_insert(v, cvector_end(v), 1));

	cvector_destroy(v);
}

UTEST(cvector_insert3)
{
	int *v = cvector_create(int, 3, 4, 0);

	v[0] = 1;
	v[1] = 2;
	v[2] = 3;

	ASSERT_CVECTOR(v, "123");
	ASSERT_EQUAL(9, *cvector_insert(v, cvector_begin(v) + 2, 9));
	ASSERT_CVECTOR(v, "1293");

	cvector_destroy(v);
}

UTEST(cvector_insert4)
{
	int *v = cvector_create(int, 0, 1, 0);

	ASSERT_PANIC(cvector_insert(v, cvector_begin(v) + 10, 123));

	cvector_destroy(v);
}

UTEST(cvector_push_back)
{
	int *v = cvector_create(int, 0, 3, 0);

	ASSERT_EQUAL(1, *cvector_push_back(v, 1));
	ASSERT_CVECTOR(v, "1");

	ASSERT_EQUAL(2, *cvector_push_back(v, 2));
	ASSERT_CVECTOR(v, "12");

	ASSERT_EQUAL(3, *cvector_push_back(v, 3));
	ASSERT_CVECTOR(v, "123");

	ASSERT_EQUAL_PTR((void *)ENOSPACE, cvector_push_back(v, 123));

	cvector_destroy(v);
}

UTEST(cvector_push_front)
{
	int *v = cvector_create(int, 0, 3, 0);

	ASSERT_EQUAL(1, *cvector_push_front(v, 1));
	ASSERT_CVECTOR(v, "1");

	ASSERT_EQUAL(2, *cvector_push_front(v, 2));
	ASSERT_CVECTOR(v, "21");

	ASSERT_EQUAL(3, *cvector_push_front(v, 3));
	ASSERT_CVECTOR(v, "321");

	ASSERT_EQUAL_PTR((void *)ENOSPACE, cvector_push_front(v, 123));

	cvector_destroy(v);
}

UTEST(cvector_erase)
{
	int *v = cvector_create(int, 6, 6, 0);
	int *it;

	v[0] = 1;
	v[1] = 2;
	v[2] = 3;
	v[3] = 4;
	v[4] = 5;
	v[5] = 6;

	it = cvector_erase(v, cvector_rbegin(v));
	ASSERT_EQUAL_PTR(cvector_end(v), it);
	ASSERT_CVECTOR(v, "12345");

	it = cvector_erase(v, cvector_rbegin(v));
	ASSERT_EQUAL_PTR(cvector_end(v), it);
	ASSERT_CVECTOR(v, "1234");

	it = cvector_erase(v, cvector_rbegin(v));
	ASSERT_EQUAL_PTR(cvector_end(v), it);
	ASSERT_CVECTOR(v, "123");

	it = cvector_erase(v, cvector_begin(v));
	ASSERT_EQUAL_PTR(cvector_begin(v), it);
	ASSERT_CVECTOR(v, "23");

	it = cvector_erase(v, cvector_begin(v));
	ASSERT_EQUAL_PTR(cvector_begin(v), it);
	ASSERT_CVECTOR(v, "3");

	it = cvector_erase(v, cvector_begin(v));
	ASSERT_EQUAL_PTR(cvector_begin(v), it);
	ASSERT_CVECTOR(v, "");

	ASSERT_EQUAL(6, cvector_capacity(v));
	cvector_destroy(v);
}

UTEST(cvector_erase2)
{
	int16 *v = cvector_create(int16, 6, 6, 0);
	int16 *it;

	v[0] = 1;
	v[1] = 2;
	v[2] = 3;
	v[3] = 4;
	v[4] = 5;
	v[5] = 6;

	it = cvector_erase(v, cvector_begin(v) + 3);
	ASSERT_EQUAL_PTR(cvector_begin(v) + 3, it);
	ASSERT_CVECTOR(v, "12356");

	cvector_destroy(v);
}

UTEST(cvector_erase3)
{
	int16 *v = cvector_create(int16, 1, 1, 0);

	ASSERT_PANIC(cvector_erase(v, cvector_end(v) + 10));

	cvector_destroy(v);
}

UTEST(cvector_pop_back)
{
	int *v = cvector_create(int, 3, 3, 0);
	int *it;

	v[0] = 1;
	v[1] = 2;
	v[2] = 3;

	it = cvector_pop_back(v);
	ASSERT_EQUAL_PTR(cvector_end(v), it);
	ASSERT_CVECTOR(v, "12");

	it = cvector_pop_back(v);
	ASSERT_EQUAL_PTR(cvector_end(v), it);
	ASSERT_CVECTOR(v, "1");

	it = cvector_pop_back(v);
	ASSERT_EQUAL_PTR(cvector_end(v), it);
	ASSERT_CVECTOR(v, "");

	cvector_destroy(v);
}

UTEST(cvector_pop_back2)
{
	int *v = cvector_create(int, 0, 0, 0);

	ASSERT_PANIC(cvector_pop_back(v));

	cvector_destroy(v);
}

UTEST(cvector_pop_front)
{
	int *v = cvector_create(int, 3, 3, 0);
	int *it;

	v[0] = 1;
	v[1] = 2;
	v[2] = 3;

	it = cvector_pop_front(v);
	ASSERT_EQUAL_PTR(cvector_begin(v), it);
	ASSERT_CVECTOR(v, "23");

	it = cvector_pop_front(v);
	ASSERT_EQUAL_PTR(cvector_begin(v), it);
	ASSERT_CVECTOR(v, "3");

	it = cvector_pop_front(v);
	ASSERT_EQUAL_PTR(cvector_begin(v), it);
	ASSERT_CVECTOR(v, "");

	cvector_destroy(v);
}

UTEST(cvector_pop_front2)
{
	int *v = cvector_create(int, 0, 0, 0);

	ASSERT_PANIC(cvector_pop_front(v));

	cvector_destroy(v);
}

int main(int argc, const char **argv)
{
	(void)argc;
	cvector_init();
	unittest(argv);
	cvector_fini();
	return 0;
}

