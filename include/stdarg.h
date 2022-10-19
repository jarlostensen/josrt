// adapted from MUSL and LLVM headers
//
#ifndef _STDARG_H
#define _STDARG_H

#include <bits/alltypes.h>
#include <josrt.h>

#if defined(_JOSRT_TOOLCHAIN_CLANG) || defined(_JOSRT_TOOLCHAIN_GCC)
    #ifndef va_list
	    typedef __builtin_va_list va_list;
    #endif
    #define va_start(v,l)   __builtin_va_start(v,l)
    #define va_end(v)       __builtin_va_end(v)
    #define va_arg(v,l)     __builtin_va_arg(v,l)
    #define va_copy(d,s)    __builtin_va_copy(d,s)
#else
    #error "Unsupporter compiler"
#endif

#endif
