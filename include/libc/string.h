
#ifndef LIBC_STRING_H
#define LIBC_STRING_H

#include <core/types.h>

uint64 tlu_strcnt(const char *s, uint8 c);
bool tlu_streq(const char *a, const char *b);
char *tlu_strchr(const char *s, uint8 c);
char *tlu_strrchr(const char *s, uint8 c);
uint64 tlu_strcmp(const char *a, const char *b);
uint64 tlu_strlen(const char *s);
uint64 tlu_strncmp(const char *a, const char *b, uint64 size);

#endif /* LIBC_STRING_H */

