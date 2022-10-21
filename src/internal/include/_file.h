#pragma once
#ifndef _INTERNAL_FILE_H
#define _INTERNAL_FILE_H

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

typedef struct __FILE_TAG {

	// a string, for example
	size_t  _pos;
    int     _fd;

	int(*read)(struct __FILE_TAG* file, char*, size_t);
	int(*write)(struct __FILE_TAG* file, const char*, size_t);
	void(*flush)(struct __FILE_TAG* file);

} __FILE;

#endif // _INTERNAL_FILE_H