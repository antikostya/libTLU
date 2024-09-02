
#ifndef CORE_ERRNO_H
#define CORE_ERRNO_H

#define PTR_ERR(ptr) ((void *)(ptr) < (void *)4096ul)

#define EOK 0 /* ok */
#define EINVAL 1 /* invalid arguments */
#define ENOMEM 2 /* no memory */
#define ENOSPACE 4 /* no space left */

#endif /* CORE_ERRNO_H */

