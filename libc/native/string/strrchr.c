
#include <libc/string.h>

const char *tlu_strrchr(const char *s, uint8 c)
{
	const char *ans = NULL;

	while (*s) {
		if (*s == c)
			ans = s;
		s++;
	}

	if (c == '\0')
		return s;
	return ans;
}

