#ifndef _JOSRT_STDIO_H
#define _JOSRT_STDIO_H

#include <sys/cdefs.h>
#include <stddef.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>

#ifdef _JOSRT_WCHAR_SUPPORT
    #include <wchar.h>
#endif
#include <josrt.h>

#ifdef _JOSRT_WCHAR_SUPPORT
_JOSRT_API_FUNC int swprintf(wchar_t* RESTRICT buffer, size_t sizeOfBuffer, const wchar_t* RESTRICT format, ...);
_JOSRT_API_FUNC int vswprintf(wchar_t *RESTRICT buffer, size_t bufsz, const wchar_t * RESTRICT format, va_list vlist);
#endif

_JOSRT_API_FUNC int snprintf(char* RESTRICT buffer, size_t sizeOfBuffer, const char* RESTRICT format, ...);
_JOSRT_API_FUNC int vsnprintf(char *RESTRICT buffer, size_t bufsz, const char * RESTRICT format, va_list parameters);

#ifndef EOF
#define EOF     (int)(-1)
#endif

// always included to ensure FILE always exists since it's used for more than just plain old file IO
#include <_file.h>

#ifdef _JOSRT_REQUIRES_IO
_JOSRT_API_FUNC int printf(const char* RESTRICT format, ...);
typedef struct __FILE_TAG FILE;
_JOSRT_API_FUNC FILE *const stdin;
_JOSRT_API_FUNC FILE *const stdout;
_JOSRT_API_FUNC FILE *const stderr;

#define stdin  (stdin)
#define stdout (stdout)
#define stderr (stderr)

#endif // _JOSRT_REQUIRES_IO

#endif // _JOSRT_STDIO_H
