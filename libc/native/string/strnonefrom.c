
#include <libc/string.h>

const char *tlu_strnonefrom(const char *s, const char *reject)
{
	while (*s) {
		if (tlu_strchr(reject, *s) != NULL) {
			return s;
		}
		s++;
	}

	return NULL;
}

