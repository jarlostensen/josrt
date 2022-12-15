#pragma once
#ifndef _INTERNAL_FILE_H
#define _INTERNAL_FILE_H

#include <stdint.h>
#include <stddef.h>

#define F_PERM 1
#define F_NORD 4
#define F_NOWR 8
#define F_EOF 16
#define F_ERR 32
#define F_SVB 64
#define F_APP 128

typedef struct __FILE_TAG {

	unsigned 	_flags;
    int     	_fd;
	int			_pos;
	
	int(*_read)(struct __FILE_TAG* file, char*, size_t);
	int(*_write)(struct __FILE_TAG* file, const char*, size_t);
	void(*_flush)(struct __FILE_TAG* file);

} __FILE;

#define __feof(f) ((f)->_flags & F_EOF)
#define __ferror(f) ((f)->_flags & F_ERR)

#ifdef _JOSRT_REQUIRES_FILE
_JOSRT_API_FUNC int __file_initialize(dynamic_allocation_policy_t*);

_JOSRT_API_FUNC int fprintf (FILE *f, const char *RESTRICT fmt, ...);
_JOSRT_API_FUNC FILE *fopen(const char *RESTRICT, const char *RESTRICT);
_JOSRT_API_FUNC int fclose(FILE *);

_JOSRT_API_FUNC int feof(FILE *);
_JOSRT_API_FUNC int ferror(FILE *);
_JOSRT_API_FUNC int fflush(FILE *);
_JOSRT_API_FUNC void clearerr(FILE *);

_JOSRT_API_FUNC int fseek(FILE *, long, int);
_JOSRT_API_FUNC long ftell(FILE *);

_JOSRT_API_FUNC size_t fread(void *RESTRICT, size_t, size_t, FILE *RESTRICT);
_JOSRT_API_FUNC size_t fwrite(const void *RESTRICT, size_t, size_t, FILE *RESTRICT);

#endif // _JOSRT_REQUIRES_FILE

#endif // _INTERNAL_FILE_H