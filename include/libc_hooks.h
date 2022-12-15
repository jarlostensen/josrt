#ifndef _JOSRT_LIBC_HOOKS_H
#define _JOSRT_LIBC_HOOKS_H

#include <stddef.h>
#include <josrt.h>

// write data to a file handle
_JOSRT_API_FUNC int sys_write(int fd, const char* data, size_t len);
// open a file and return handle
_JOSRT_API_FUNC int sys_open(const char* path, int flags);
// read len bytes from a file or EOF (or error)
_JOSRT_API_FUNC int sys_read(int fd, char* dest, size_t len);
// close a file
_JOSRT_API_FUNC int sys_close(int fd);

// low level exit to host
extern void __exit(int);

#endif // _JOSRT_LIBC_HOOKS_H