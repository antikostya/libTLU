
#include <libc/string.h>

const char *tlu_strchr(const char *s, uint8 c)
{
	while (*s) {
		if (*s == c)
			return s;
		s++;
	}

	if (c == '\0')
		return s;
	return NULL;
}

