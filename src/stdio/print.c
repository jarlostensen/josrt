#ifdef _JOSRT_REQUIRES_IO

#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>

#include "../internal/include/_vprint.h"

static int _stdout_write(FILE* f, const char* s, size_t l) {
	sys_write(f->_fd, s, l);
	//TODO: check for error
	return l;
}

static FILE _stdout = {
	._fd = 1,
	._pos = 0,
	._write = _stdout_write
};

extern int printf(const char* RESTRICT format, ...)
{
	va_list parameters;
	va_start(parameters, format);
	
	int count = _vfprint_impl(&_stdout, format, parameters);
	va_end(parameters);
	return count;
}

int puts(const char* string) {
	return printf("%s\n", string);
}

#endif // _JOSRT_REQUIRES_IO


