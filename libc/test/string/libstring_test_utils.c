
#include <libc/test/libc_test_utils.h>
#include <libc/test/libstring_test.h>
#include <utest/utest.h>
#include <utest/utils.h>
#include <core/panic.h>

#include <libc/string.h>

#include <string.h>
#include <stdio.h>

static bool sstartswith(const char *str, const char *pattern)
{
	size_t ss = strlen(str);
	size_t ps = strlen(pattern);

	if (ss < ps) {
		return false;
	}
	return strstr(str, pattern) == str;
}

static bool sendswith(const char *str, const char *pattern)
{
	size_t a = strlen(str);
	size_t b = strlen(pattern);

	if (b > a) {
		return false;
	}
	return strncmp(str + a - b, pattern, b + 1) == 0;
}

static void memzero(void *str, size_t len)
{
	memset(str, 0, len);
}

static int utest_string_callback(struct string_context *context)
{
	intmax_t real_ret, expected_ret;
	const void *real_p, *expected_p;
	int ret;

	char *rsrc = (char *)context->real_src;
	char *rdst = (char *)context->real_dst;
	char *esrc = (char *)context->expected_src;
	char *edst = (char *)context->expected_dst;
	size_t size = context->size;
	size_t offset = context->offset;

	switch (context->function) {
	case FUNC_STRLEN:
		esrc[context->needle] = 0;
		rsrc[context->needle] = 0;
		BUG_ON(NULL == memchr(esrc + offset, 0, size + 1));

		real_ret = (intmax_t)tlu_strlen(rsrc + offset);
		expected_ret = (intmax_t)strlen(esrc + offset);
		ASSERT_EQUAL(expected_ret, real_ret);
		return NEXT_OFFSET;

	case FUNC_STRCMP:
		esrc[context->needle] = 0;
		rsrc[context->needle] = 0;
		BUG_ON(NULL == memchr(esrc + offset, 0, size + 1));

		switch (context->state) {
		case 0:
			real_ret = tlu_strcmp(rsrc + offset, rdst);
			expected_ret = strcmp(esrc + offset, edst);
			ret = NEXT_OFFSET_OR_STATE;
			break;
		case 1:
			real_ret = tlu_strcmp(rsrc, rdst + offset);
			expected_ret = strcmp(esrc, edst + offset);
			ret = NEXT_OFFSET_OR_STATE;
			break;
		case 2:
			real_ret = tlu_strcmp(rsrc + offset, rsrc + offset);
			expected_ret = strcmp(esrc + offset, esrc + offset);
			ret = NEXT_OFFSET;
			break;
		default:
			BUG("utest::strcmp: invalid state");
		}

		ASSERT_EQUAL_SIGN(expected_ret, real_ret);
		return ret;

	case FUNC_STREQ:
		esrc[context->needle] = 0;
		rsrc[context->needle] = 0;
		BUG_ON(NULL == memchr(esrc + offset, 0, size + 1));

		switch (context->state) {
		case 0:
			real_ret = tlu_streq(rsrc + offset, rdst);
			expected_ret = strcmp(esrc + offset, edst);
			ret = NEXT_OFFSET_OR_STATE;
			break;
		case 1:
			real_ret = tlu_streq(rsrc, rdst + offset);
			expected_ret = strcmp(esrc, edst + offset);
			ret = NEXT_OFFSET_OR_STATE;
			break;
		case 2:
			real_ret = tlu_streq(rsrc + offset, rsrc + offset);
			expected_ret = strcmp(esrc + offset, esrc + offset);
			ret = NEXT_OFFSET;
			break;
		default:
			BUG("utest::strcmp: invalid state");
		}

		expected_ret = (expected_ret == 0);
		ASSERT_EQUAL(expected_ret, real_ret);
		return ret;

	case FUNC_STRCHR:
		esrc[context->needle] = 0;
		rsrc[context->needle] = 0;
		BUG_ON(NULL == memchr(esrc + offset, esrc[context->needle], size + 1));

		switch (context->state) {
		case 0:
			real_p = tlu_strchr(rsrc + offset, rsrc[context->needle]);
			expected_p = strchr(esrc + offset, esrc[context->needle]);
			ret = NEXT_OFFSET_OR_STATE;
			break;
		case 1:
			real_p = tlu_strchr(rsrc + offset, 0);
			expected_p = strchr(esrc + offset, 0);
			ret = NEXT_OFFSET;
			break;
		default:
			BUG("utest::strchr: invalid state");
		}

		if (expected_p == NULL) {
			ASSERT_NULL(real_p);
		} else {
			expected_ret = (intptr_t)expected_p - (intptr_t)esrc;
			real_ret = (intptr_t)real_p - (intptr_t)rsrc;
			ASSERT_EQUAL(expected_ret, real_ret);
		}
		return ret;

	case FUNC_STRRCHR:
		esrc[context->needle] = 0;
		rsrc[context->needle] = 0;
		BUG_ON(NULL == memchr(esrc + offset, esrc[context->needle], size + 1));

		switch (context->state) {
		case 0:
			real_p = tlu_strrchr(rsrc + offset, rsrc[context->needle]);
			expected_p = strrchr(esrc + offset, esrc[context->needle]);
			ret = NEXT_OFFSET_OR_STATE;
			break;
		case 1:
			real_p = tlu_strrchr(rsrc + offset, 0);
			expected_p = strrchr(esrc + offset, 0);
			ret = NEXT_OFFSET;
			break;
		default:
			BUG("utest::strrchr: invalid state");
		}

		if (expected_p == NULL) {
			ASSERT_NULL(real_p);
		} else {
			expected_ret = (intptr_t)expected_p - (intptr_t)esrc;
			real_ret = (intptr_t)real_p - (intptr_t)rsrc;
			ASSERT_EQUAL(expected_ret, real_ret);
		}
		return ret;

	case FUNC_SSTARTSWITH:
		memzero(esrc + size, 2);
		memzero(rsrc + size, 2);
		BUG_ON(NULL == memchr(esrc + offset, 0, size + 1));

		real_ret = 0;
		expected_ret = 0;

		switch (context->state) {
		case 0:
			real_ret = tlu_sstartswith(rsrc, esrc);
			expected_ret = sstartswith(rsrc, esrc);
			ret = NEXT_OFFSET_OR_STATE;
			break;
		case 1:
			esrc[context->needle] = '\0';
			real_ret = tlu_sstartswith(rsrc + offset, esrc);
			expected_ret = sstartswith(rsrc + offset, esrc);
			rsrc[context->needle] = '\0';
			ret = NEXT_OFFSET_OR_STATE;
			break;
		case 2:
			rsrc[context->needle] = '\0';
			real_ret = tlu_sstartswith(rsrc, esrc + offset);
			expected_ret = sstartswith(rsrc, esrc + offset);
			esrc[context->needle] = '\0';
			ret = NEXT_OFFSET_OR_STATE;
			break;
		case 3:
			context->expected_src[context->needle]++;
			real_ret = tlu_sstartswith(rsrc + offset, esrc + offset);
			expected_ret = sstartswith(rsrc + offset, esrc + offset);
			context->real_src[context->needle]++;
			ret = NEXT_OFFSET_OR_STATE;
			break;

		case 4:
			context->real_src[context->needle]++;
			real_ret = tlu_sstartswith(rsrc + offset, esrc + offset);
			expected_ret = sstartswith(rsrc + offset, esrc + offset);
			context->expected_src[context->needle]++;
			ret = NEXT_OFFSET;
			break;
		default:
			BUG("utest::sstartswith: invalid state");
		}
		ASSERT_EQUAL(expected_ret, real_ret);
		return ret;

	case FUNC_SENDSWITH:
		memzero(esrc + size, 2);
		memzero(rsrc + size, 2);
		BUG_ON(NULL == memchr(esrc + offset, 0, size + 1));

		real_ret = 0;
		expected_ret = 0;

		switch (context->state) {
		case 0:
			real_ret = tlu_sendswith(rsrc + offset, esrc + offset);
			expected_ret = sendswith(rsrc + offset, esrc + offset);
			ret = NEXT_OFFSET_OR_STATE;
			break;
		case 1:
			esrc[context->needle] = '\0';
			real_ret = tlu_sendswith(rsrc + offset, esrc + offset);
			expected_ret = sendswith(rsrc + offset, esrc + offset);
			rsrc[context->needle] = '\0';
			ret = NEXT_OFFSET_OR_STATE;
			break;
		case 2:
			rsrc[context->needle] = '\0';
			real_ret = tlu_sendswith(rsrc + offset, esrc + offset);
			expected_ret = sendswith(rsrc + offset, esrc + offset);
			esrc[context->needle] = '\0';
			ret = NEXT_OFFSET_OR_STATE;
			break;
		case 3:
			context->expected_src[context->needle]++;
			real_ret = tlu_sendswith(rsrc + offset, esrc + offset);
			expected_ret = sendswith(rsrc + offset, esrc + offset);
			context->real_src[context->needle]++;
			ret = NEXT_OFFSET_OR_STATE;
			break;

		case 4:
			context->real_src[context->needle]++;
			real_ret = tlu_sendswith(rsrc + offset, esrc + offset);
			expected_ret = sendswith(rsrc + offset, esrc + offset);
			context->expected_src[context->needle]++;
			ret = NEXT_OFFSET;
			break;
		default:
			BUG("utest::sendswith: invalid state");
		}

		if (expected_ret != real_ret) {
			printf("\n");
		}

		ASSERT_EQUAL(expected_ret, real_ret);
		return ret;

	default:
		BUG("utest::string_suite: unknown function");
	}
	BUG("utest:string_suite: should not be there");
}

void utest_string_suite(size_t max_size, size_t max_offset, struct mem_context *context, bool printable)
{
	int ret;

	context->state = 0;
	while (true) {
		ret = utest_mem_suite_run(max_size, max_offset, context,
					  printable, utest_string_callback);

		if (ret != NEXT_STATE) {
			break;
		}
		utest_ok();
		context->state++;
	}
}

