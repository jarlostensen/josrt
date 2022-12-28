#pragma once

#ifndef _JOSRT_KERNEL_HEX_DUMP_H
#define _JOSRT_KERNEL_HEX_DUMP_H

#include <stdio.h>

enum hex_dump_unit_size
{
	k8bitInt,
	k16bitInt,
	k32bitInt,
	k64bitInt,
};

typedef void (*hex_dump_write_line_func_t)(const char*);
void hex_dump_mem(hex_dump_write_line_func_t write_line_func, const void* mem, size_t bytes, enum hex_dump_unit_size unit_size);

#endif // _JOSRT_KERNEL_HEX_DUMP_H

