
#include <libc/string.h>

const char *tlu_strallfrom(const char *s, const char *accept)
{
	while (*s) {
		if (tlu_strchr(accept, *s) == NULL) {
			return s;
		}
		s++;
	}

	return NULL;
}

