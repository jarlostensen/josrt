/*
 * Various GCC specific defines. 
 * 
 * Adapted from Zephyr OS, parts Copyright (c) 2010-2014,2017 Wind River Systems, Inc.
 * SPDX-License-Identifier: Apache-2.0
 *
 */

#ifndef ZEPHYR_INCLUDE_TOOLCHAIN_GCC_H_
#define ZEPHYR_INCLUDE_TOOLCHAIN_GCC_H_

/**
 * @file
 * @brief GCC toolchain abstraction
 *
 * Macros to abstract compiler capabilities for GCC toolchain.
 */

#define GCC_VERSION \
	((__GNUC__ * 10000) + (__GNUC_MINOR__ * 100) + __GNUC_PATCHLEVEL__)

/* GCC supports #pragma diagnostics since 4.6.0 */
#if !defined(TOOLCHAIN_HAS_PRAGMA_DIAG) && (GCC_VERSION >= 40600)
#define TOOLCHAIN_HAS_PRAGMA_DIAG 1
#endif

#if !defined(TOOLCHAIN_HAS_C_GENERIC) && (GCC_VERSION >= 40900)
#define TOOLCHAIN_HAS_C_GENERIC 1
#endif

#if !defined(TOOLCHAIN_HAS_C_AUTO_TYPE) && (GCC_VERSION >= 40900)
#define TOOLCHAIN_HAS_C_AUTO_TYPE 1
#endif

#if (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)) || \
	(__STDC_VERSION__) >= 201100
#define BUILD_ASSERT(EXPR, MSG...) _Static_assert(EXPR, "" MSG)
#else
#define BUILD_ASSERT(EXPR, MSG...)
#endif


#ifdef __cplusplus
#define RESTRICT __restrict
#else
#define RESTRICT restrict
#endif

#include <stdbool.h>

#define CODE_UNREACHABLE __builtin_unreachable()
#define FUNC_NORETURN    __attribute__((__noreturn__))

/* Unaligned access */
#define UNALIGNED_GET(p)						\
__extension__ ({							\
	struct  __attribute__((__packed__)) {				\
		__typeof__(*(p)) __v;					\
	} *__p = (__typeof__(__p)) (p);					\
	__p->__v;							\
})

#define UNALIGNED_PUT(v, p)                                             \
do {                                                                    \
	struct __attribute__((__packed__)) {                            \
		__typeof__(*p) __v;                                     \
	} *__p = (__typeof__(__p)) (p);                                 \
	__p->__v = (v);                                               \
} while (false)

/* Double indirection to ensure section names are expanded before
 * stringification
 */
#define __GENERIC_SECTION(segment) __attribute__((section(STRINGIFY(segment))))
#define Z_GENERIC_SECTION(segment) __GENERIC_SECTION(segment)

#define __GENERIC_DOT_SECTION(segment) \
	__attribute__((section("." STRINGIFY(segment))))
#define Z_GENERIC_DOT_SECTION(segment) __GENERIC_DOT_SECTION(segment)

#define ___in_section(a, b, c) \
	__attribute__((section("." Z_STRINGIFY(a)			\
				"." Z_STRINGIFY(b)			\
				"." Z_STRINGIFY(c))))
#define __in_section(a, b, c) ___in_section(a, b, c)
#define __in_section_unique(seg) ___in_section(seg, __FILE__, __COUNTER__)
#define __in_section_unique_named(seg, name) \
	___in_section(seg, __FILE__, name)

#ifndef __fallthrough
	#if __GNUC__ >= 7
	#define __fallthrough        __attribute__((fallthrough))
	#else
	#define __fallthrough
	#endif	/* __GNUC__ >= 7 */
#endif

#ifndef __packed
#define __packed        __attribute__((__packed__))
#endif

#ifndef __aligned
#define __aligned(x)	__attribute__((__aligned__(x)))
#endif

#ifndef __printf_like
#define __printf_like(f, a)   __attribute__((format (printf, f, a)))
#endif

#define __used		__attribute__((__used__))
#define __unused	__attribute__((__unused__))
#define __maybe_unused	__attribute__((__unused__))

#ifndef __deprecated
#define __deprecated	__attribute__((deprecated))
#endif

#ifndef __attribute_const__
#define __attribute_const__ __attribute__((__const__))
#endif

#ifndef __must_check
#define __must_check __attribute__((warn_unused_result))
#endif

#ifndef __always_inline
#define __always_inline __attribute__((always_inline))
#endif

#define ARG_UNUSED(x) (void)(x)

#define likely(x)   (__builtin_expect((bool)!!(x), true) != 0L)
#define unlikely(x) (__builtin_expect((bool)!!(x), false) != 0L)
#define popcount(x) __builtin_popcount(x)

#ifndef __no_optimization
#define __no_optimization __attribute__((optimize("-O0")))
#endif

#ifndef __weak
#define __weak __attribute__((__weak__))
#endif


/* Builtins with availability that depend on the compiler version. */
#if __GNUC__ >= 5
#define HAS_BUILTIN___builtin_add_overflow 1
#define HAS_BUILTIN___builtin_sub_overflow 1
#define HAS_BUILTIN___builtin_mul_overflow 1
#define HAS_BUILTIN___builtin_div_overflow 1
#endif
#if __GNUC__ >= 4
#define HAS_BUILTIN___builtin_clz 1
#define HAS_BUILTIN___builtin_clzl 1
#define HAS_BUILTIN___builtin_clzll 1
#define HAS_BUILTIN___builtin_ctz 1
#define HAS_BUILTIN___builtin_ctzl 1
#define HAS_BUILTIN___builtin_ctzll 1
#endif


#define compiler_barrier() do { \
	__asm__ __volatile__ ("" ::: "memory"); \
} while (false)

/**
 * @brief Calculate power of two ceiling for some nonzero value
 *
 * @param x Nonzero unsigned long value
 * @return X rounded up to the next power of two
 */
#define _POW2_CEIL(x) \
	((x) <= 2UL ? (x) : (1UL << (8 * sizeof(long) - __builtin_clzl((x) - 1))))

/**
 * @brief Check whether or not a value is a power of 2
 *
 * @param x The value to check
 * @return true if x is a power of 2, false otherwise
 */
#define _IS_POW2(x) (((x) != 0) && (((x) & ((x)-1)) == 0))

#define _JOSRT_ALWAYS_INLINE __attribute__((always_inline))
#define _JOSRT_ALIGNED_TYPE(type,name,alignment) type name __attribute__ ((aligned (alignment)))

#endif /* ZEPHYR_INCLUDE_TOOLCHAIN_GCC_H_ */
