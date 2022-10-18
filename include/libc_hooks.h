#ifndef _JOSRT_LIBC_HOOKS_H
#define _JOSRT_LIBC_HOOKS_H

#include <stddef.h>

// write data to a file handle
extern int sys_write(int fd, const char* data, size_t len);

#endif // _JOSRT_LIBC_HOOKS_H