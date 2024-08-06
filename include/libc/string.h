
#ifndef LIBC_STRING_H
#define LIBC_STRING_H

#include <core/types.h>

bool tlu_strends(const char *str, const char *pattern);
bool tlu_streq(const char *a, const char *b);
bool tlu_strstarts(const char *str, const char *pattern);
char *tlu_strnonefrom(const char *s, const char *breakset);
char *tlu_strchr(const char *s, uint8 c);
char *tlu_strrchr(const char *s, uint8 c);
char *tlu_strallfrom(const char *s, const char *keepset);
const char *tlu_strstr(const char *a, const char *b);
uint64 tlu_strcmp(const char *a, const char *b);
uint64 tlu_strcnt(const char *s, uint8 c);
uint64 tlu_strlen(const char *s);
uint64 tlu_strncmp(const char *a, const char *b, uint64 size);
void tlu_strcpy(char *dst, const char *src);

#endif /* LIBC_STRING_H */

