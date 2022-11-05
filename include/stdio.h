#ifndef _JOS_STDIO_H
#define _JOS_STDIO_H

#include <sys/cdefs.h>
#include <stddef.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>
//TODO: genericise...
#ifdef JOBASE_WCHAR_SUPPORT
    #include <wchar.h>
#endif
#include <josrt.h>

#ifdef JOBASE_WCHAR_SUPPORT
extern int swprintf(wchar_t* RESTRICT buffer, size_t sizeOfBuffer, const wchar_t* RESTRICT format, ...);
extern int vswprintf(wchar_t *RESTRICT buffer, size_t bufsz, const wchar_t * RESTRICT format, va_list vlist);
#endif

extern int snprintf(char* RESTRICT buffer, size_t sizeOfBuffer, const char* RESTRICT format, ...);
extern int vsnprintf(char *RESTRICT buffer, size_t bufsz, const char * RESTRICT format, va_list parameters);
extern int printf(const char* RESTRICT format, ...);

typedef struct __FILE_TAG FILE;
extern FILE *const stdin;
extern FILE *const stdout;
extern FILE *const stderr;

#define stdin  (stdin)
#define stdout (stdout)
#define stderr (stderr)

#ifdef _JOSRT_REQUIRES_FILE

extern FILE *fopen(const char *RESTRICT, const char *RESTRICT);
extern int fclose(FILE *);

extern int feof(FILE *);
extern int ferror(FILE *);
extern int fflush(FILE *);
extern void clearerr(FILE *);

extern int fseek(FILE *, long, int);
extern long ftell(FILE *);

extern size_t fread(void *RESTRICT, size_t, size_t, FILE *RESTRICT);
extern size_t fwrite(const void *RESTRICT, size_t, size_t, FILE *RESTRICT);
#endif

#endif // _JOS_STDIO_H
