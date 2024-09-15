
#ifndef CONTAINER_CSTRING_H
#define CONTAINER_CSTRING_H

#include <core/compiler.h>
#include <core/types.h>

// ----------------------------------------------------------------------------
// macro

enum cstring_errorcodes {
	CSTRING_PANIC_INVALID_POINTER,
	CSTRING_PANIC_OUT_OF_RANGE,
};

// ----------------------------------------------------------------------------
// external
extern void *cstring_allocate(uint64 size);
extern void cstring_deallocate(void *ptr);
extern __noret void cstring_panic(enum cstring_errorcodes errorcode);

// ----------------------------------------------------------------------------
// initialization
void cstring_init(void);
void cstring_fini(void);

// ----------------------------------------------------------------------------
// construction
const char *cstring_create(const char *orig);
void cstring_destroy(const char *string);

const char *cstring_copy(const char *other);

// ----------------------------------------------------------------------------
// element access
char cstring_at(const char *string, uint64 idx);
#define cstring_rat(string, idx) cstring_at(string, cstring_size(string) - idx - 1)

#define cstring_front(string) cstring_at(0)
#define cstring_back(string) cstring_rat(0)

// ----------------------------------------------------------------------------
// capacity
uint64 cstring_size(const char *string);
#define cstring_emtpy(string) (cstring_size(string) == 0)

// ----------------------------------------------------------------------------
// iterators
#define cstring_begin(string) (string)
#define cstring_end(string) ((string) + cstring_size(string))

#define cstring_rbegin(string) ((string) + cstring_size(string) - 1)
#define cstring_rend(string) ((string) - 1)

// ----------------------------------------------------------------------------
// traverse
#define cstring_for_each(string, iter) __cstring_for_each(string, iter)

// ============================================================================
// internal macros
#define __cstring_for_each(string, iter)		\
	for ((iter) = cstring_begin(string), 		\
	     assert_same_ptr_type(string, iter);	\
	     (iter) != cvector_end(vector);		\
	     (iter)++)


#if 0

// EXTENDED INTERFACE
const char *cstring_create_from(const char *begin, const char *end);

#define cstring_for_each_rev(string)

const char *cstring_find(const char *string, const char *other);
const char *cstring_rfind(const char *string, const char *other);
bool cstring_contains(const char *string, const char *other);
uint64 cstring_count(const char *string, const char *other);
const char *cstring_findc(const char *string, char c);
const char *cstring_rfindc(const char *string, char c);
bool cstring_containsc(const char *string, char c);
uint64 cstring_countc(const char *string, char c);

int cstring_compare(const char *s1, const char *s2);
bool cstring_equal(const char *s1, const char *s2);

bool cstring_startswith(const char *string, const char *prefix);
bool cstring_endswith(const char *string, const char *suffix);

bool cstring_isalpha(const char *string);
bool cstring_isdigit(const char *string);
bool cstring_islower(const char *string);
bool cstring_isspace(const char *string);
bool cstring_isupper(const char *string);

const char *cstring_tolower(const char *string);
const char *cstring_toupper(const char *string);

const char *cstring_lstrinp(const char *string);
const char *cstring_rstrip(const char *string);
const char *cstring_strip(const char *string);
const char *cstring_remove_prefix(const char *string, const char *prefix);
const char *cstring_remove_suffix(const char *string, const char *suffix);
const char *cstring_replace(const char *string, const char *from, const char *to);

const char *cstring_format(const char *format, ...);


// FUTURE DRAFT

const char *cstring_insert(const char *string, const char *pos, const char *s);
const char *cstring_insertc(const char *string, const char *pos, char c);

const char *cstring_insert_from(const char *string, const char *pos, const char *start, const char *end);
const char *cstring_remove_from(const char *string, const char *start, const char *end);
#endif

#endif /* CONTAINER_CSTRING_H */


