
#ifndef CORE_COMPILER_H
#define CORE_COMPILER_H

#ifndef __always_inline
# define __always_inline inline __attribute__((__always_inline__))
#endif

#ifndef noinline
# define noinline __attribute__((__noinline__))
#endif

#ifndef __must_check
# define __must_check __attribute__((__warn_unused_result__))
#endif

#ifndef __used
# define __used __attribute__((__used__))
#endif

#ifndef __unused
# define __unused __attribute__((__unused__))
#endif

#ifndef __packed
# define __packed __attribute__((__packed__))
#endif

#ifndef __noret
# define __noret __attribute__((__noreturn__))
#endif

#ifndef __cold
# define __cold __attribute__((__cold__))
#endif

#ifndef __aligned
# define __aligned(__v) __attribute__((__aligned__(__v)))
#endif

#ifndef __section
# define __section(__sec) __attribute__((__section__(__sec)))
#endif

#ifndef __printf
# define __printf(a, b) __attribute__((__format__(printf, a, b)))
#endif

#ifndef __scanf
# define __scanf(a, b) __attribute__((__format__(scanf, a, b)))
#endif

#ifndef __weak
# define __weak __attribute__((__weak__))
#endif

#ifndef likely
# define likely(expr) __builtin_expect(!!(expr), 1)
#endif

#ifndef unlikely
# define unlikely(expr) __builtin_expect(!!(expr), 0)
#endif

#ifndef unreachable
# define unreachable() __builtin_unreachable()
#endif

#ifndef fallthrough
# define fallthrough __attribute__((__fallthrough__))
#endif

#ifndef typeof
# define typeof(expr) __typeof(expr)
#endif

#ifndef typeof_unqual
# define typeof_unqual(expr) typeof(expr)
#endif

#ifndef offsetof
# define offsetof(type, member) __builtin_offsetof(type, member)
#endif

#ifndef __no_sanitize_address
# define __no_sanitize_address __attribute__((__no_sanitize_address__))
#endif

#ifndef barrier
# define barrier() __asm__ __volatile__("": : :"memory")
#endif

#endif /* CORE_COMPILER_H */

