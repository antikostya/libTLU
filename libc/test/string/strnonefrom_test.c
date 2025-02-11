
#include <core/common.h>
#include <libc/string.h>
#include <utest/utest.h>
#include <utest/utils.h>

#define _GNU_SOURCE
#include <string.h>
#include <stdlib.h>

UTEST(strnonefrom_simple)
{
	const char *s = "12x456x78910x";

	ASSERT_EQUAL_PTR(s + 2, tlu_strnonefrom(s, "zxcvb"));
}

UTEST(strnonefrom_simple2)
{
	const char *s = "123456";

	ASSERT_NULL(tlu_strnonefrom(s, "qwerty"));
	ASSERT_EQUAL_PTR(s, tlu_strnonefrom(s, "123456"));
	ASSERT_EQUAL_PTR(s + 5, tlu_strnonefrom(s, "6"));
}

UTEST(strnonefrom_simple3)
{
	ASSERT_NULL(tlu_strnonefrom("123", ""));
	ASSERT_NULL(tlu_strnonefrom("", ""));
	ASSERT_NULL(tlu_strnonefrom("", "123"));
}

UTEST(strnonefrom_seq)
{
	const uint MAX_SIZE = 512;
	const uint MAX_OFFSET = 8;

	utest_progress_start();
	for (uint off1 = 0; off1 < MAX_OFFSET; off1++) {
		for (uint off2 = 0; off2 < MAX_OFFSET; off2++) {
			for (uint size = 0; size < MAX_SIZE; size++) {
				char *s = utest_malloc(size + max(off1, off2) + 1);

				utest_random_string(s, size + max(off1, off2));
				s[max(off1, off2) + size] = '\0';

				const void *real = tlu_strnonefrom(s + off1, s + off2);
				const char *exp = strpbrk(s + off1, s + off2);
				ASSERT_EQUAL_PTR(exp, real);

				free(s);
				utest_progress3(off1, off2, size, MAX_OFFSET, MAX_OFFSET, MAX_SIZE);
			}
		}
	}
	utest_progress_done();
}

