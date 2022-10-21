#pragma once

#include "libc_internal.h"
#include "_file.h"

typedef struct _printf_file_tag
{
	__FILE _f;
	char*	_buffer;
	char* 	_wp;
	const char* _end;

} _printf_file_t;


#define CHAR char
#define DUAL_CHAR wchar_t
#define STRLEN strlen
#define TEXT(s) (s)

int _vfprint_impl(FILE* f, const CHAR* RESTRICT format, va_list parameters);