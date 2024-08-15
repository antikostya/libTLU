
# libc implementation

# memory functions

| table of contents   |
|:------------------- |
| tlu_memccpy         |
| tlu_memchr          |
| tlu_memcmp          |
| tlu_memcnt          |
| tlu_memcpy          |
| tlu_memeq           |
| tlu_memmem (TODO)   |
| tlu_memmove         |
| tlu_memnchr         |
| tlu_memnrchr        |
| tlu_memset          |
| tlu_memset_secure   |
| tlu_memzero         |
| tlu_memzero_securee |


## tlu_memccpy
> ### NAME
> `tlu_memccpy` - copy memory area
> ### SYNOPSIS
> ```c
> #include <libc/mem.h>
>
> void *tlu_memccpy(void *dst, const void *src, uint8 c, uint64 size);
> ```
> ### DESCRIPTION
> The `tlu_memccpy()` function copies no more than `size` bytes from memory area `src` to memory area `dst`, stopping when the character `c` is found.
>
> If the memory areas overlap, the results are undefined.
> ### RETURN VALUE
> The `tlu_memccpy()` function returns a pointer to the next character in `dst` after `c`, or **NULL** if `c` was not found in the first `size` characters of `src`.


## tlu_memchr
> ### NAME
> `tlu_memchr` - scan memory for a character
> ### SYNOPSIS
> ```c
> #include <libc/mem.h>
>
> void *tlu_memchr(const void *p, uint8 c);
> ```
> ### DESCRIPTION
> The `tlu_memchr()` function scans the initial bytes of the memory area pointed to by `p` for the first instance of `c`. It assumes (i.e., the programmer knows for certain) that an instance of `c` lies somewhere in the memory area starting at the location pointed to by `p`, and so performs an optimized search for `c` (i.e., no use of a count argument to limit the range of the search). If an instance of c is not found, the results are unpredictable.
>
> Both `c` and the bytes of the memory area pointed to by `p` are interpreted as ***uint8***.
> ### RETURN VALUE
> The `tlu_memchr()` function returns a pointer to the matching byte, if one is found. If no matching byte is found, the result is unspecified.


## tlu_memcmp
> ### NAME
> `tlu_memcmp` - compare memory areas
> ### SYNOPSIS
> ```c
> #include <libc/mem.h>
>
> int tlu_memcmp(const void *a, const void *b, uint64 size);
> ```
> ### DESCRIPTION
> The `tlu_memcmp()` function compares the first `size` bytes (each interpreted as ***uint8***) of the memory areas `a` and `b`.
> ### RETURN VALUE
> The `tlu_memcmp()` function returns an integer:
>  - less than zero if `a` < `b`
>  - zero if `a` == `b`
>  - greater than zero if `a` > `b`
>
> In other words the `tlu_memcmp()` function returns an integer less than, equal to, or greater than **zero** if the first `size` bytes of `a` is found, respectively, to be less than, to match, or be greater than the first `size` bytes of `b`.
>
> For a nonzero return value, the sign is determined by the sign of the difference between the first pair of bytes (interpreted as ***uint8***) that differ in `a` and `b`.
>
> If `size` is zero, the return value is zero.


## tlu_memcnt
> ### NAME
> `tlu_memcnt` - count memory character
> ### SYNOPSIS
> ```c
> #include <libc/mem.h>
>
> uint64 tlu_memcnt(const void *p, uint8 c, uint64 size);
> ```
> ### DESCRIPTION
> The `tlu_memcnt()` function scans the initial `size` bytes of the memory area pointed to by `p` and count number instances of `c`.
>
> Both `c` and the bytes of the memory area pointed to by `s` are interpreted as ***uint8***.
> ### RETURN VALUE
> The `tlu_memchr()` function return number of instances of `c` in the given memory area.

## tlu_memcpy
> ### NAME
> `tlu_memcpy` - copy memory area
> ### SYNOPSIS
> ```c
> #include <libc/mem.h>
>
> void tlu_memcpy(void *dst, const void *src, uint64 size);
> ```
> ### DESCRIPTION
> The `tlu_memcpy()` function copies `size` bytes from memory area `src` to memory area `dst`. The memory areas must not overlap. Use **tlu_memmove()** if the memory areas do overlap.
>
> If the memory areas overlap, the results are undefined.
> ### RETURN VALUE
> none


## tlu_memeq
> ### NAME
> `tlu_memeq` - compare memory areas
> ### SYNOPSIS
> ```c
> #include <libc/mem.h>
>
> bool tlu_memeq(const void *a, const void *b, uint64 size);
> ```
> ### DESCRIPTION
> The `tlu_memeq()` function compares the first `size` bytes (each interpreted as ***uint8***) of the memory areas `a` and `b`.
> ### RETURN VALUE
> The `tlu_memcmp()` returns **true** if the first `size` bytes of `a` are found to match the first `size` bytes of `b`, else return **false**.
>
> If `size` is zero, the return value is zero.


## tlu_memmove
> ### NAME
> `tlu_memmove` - copy memory area
> ### SYNOPSIS
> ```c
> #include <libc/mem.h>
>
> void tlu_memmove(void *dst, const void *src, uint64 size);
> ```
> ### DESCRIPTION
> The `tlu_memmove()` function copies `n` bytes from memory area `src` to memory area `dst`. The memory areas may overlap: copying takes place as though the bytes in `src` are first copied into a temporary array that does not overlap `src` or `dest`, and the bytes are then copied from the temporary array to `dst`.
> ### RETURN VALUE
> none


## tlu_memnchr
> ### NAME
> `tlu_memnchr` - scan memory for a character
> ### SYNOPSIS
> ```c
> #include <libc/mem.h>
>
> void *tlu_memnchr(const void *p, uint8 c, uint64 size);
> ```
> ### DESCRIPTION
> The `tlu_memchr()` function scans the initial `size` bytes of the memory area pointed to by `p` for the first instance of `c`.
>
> Both `c` and the bytes of the memory area pointed to by `p` are interpreted as ***uint8***.
> ### RETURN VALUE
> The `tlu_memchr()` function return a pointer to the matching byte or **NULL** if the character does not occur in the given memory area.


## tlu_memnrchr
> ### NAME
> `tlu_memnrchr` - scan memory for a character
> ### SYNOPSIS
> ```c
> #include <libc/mem.h>
>
> void *tlu_memnrchr(const void *p, uint8 c, uint64 size);
> ```
> ### DESCRIPTION
> The `tlu_memnrchr()` function scans the initial `size` bytes of the memory area pointed to by `p` for the last instance of `c`.
>
> In other words `tlu_memnrchr()` function is like the `tlu_memnchr()` function, except that it searches backward from the end of the `size` bytes pointed to by `p` instead of forward from the beginning.
>
> Both `c` and the bytes of the memory area pointed to by `p` are interpreted as ***uint8***.
> ### RETURN VALUE
> The `tlu_memchr()` function return a pointer to the matching byte or **NULL** if the character does not occur in the given memory area.


## tlu_memset
> ### NAME
> `tlu_memset` - fill memory with a constant byte
> ### SYNOPSIS
> ```c
> #include <libc/mem.h>
>
> void tlu_memset(void *ptr, uint8 c, uint64 size);
> ```
> ### DESCRIPTION
> The `tlu_memset()` function fills the first `size` bytes of the memory area pointed to by `ptr` with the constant byte `c`.
> ### RETURN VALUE
> none


## tlu_memset_secure
> ### NAME
> `tlu_memset_secure` - fill memory with a constant byte
> ### SYNOPSIS
> ```c
> #include <libc/mem.h>
>
> void tlu_memset_secure(void *ptr, uint8 c, uint64 size);
> ```
> ### DESCRIPTION
> The `tlu_memset_secure()` function fills the first `size` bytes of the memory area pointed to by `ptr` with the constant byte `c`.
>
> The `tlu_memset_secure()` function performs the same task as `tlu_memset()`. It differs from `tlu_memset()` in that it guarantees that compiler optimizations will not remove the erase operation if the compiler deduces that the operation is *"unnecessary"*.
> ### RETURN VALUE
> none


## tlu_memzero
> ### NAME
> `tlu_memzero` - zero memory
> ### SYNOPSIS
> ```c
> #include <libc/mem.h>
>
> void tlu_memzero(void *ptr, uint64 size);
> ```
> ### DESCRIPTION
> The `tlu_memzero()` function fills the first `size` bytes of the memory area pointed to by `ptr` with zero bytes.
> ### RETURN VALUE
> none


## tlu_memzero_secure
> ### NAME
> `tlu_memzero_secure` - zero memory
> ### SYNOPSIS
> ```c
> #include <libc/mem.h>
>
> void tlu_memzero_secure(void *ptr, uint64 size);
> ```
> ### DESCRIPTION
> The `tlu_memzero_secure()` function fills the first `size` bytes of the memory area pointed to by `ptr` with zero bytes. It differs from `tlu_memzero()` in that it guarantees that compiler optimizations will not remove the erase operation if the compiler deduces that the operation is *"unnecessary"*.
> ### RETURN VALUE
> none




# string functions

| table of contents      |
|:---------------------- |
| tlu_strallfrom        +|
| tlu_strcasecmp (TODO)  |
| tlu_strcasestr (TODO)  |
| tlu_strchr            +|
| tlu_strcmp            +|
| tlu_strcnt             |
| tlu_strcpy             |
| tlu_strends            |
| tlu_streq              |
| tlu_strlen             |
| tlu_strncasecmp (TODO) |
| tlu_strncmp            |
| tlu_strnonefrom        |
| tlu_strrchr            |
| tlu_strstarts          |
| tlu_strstr             |


## tlu_strallfrom
> ### NAME
> `tlu_strallfrom` - search a string for any of disallowed characters
> ### SYNOPSIS
> ```c
> #include <libc/string.h>
>
> const char *tlu_strallfrom(const char *s, const char *accept);
> ```
> ### DESCRIPTION
> The `tlu_strallfrom()` function locates the first occurrence in the string `s` of any of the bytes not in the string `accept`. In other words it locates the next character after segment of `s` which consists entirely of bytes in `accept`.
> ### RETURN VALUE
> The `tlu_strallfrom()` function returns a pointer to the byte in `s` that not matches one of the bytes in `accept`, or **NULL** if no such byte is found.


## tlu_strchr
> ### NAME
> `tlu_strchr` - locate character in string
> ### SYNOPSIS
> ```c
> #include <libc/string.h>
>
> const char *tlu_strchr(const char *s, uint8 c);
> ```
> ### DESCRIPTION
> The `tlu_strchr()` function returns a pointer to the first occurrence of the character `c` in the string `s`.
> ### RETURN VALUE
> The `tlu_strchr()` functions return a pointer to the matched character or **NULL** if the character is not found. The terminating null byte is considered part of the string, so that if `c` is specified as **\0**, these functions return a pointer to the terminator.


## tlu_strcmp
> ### NAME
> `tlu_strcmp` - compare two strings
> ### SYNOPSIS
> ```c
> #include <libc/string.h>
>
> const char *tlu_strcmp(const char *s1, const char *s2);
> ```
> ### DESCRIPTION
> The `tlu_strcmp()` function compares the two strings `s1` and `s2`. The locale is not taken into account. The comparison is done using unsigned characters.
>
> strcmp() returns an integer indicating the result of the comparison, as follows:
> - **zero**, if the `s1` and `s2` are equal
> - a **negative value** if `s1` is less than `s2`
> - a **positive value** if `s1` is greater than `s2`
> ### RETURN VALUE
> The `tlu_strcmp()` function return an integer less than, equal to, or greater than **zero** if `s1` is found, respectively, to be less than, to match, or be greater than `s2`.

# character classifiction functions

| table of contents  |
|:------------------ |
| isalnum            |
| isalpha            |
| isdigit            |
| isdigit_base (TOD) |
| ishex              |
| islower            |
| isoct (TODO)       |
| isprint            |
| ispunct            |
| isspace            |
| isupper            |
| tolower            |
| toupper            |

# lexixal functions (TODO)

```c
int tlu_int2str(int64 n, char *s, uint base);
errors = {
	EINVAL // invalid base
}
int tlu_uint2str(uint64 n, char *s, uint base);
errors = // same for tlu_int2str
int64 tlu_str2int(const char *ptr, uint base, const char **end, int *err);
errors = {
	EINVAL // invalid base or no number in @ptr
	EOVERFLOW // number overflow
	EUNDERFLOW // number underflow
}
uint64 tlu_str2uint(const char *ptr, uint base const char **end, int *err);
errors = // same as tlu_str2int, but without EUNDERFLOW

void tlu_itoa(int64 n, char *dst);
// alias for tlu_int2str(n, dst, 10)
int64 tlu_atoi(char *s);
// alias for tlu_str2int(s, 10, NULL, NULL)
```
