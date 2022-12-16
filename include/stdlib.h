#ifndef _JOS_STDLIB_H
#define _JOS_STDLIB_H

#include <sys/cdefs.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#if defined(__clang__) || defined(__GCC__)
__attribute__((__noreturn__))  void abort(void);
#else
#error Unsupported platform
#endif

#ifdef NULL
#undef NULL
#define NULL (0)
#endif

void srand(unsigned seed);
int rand(void);

unsigned long long strtoull(const char *restrict s, char **restrict p, int base);
long long strtoll(const char *restrict s, char **restrict p, int base);
unsigned long strtoul(const char *restrict s, char **restrict p, int base);
long strtol(const char *restrict s, char **restrict p, int base);
intmax_t strtoimax(const char *restrict s, char **restrict p, int base);
uintmax_t strtoumax(const char *restrict s, char **restrict p, int base);

// We can only support this when we are more __STD_LIBC__ compliant
// extern int posix_memalign(void **__memptr, size_t __alignment, size_t __size);
// extern void* malloc(size_t);
// extern void free(void*);


int* _errno(void);
#define errno (*_errno())

#ifdef __cplusplus
}
#endif

#endif // _JOS_STDLIB_H
