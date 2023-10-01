
#ifndef _UTEST_UTILS_H_
#define _UTEST_UTILS_H_

#include <stdint.h>

void utest_random_init(uint32_t seed);
uint64_t utest_random(void);
uint64_t utest_random_range(uint64_t from, uint64_t to);

void utest_progress_start(void);
void utest_progress(unsigned long current, unsigned long total);
void utest_progress_done(void);

#endif /* _UTEST_UTILS_H_ */

