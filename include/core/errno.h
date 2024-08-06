
#ifndef CORE_ERRNO_H
#define CORE_ERRNO_H

#define PTR_ERR(ptr) ((void *)(ptr) < (void *)4096ul)

#define EOK 0 /* ok */
#define EINVAL 1 /* invalid arguments */
#define ENOMEM 2 /* no memory */
#define ENOENT 3 /* entry not found */
#define ENOMODIFY 4 /* object modification forbidden */

#endif /* CORE_ERRNO_H */

