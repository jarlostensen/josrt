// adapted from MUSL and LLVM headers
//
#ifndef _STDARG_H
#define _STDARG_H

#define __NEED_va_list
#include <bits/alltypes.h>

//TODO: wrap this in Clang/GCC checks
#ifndef _JOS_VA_LIST_DEFINED
    #define _JOS_VA_LIST_DEFINED
    typedef __builtin_va_list va_list;
#endif

#define va_start(v,l)   __builtin_va_start(v,l)
#define va_end(v)       __builtin_va_end(v)
#define va_arg(v,l)     __builtin_va_arg(v,l)
#define va_copy(d,s)    __builtin_va_copy(d,s)

#endif
