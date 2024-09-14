
#include <container/cstring.h>
#include <utest/utest.h>
#include <utest/utils.h>

#include "../cstring_internal.h"
#include <setjmp.h>
#include <stdio.h>
#include <string.h>


int main(int argc, const char **argv)
{
	(void)argc;
	cstring_init();
	unittest(argv);
	cstring_fini();
	return 0;
}
