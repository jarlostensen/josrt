#pragma once
#ifndef _JOSRT_H
#define _JOSRT_H

//TODO: #include <joBase.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef _JOSRT_BARE_BONES_BUILD
    // no explicit FILE support
    #undef _JOSRT_REQUIRES_FILE
    // no other IO support (no stdout/stdin/stderr)
    #undef _JOSRT_REQUIRES_IO
#endif

// #define _JOSRT_KERNEL_BUILD for bare bones build (this will also remove some libc functionality)

// ======================================================
// Allocator interfaces
// There is no global malloc/free, all allocations are made from heaps or page pools.
// 

// allocation alignment in BYTES
typedef enum _alloc_alignment {

	// any byte alignment
	kAllocAlign_None = 1,
	// eight bytes
	kAllocAlign_8 = 8,
	// sixteen bytes
	kAllocAlign_16 = 16,
	// thirty two bytes
	kAllocAlign_32 = 32,
	// sixty four bytes
	kAllocAlign_64 = 64,
	// one hundred and twenty eight bytes
	kAllocAlign_128 = 128,
	// two hundred and fifty six bytes
	kAllocAlign_256 = 256,
	// four kilobytes
	kAllocAlign_4k = 0x1000,

	// not used
	kAllocAlign_Max

} alloc_alignment_t;

// =================================================================
// generic allocator interface

struct _generic_allocator;
// allocate bytes. ALLWAYS a minimum of 8 byte aligned
typedef void* (*generic_allocator_alloc_func_t)(struct _generic_allocator*, size_t);
typedef void  (*generic_allocator_free_func_t)(struct _generic_allocator*, void*);
// allocate bytes. ALLWAYS a minimum of 8 byte aligned
typedef void* (*generic_allocator_realloc_func_t)(struct _generic_allocator*, void*, size_t);
typedef size_t (*generic_allocator_avail_func_t)(struct _generic_allocator*);

typedef struct _generic_allocator {
    generic_allocator_alloc_func_t      alloc;
    generic_allocator_free_func_t       free;
    generic_allocator_realloc_func_t    realloc;
    generic_allocator_avail_func_t      available;

} generic_allocator_t;

#define _JOSRT_ALLOCATOR_POLICY(name)\
typedef struct _##name {\
    generic_allocator_t *   allocator;\
    int                    __force_no_implicit_ptr_cast;\
} name##_t

// allocator policy which supports allocation but no free or realloc
_JOSRT_ALLOCATOR_POLICY(static_allocation_policy);
// allocator policy which supports dynamic (general) allocation, free, and realloc
_JOSRT_ALLOCATOR_POLICY(dynamic_allocation_policy);

// =================================================================
// page allocator interface

#define PAGE_NOACCESS           0x01
#define PAGE_READONLY           0x02
#define PAGE_READWRITE          0x04
#define PAGE_EXECUTE            0x10
#define PAGE_EXECUTE_READ       0x20
#define PAGE_EXECUTE_READWRITE  0x40
#define PAGE_GUARD             0x100
#define PAGE_NOCACHE           0x200
#define PAGE_WRITECOMBINE      0x400

// page allocator interface
// pages can be allocated and protected
struct _page_allocator;
typedef void* (*page_allocator_alloc_func_t)(struct _page_allocator*, size_t, unsigned int flags);
typedef void* (*page_allocator_free_func_t)(struct _page_allocator*, void*);
typedef void* (*page_allocator_protect_func_t)(struct _page_allocator*, void*, size_t, unsigned int flags);

typedef struct _page_allocator {
    // allocate, page aligned
    page_allocator_alloc_func_t   alloc;
    // free page aligned
    page_allocator_free_func_t    free;
    // protect page aligned
    page_allocator_protect_func_t protect;
} page_allocator_t;


// ========================================================================= misc types

// generic position
typedef struct _pos {

    size_t      x;
    size_t      y;

} pos_t;
// generic rectangle [top, bottom>, [left, right>
typedef struct _rect {
    size_t      top;
    size_t      left;
    size_t      bottom;
    size_t      right;
} rect_t;

#if defined(__llvm__)    
    #include <toolchain/llvm.h>
    #include <x86intrin.h>
    #define _JOSRT_TOOLCHAIN_CLANG
#elif defined(__GNUC__) || (defined(_LINKER) && defined(__GCC_LINKER_CMD__))
    #include "toolchain/gcc.h"
    #define _JOSRT_TOOLCHAIN_GCC
#else
    //TODO: VisualStudio
    #error "Invalid/unknown toolchain configuration"
#endif

#define _JOSRT_SWAP(a,b)\
    (a) ^= (b);\
    (b) ^= (a); \
    (a) ^= (b)
#if !defined(__clang__)
    #define _JOSRT_SWAP_PTR(a,b)\
    ((uintptr_t)(a)) ^= ((uintptr_t)(b));\
    ((uintptr_t)(b)) ^= ((uintptr_t)(a));\
    ((uintptr_t)(a)) ^= ((uintptr_t)(b))
#endif

#ifndef max
#define max(a,b)\
    ((a) > (b) ? (a) : (b))
#endif
#ifndef min
#define min(a,b)\
    ((a) < (b) ? (a) : (b))
#endif

#define _JOSRT_MAYBE_UNUSED __unused
#define _JOSRT_INLINE_FUNC __unused static
#define _JOSRT_API_FUNC extern

#define _JOSRT_PTR_IS_ALIGNED(ptr, alignment)\
    (((uintptr_t)ptr & ((uintptr_t)alignment - 1)) == 0)

#define _JOSRT_ALIGN(val, alignment)\
    (((uintptr_t)val + ((uintptr_t)alignment - 1)) & ~((uintptr_t)alignment - 1))

_JOSRT_INLINE_FUNC void aligned_alloc(generic_allocator_t* allocator, size_t bytes, alloc_alignment_t alignment, 
                                    void** out_alloc_base, void** out_alloc_aligned) {
    if (!allocator || !bytes) {
        *out_alloc_base = *out_alloc_aligned = 0;
        return;
    }
    void* ptr = allocator->alloc(allocator, bytes + (size_t)alignment - 1);
    *out_alloc_base = ptr;
    if (!_JOSRT_PTR_IS_ALIGNED(ptr, alignment)) {
        *out_alloc_aligned = (void*)_JOSRT_ALIGN(ptr, alignment);
    }
    else {
        *out_alloc_aligned = ptr;
    }
}

// ================================================
// warnings

#pragma warning(disable:4200)
#pragma warning(disable:4201)
#pragma warning(disable:4213)

#endif // _JOSRT_H
