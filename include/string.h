#ifndef _STRING_H
#define _STRING_H 1

#include <sys/cdefs.h>

#include <stddef.h>

int memcmp(const void*, const void*, size_t);
void* memcpy(void* __restrict, const void* __restrict, size_t);
void* memmove(void*, const void*, size_t);
void* memset(void*, int, size_t);

// unsafe because it requires that s_ terminates within 16 readable bytes (or this will segfault)
size_t josrt_strlen_unsafe(const char* s_);

// safe strlen (but slower)
size_t strlen(const char*);

int strcmp(const char *l, const char *r);

#ifndef EOF
#define EOF     (int)(-1)
#endif

#endif
