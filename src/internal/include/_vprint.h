#pragma once

#include "libc_internal.h"
#include "_file.h"

#define _PRINTF_FILE_LINE_LENGTH 256
typedef struct _printf_file_tag
{
	__FILE _f;
	char    _line[_PRINTF_FILE_LINE_LENGTH];
	size_t	_wp;

} _printf_file_t;


#define CHAR char
#define DUAL_CHAR wchar_t
#define STRLEN strlen
#define TEXT(s) (s)

int _vfprint_impl(FILE* f, const CHAR* RESTRICT format, va_list parameters);