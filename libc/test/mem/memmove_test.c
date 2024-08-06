
#include <core/common.h>
#include <libc/mem.h>
#include <utest/utest.h>
#include <utest/utils.h>

#include <string.h>
#include <stdlib.h>

UTEST(memmove_simple)
{
	const char *src = "12345";
	char dst[] = "xxxxx";
	const uint n = strlen(src);

	tlu_memcpy(dst, src, n);
	ASSERT_EQUAL_MEM(src, dst, n);
}

UTEST(memmove_intersect)
{
	char s[] = "12345678";
	const char *ans = "23456788";
	const uint n = strlen(s);

	tlu_memmove(s, s + 1, n - 1);
	ASSERT_EQUAL_MEM(ans, s, n);
}


UTEST(memmove_intersect2)
{
	char s[] = "12345678";
	const char *ans = "11234567";
	const uint n = strlen(s);

	tlu_memmove(s + 1, s, n - 1);
	ASSERT_EQUAL_MEM(ans, s, n);
}

UTEST(memmove_seq)
{
	const uint MAX_SIZE = 512;
	const uint MAX_OFFSET = 8;

	utest_progress_start();
	for (uint off1 = 0; off1 < MAX_OFFSET; off1++) {
		for (uint off2 = 0; off2 < MAX_OFFSET; off2++) {
			for (uint size = 0; size < MAX_SIZE; size++) {
				char *s_exp = utest_malloc(size + max(off1, off2));
				char *s_real = utest_malloc(size + max(off1, off2));

				utest_random_strings(s_exp + off1, s_real + off1, size);
				utest_random_strings(s_exp + off2, s_real + off2, size);

				tlu_memmove(s_real + off1, s_real + off2, size);
				memmove(s_exp + off1, s_real + off2, size);

				free(s_exp);
				free(s_real);

				utest_progress3(off1, off2, size, MAX_OFFSET, MAX_OFFSET, MAX_SIZE);
				if (size > 128)
					size += utest_random_range(1, 8);
			}
		}
	}
	utest_progress_done();
}

