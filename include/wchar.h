#pragma once
#ifndef _JOSRT_WCHAR_H
#define _JOSRT_WCHAR_H

#include <stdint.h>
#include <stddef.h>

#ifndef _JOSRT_WCHAR_DEFINED
    #define _JOSRT_WCHAR_DEFINED
    typedef uint16_t   wchar_t;
#endif

size_t wcslen( const int *str );

#endif // _JOSRT_WCHAR_H

