#ifndef _JOSRT_ASSERT_H
#define _JOSRT_ASSERT_H

#ifdef NDEBUG
    #define assert(expression) ((void)0)
#else
    #define assert(x) ((void)((x) || (__assert_fail(#x, __FILE__, __LINE__, __func__),0)))
#endif

#endif // _JOSRT_ASSERT_H
