
#ifndef _CONTAINER_CSTRING_H_
#define _CONTAINER_CSTRING_H_

#include <stdbool.h>

#include <core/compiler.h>

struct cstring {
	char *data;
	unsigned long size;
	unsigned long alloc;
};

#define CSTRING_INIT() (struct cstring){.data = NULL, .size=0, .alloc = 0};

#if CONFIG_CONTAINER_ALLOC_FAIL && !defined(__may_alloc)
# define __may_alloc __must_check
#else
# define __may_alloc /* empty */
#endif

#define __DEBATING __unused
#define __TODO __unused

/**
 * constructors and destructors
 */
__may_alloc int cstring_init(struct cstring *cstring, const char *init);
void cstring_destroy(struct cstring *cstring);

/**
 * string informaion
 */
const char *cstring_data(const struct cstring *cstring);
unsigned long cstring_size(const struct cstring *cstring);
unsigned long cstring_alloc(const struct cstring *cstring);
char cstring_at(const struct cstring *cstring, long pos);

/**
 * string moditication
 */
void cstring_set(struct cstring *cstring, long pos, char c);
__may_alloc int cstring_append(struct cstring *cstring, char c);
__may_alloc int cstring_appfront(struct cstring *cstring, char c) __TODO;
__may_alloc int cstring_extend(struct cstring *cstring, const char *other);
__may_alloc int cstring_extfront(struct cstring *cstring, const char *other) __TODO;
__may_alloc int cstring_remove_suffix(struct cstring *cstring, unsigned long size) __TODO;
__may_alloc int cstring_remove_prefix(struct cstring *cstring, unsigned long size) __TODO;

/**
 * string creation
 */
__may_alloc int cstring_copy(const struct cstring *src, struct cstring *dst) __DEBATING;
__may_alloc int cstring_subs(const struct cstring *src, const struct cstring *dst, unsigned long begin, unsigned long end) __DEBATING;

/**
 * string lookups
 */
bool cstring_contains(const struct cstring *cstring, const char *pattern);
bool cstring_starts_with(const struct cstring *cstring, const char *pattern);
bool cstring_ends_with(const struct cstring *cstring, const char *pattern);
unsigned long cstring_count(const struct cstring *cstring, const char *pattern);
long cstring_find(const struct cstring *cstring, const char *pattern);
unsigned long cstring_rfind(const struct cstring *cstring, const char *pattern);

/**
 * string character informaion
 */
bool cstring_isalpha(const struct cstring *cstring) __TODO;
bool cstring_isdigit(const struct cstring *cstring) __TODO;
bool cstring_isalnum(const struct cstring *cstring) __TODO;
bool cstring_islower(const struct cstring *cstring) __TODO;
bool cstring_isupper(const struct cstring *cstring) __TODO;
bool cstring_isspace(const struct cstring *cstring) __TODO;

/**
 * cstring formatting
 */
void cstring_tolwer(struct cstring *cstring) __TODO;
void cstring_toupper(struct cstring *cstring) __TODO;
__may_alloc int cstring_replace(struct cstring *cstring, const char *from, const char *to) __TODO;
__may_alloc void cstring_lstrip(struct cstring *cstring) __TODO;
__may_alloc void cstring_rstrip(struct cstring *cstring) __TODO;
__may_alloc void cstring_strip(struct cstring *cstring) __TODO;
void cstring_reverse(struct cstring *cstring) __TODO;

#endif /* _CONTAINER_CSTRING_H_ */

