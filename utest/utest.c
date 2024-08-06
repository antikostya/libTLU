
#include <core/color.h>
#include <core/common.h>
#include <core/types.h>
#include <utest/internal.h>
#include <utest/utest.h>
#include <utest/utils.h>

#include <ctype.h>
#include <setjmp.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define UTEST_PRINT_VA(color, format)				\
	do {							\
		va_list __ap;					\
		va_start(__ap, format);				\
		__utest_print_color(color, format, __ap);	\
		va_end(__ap);					\
	} while (false)

static struct __utest utest_init __UTEST_ATTR = { .name = NULL,
						  .func = NULL,
						  .magic = __UTEST_MAGIC,
						  .skip = false };

static jmp_buf jump_buf;
static struct timespec time_start_monotonic;
static uint nr_tests = 0;
static uint nr_ok = 0;
static uint nr_skipped = 0;
static uint nr_failed = 0;

static void utest_fini(const char *color, bool abort);

extern __printf(2, 0)
int __utest_print_color(const char *color, const char *format, va_list ap);

void utest_ok(void)
{
	utest_print_green("[ OK ] ");
	fflush(stdout);
}

static void signal_handler(int sig)
{
	switch (sig) {
	case SIGSEGV:
		nr_failed++;
		utest_print_red("[SEGMENTATION FAULT]\n");
		break;
	case SIGBUS:
		nr_failed++;
		utest_print_red("[BUS ERROR]\n");
		break;
	default:
		nr_failed++;
		utest_print_red("[UNKNOWN SIGNAL]\n");
	}
	if (CONFIG_UTEST_FAIL_HALT) {
		while (true);
	}
	utest_fini(ANSI_RED, true);
}

__no_sanitize_address
static struct __utest *get_utest_begin(struct __utest *begin)
{
	uint64 magic = begin->magic;

	while (begin->magic == magic) {
		--begin;
	}

	return ++begin;
}

__no_sanitize_address
static struct __utest *get_utest_end(struct __utest *end)
{
	uint64 magic = end->magic;

	while (end->magic == magic) {
		++end;
	}

	return end;
}

__no_sanitize_address
static uint get_utest_longest_name(struct __utest *start, struct __utest *end)
{
	uint longest_name = 0;

	for (; start != end; start++) {
		if (start->name == NULL)
			continue;

		longest_name = max(longest_name, strlen(start->name));
		nr_tests++;
	}

	return longest_name;
}

static bool do_skip_utest(const char *name, const char **keep_list)
{
	if (keep_list == NULL) {
		return false;
	}
	while (*keep_list) {
		if (!strncmp(name, *keep_list, strlen(*keep_list))) {
			return false;
		}
		++keep_list;
	}
	return true;
}

static void utest_fini(const char *color, bool abort)
{
	struct timespec time_end_monotonic;
	struct timespec time_cpu;
	double diff;

	clock_gettime(CLOCK_MONOTONIC, &time_end_monotonic);
	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time_cpu);
	utest_print_color(color, "RESULTS: %u tests (%u ok, %u skipped, %u failed)\n", nr_tests, nr_ok, nr_skipped, nr_failed);

	diff = time_end_monotonic.tv_sec - time_start_monotonic.tv_sec +
		((double)time_end_monotonic.tv_nsec - time_start_monotonic.tv_nsec) * 1e-9;
	printf("user time: %.4fs, system time: %.4fs\n",
	       diff, time_cpu.tv_sec + time_cpu.tv_nsec * 1e-9);

	if (abort)
		exit(1);
}

static void suite_run(struct __utest *suite, const char *name, const char **keep_list)
{
	struct __utest *begin = get_utest_begin(suite);
	struct __utest *end = get_utest_end(suite);
	uint longest_name = get_utest_longest_name(begin, end);
	uint i = 1;

	if (SIG_ERR == signal(SIGSEGV, signal_handler) ||
	    SIG_ERR == signal(SIGBUS, signal_handler)) {
		utest_panic("failed to set signal handlers");
	}

	for (; begin != end; begin++, i++) {
		if (begin->name == NULL)
			continue;

		if (setjmp(jump_buf))
			continue;

		printf("%s %u/%u: %s:", name, i, nr_tests, begin->name);
		printf("%-*.s\t", (int)(longest_name - strlen(begin->name)), "");
		fflush(stdout);

		if (begin->skip || do_skip_utest(begin->name, keep_list)) {
			utest_print_yellow("[SKIPPED]\n");
			nr_skipped++;
		} else {
			begin->func();
			nr_ok++;
			utest_ok();
			printf("\n");
		}
	}

	if (SIG_ERR == signal(SIGSEGV, SIG_DFL) ||
	    SIG_ERR == signal(SIGBUS, SIG_DFL)) {
		utest_panic("failed restore signal handlers");
	}

	if (nr_failed > 0)
		utest_fini(ANSI_RED, false);
	else
		utest_fini(ANSI_GREEN, false);
}

void unittest(const char **argv)
{
	if (argv == NULL || argv[1] == NULL) {
		argv = NULL;
	} else {
		argv = &argv[1];
	}

	clock_gettime(CLOCK_MONOTONIC, &time_start_monotonic);
	suite_run(&utest_init, "TEST", argv);
}

__cold __noret
static void assert_failed(void)
{
	if (CONFIG_UTEST_FAIL_HALT) {
		while (true);
	} if (CONFIG_UTEST_FIRST_FAIL) {
		utest_fini(ANSI_RED, true);
	} else {
		longjmp(jump_buf, 1);
	}
	unreachable();
}

static void announce_fail(const char *file, uint line)
{
	nr_failed++;
	utest_print_red("[FAIL]\n");
	utest_print_red("[ASSERT FAILED]");
	utest_print_white(": %s:%u\n", file, line);
	fflush(stdout);
}

static void print_bool(bool v)
{
	if (v == true) {
		utest_print_blue("true");
	} else {
		utest_print_blue("false");
	}
}

static void print_int(int64 v)
{
	if (v == 0) {
		utest_print_blue("zero");
	} else {
		utest_print_blue("%ld", v);
	}
}

static void print_ptr(const void *v)
{
	if (v == NULL) {
		utest_print_blue("NULL");
	} else {
		utest_print_blue("%p", v);
	}
}

static void print_mem(const void *v, uint64 size)
{
	const uint8 *s = v;

	if (s == NULL) {
		utest_print_blue("(nil)");
	} else {
		utest_print_blue("\"");
		for (uint64 i = 0; i < size; i++) {
			if (isalnum(s[i])) {
				utest_print_blue("%c", s[i]);
			} else {
				utest_print_blue(".");
			}
		}
		utest_print_blue("\"");
	}
}

__used
static void print_str(const char *v)
{
	if (v == NULL) {
		utest_print_blue("(nil)");
	} else {
		print_mem(v, strlen(v));
	}
}

static void print_sign(int64 v)
{
	if (v > 0) {
		utest_print_blue("positive value");
	} else if (v < 0) {
		utest_print_blue("negative value");
	} else {
		utest_print_blue("zero");
	}
}

void __assert_fail_impl(const char *file, uint line, const char *format, ...)
{
	announce_fail(file, line);
	if (format != NULL) {
		utest_print_yellow("reason: ");
		UTEST_PRINT_VA(ANSI_YELLOW, format);
		printf("\n");
	} else {
		utest_print_yellow("should not be there\n");
	}

	assert_failed();
}

void __assert_bool_impl(bool exp, bool real, const char *file, uint line, const char *format, ...)
{
	if (unlikely(exp != real)) {
		announce_fail(file, line);
		if (format != NULL) {
			UTEST_PRINT_VA(ANSI_YELLOW, format);
			utest_print_yellow(": ");
		}

		utest_print_yellow("expected ");
		print_bool(exp);
		utest_print_yellow(", got ");
		print_bool(real);
		printf("\n");
	} else {
		return;
	}

	assert_failed();
}

void __assert_eq_impl(int64 exp, int64 real, bool eq, const char *file, uint line, const char *format, ...)
{
	if (unlikely(eq && exp != real)) {
		announce_fail(file, line);
		if (format != NULL) {
			UTEST_PRINT_VA(ANSI_YELLOW, format);
			utest_print_yellow(": ");
		}

		utest_print_yellow("expected ");
		print_int(exp);
		utest_print_yellow(", got ");
		print_int(real);
		printf("\n");
	} else if (unlikely(!eq && exp == real)) {
		announce_fail(file, line);
		if (format != NULL) {
			UTEST_PRINT_VA(ANSI_YELLOW, format);
			utest_print_yellow(": ");
		}

		utest_print_yellow("got ");
		print_int(real);
		utest_print_yellow(", but didnt expect\n");
	} else {
		return;
	}

	assert_failed();
}

void __assert_gt_impl(int64 border, int64 val, bool greater, bool equal, const char *file, uint line, const char *format, ...)
{
	const char *sign;
	bool ok;

	switch (greater << 1 | equal) {
	case 0b00:
		/* less than */
		sign = "<";
		ok = val < border;
		break;
	case 0b01:
		/* less or equal */
		sign = "<=";
		ok = val <= border;
		break;
	case 0b10:
		/* greater than */
		sign = ">";
		ok = val > border;
		break;
	case 0b11:
		/* greater or equal */
		sign = ">=";
		ok = val >= border;
		break;
	}

	if (unlikely(!ok)) {
		announce_fail(file, line);
		if (format != NULL) {
			UTEST_PRINT_VA(ANSI_YELLOW, format);
			utest_print_yellow(": ");
		}

		utest_print_yellow("expected ");
		print_int(val);
		utest_print_blue(" %s ", sign);
		print_int(border);
		utest_print_yellow("\n");
		assert_failed();
	}
}

void __assert_ptr_impl(const void *exp, const void *real, bool eq, const char *file, uint line, const char *format, ...)
{
	if (unlikely(eq && exp != real)) {
		announce_fail(file, line);
		if (format != NULL) {
			UTEST_PRINT_VA(ANSI_YELLOW, format);
			utest_print_yellow(": ");
		}

		utest_print_yellow("expected ");
		print_ptr(exp);
		utest_print_yellow(", got ");
		print_ptr(real);
		printf("\n");
	} else if (unlikely(!eq && exp == real)) {
		announce_fail(file, line);
		if (format != NULL) {
			UTEST_PRINT_VA(ANSI_YELLOW, format);
			utest_print_yellow(": ");
		}

		utest_print_yellow("got ");
		print_ptr(real);
		utest_print_yellow(", but didnt expect\n");
	} else {
		return;
	}

	assert_failed();
}

static void __utest_print_mem(const char *exp, const char *real, uint64 size_exp, uint64 size_real)
{
	utest_print_yellow("expected: ");
	print_mem(exp, size_exp);
	utest_print_yellow("\ngot:      ");

	utest_print_blue("\"");
	for (uint64 i = 0; i < size_real; ++i) {
		if (i >= size_exp || exp[i] != real[i]) {
			if (isalnum(real[i])) {
				utest_print_red("%c", real[i]);
			} else {
				utest_print_red(".");
			}
		} else {
			utest_print_blue("%c", real[i]);
		}
	}
	utest_print_blue("\"");
	printf("\n");
}

void utest_print_mem(const void *exp, const void *real, uint64 size)
{
	__utest_print_mem(exp, real, size, size);
}

void utest_print_str(const char *exp, const char *real)
{
	__utest_print_mem(exp, real, strlen(exp), strlen(real));
}

void __assert_mem_impl(const char *exp, const char *real, uint64 size, bool eq, const char *file, uint line, const char *format, ...)
{
	bool mem_eq;
	uint64 size_exp = size;
	uint64 size_real = size;

	if (size == (uint64)-1) {
		size_exp = strlen(exp);
		size_real = strlen(real);
	}

	if (size_exp == size_real)
		mem_eq = !memcmp(exp, real, size_exp);
	else
		mem_eq = false;

	if (unlikely(eq && !mem_eq)) {
		announce_fail(file, line);
		if (format != NULL) {
			UTEST_PRINT_VA(ANSI_YELLOW, format);
			printf("\n");
		}

		__utest_print_mem(exp, real, size_exp, size_real);
	} else if (unlikely(!eq && mem_eq)) {
		announce_fail(file, line);
		if (format != NULL) {
			UTEST_PRINT_VA(ANSI_YELLOW, format);
			utest_print_yellow(": ");
		}

		utest_print_yellow("got ");
		print_mem(real, size_real);
		utest_print_yellow(", but didnt expect\n");
	} else {
		return;
	}

	assert_failed();
}

void __assert_sign_impl(int64 exp, int64 real, bool eq, const char *file, uint line, const char *format, ...)
{
	if (unlikely(eq && sign(exp) != sign(real))) {
		announce_fail(file, line);
		if (format != NULL) {
			UTEST_PRINT_VA(ANSI_YELLOW, format);
			utest_print_yellow(": ");
		}

		utest_print_yellow("expected ");
		print_sign(exp);
		utest_print_yellow(", got ");
		print_sign(real);
		printf("\n");
	} else if (unlikely(!eq && sign(exp) == sign(real))) {
		announce_fail(file, line);
		if (format != NULL) {
			UTEST_PRINT_VA(ANSI_YELLOW, format);
			utest_print_yellow(": ");
		}

		utest_print_yellow("got ");
		print_sign(real);
		utest_print_yellow(", but expected ");
		print_sign(exp);
		printf("\n");
	} else {
		return;
	}

	assert_failed();
}

__cold __noret __printf(1, 5) void
__utest_panic(const char *format, const char *file, const char *func, uint line, ...)
{
	va_list args;

	utest_print_red("[PANIC]\n");
	utest_print_white("%s:%u: %s: ", file, line, func);

	va_start(args, line);
	__utest_print_color(ANSI_WHITE, format, args);
	printf("\n");
	fflush(stdout);

	nr_failed++;
	utest_fini(ANSI_RED, true);
	unreachable();
}

