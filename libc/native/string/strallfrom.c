
#include <libc/string.h>

char *tlu_strallfrom(const char *s, const char *keepset)
{
	while (*s) {
		if (tlu_strchr(keepset, *s) == NULL) {
			return (char *)s;
		}
		s++;
	}

	return NULL;
}

