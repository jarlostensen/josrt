#pragma once
#ifndef _JOSRT_H
#define _JOSRT_H

#include <joBase.h>

#if defined(__clang__) || defined(__GNUC__)
    #define ASM_SYNTAX_ATNT    
#else 
    #define ASM_SYNTAX_INTEL
#endif

#if defined(_JO_BARE_METAL_BUILD) && !defined(_JOSRT_KERNEL_BUILD)
// for backwards compatibility
#define _JOSRT_KERNEL_BUILD
#endif

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

// a static allocation policy means that memory will be allocated, but never freed.
// the allocator can therefore be linear
typedef struct _static_allocation_policy {
    generic_allocator_t *   allocator;
    // needed to prevent the compiler from implicitly casting this struct to an allocator*
    bool                    __no_implicit_ptr_cast; 
} static_allocation_policy_t;

// a dynamic allocation policy requires an allocator that can free and reallocate memory
typedef struct _dynamic_allocation_policy {
    generic_allocator_t *   allocator;
    // needed to prevent the compiler from implicitly casting this struct to an allocator*
    bool                    __no_implicit_ptr_cast;
} dynamic_allocation_policy_t;

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
//TODO: TEMPORARY, this should really be something else (barebones?)
    #define _JOSRT_KERNEL_BUILD        
    #include <toolchain/llvm.h>
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

 #define _JOSRT_MAX(a,b) \
   ({ __typeof__ (a) _a = (a); \
    __typeof__ (b) _b = (b); \
    _a > _b ? _a : _b; })
#undef max
#define max(a,b)\
    ((a) > (b) ? (a) : (b))


#ifdef _JOSRT_KERNEL_BUILD

#define _JOSRT_MAYBE_UNUSED __attribute__((unused))
#define _JOSRT_INLINE_FUNC __attribute__((unused)) static
#define _JOSRT_API_FUNC extern
#define _JOSRT_ALWAYS_INLINE __attribute__((always_inline))

#define _JOSRT_PACKED_ __attribute((packed))
#define _JOSRT_UNREACHABLE() __builtin_unreachable()

void trace(const char* channel, const char* msg,...);
#define _JOSRT_KTRACE_CHANNEL(channel, msg, ...) trace(channel, msg, ##__VA_ARGS__)
#define _JOSRT_KTRACE(msg, ...)  trace(0, msg, ##__VA_ARGS__)

void trace_buf(const char* channel, const void* data, size_t length);
#define _JOSRT_KTRACE_CHANNEL_BUF(channel, data,length) trace_buf(channel, data, length)
#define _JOSRT_KTRACE_BUF(data,length) trace_buf(0, data, length)

#define _JOSRT_BOCHS_DBGBREAK() asm volatile ("xchg %bx,%bx")
#define _JOSRT_GDB_DBGBREAK() __asm__ volatile ("int $03")

#define _JOSRT_BOCHS_DBGBREAK_TRACE()\
trace(0, "break at %s:%d\n", __FILE__,__LINE__);\
asm volatile ("xchg %bx,%bx")

#define _JOSRT_ASSERT_COND(cond) #cond
#define _JOSRT_ASSERT(cond)\
if(!(cond))\
{\
    if ( !debugger_is_connected() ) {\
        trace(0, "assert %s, %s:%d \n", _JOSRT_ASSERT_COND(cond), __FILE__,__LINE__);\
        _JOSRT_GDB_DBGBREAK();\
    }\
    else {\
        debugger_trigger_assert(_JOSRT_ASSERT_COND(cond), __FILE__, __LINE__);\
    }\
}

#define _JOSRT_ALIGNED_TYPE(type,name,alignment) type name __attribute__ ((aligned (alignment)))

#define _JOSRT_PACKED __attribute__((packed))
#define _JOSRT_NORETURN __attribute__((__noreturn__))

#define _JOSRT_KERNEL_PANIC()\
    trace(0, "PANIC @ %s:%d \n", __FILE__,__LINE__);\
    k_panic()

#ifndef min
#define min(a,b) ((a)<(b) ? (a) : (b))
#endif

#else
//TODO: check if this is actually VS, but we're assuming it because we're in control...
#define _JOSRT_UNREACHABLE()
#define _JOSRT_MAYBE_UNUSED
#define _JOSRT_INLINE_FUNC static
#define _JOSRT_API_FUNC extern
#define _JOSRT_BOCHS_DBGBREAK() __debugbreak()

#define _JOSRT_KTRACE_CHANNEL(channel, msg,...)
#define _JOSRT_KTRACE(msg,...)
#define _JOSRT_KTRACE_CHANNEL_BUF(channel, data,length)
#define _JOSRT_KTRACE_BUF(data,length)

#define _JOSRT_ALWAYS_INLINE

#define _JOSRT_BOCHS_DBGBREAK_TRACE() __debugbreak()
#define _JOSRT_GDB_DBGBREAK() __debugbreak()

#define _JOSRT_PACKED_
#define _JOSRT_NORETURN

#define _JOSRT_KERNEL_PANIC()

#ifdef _DEBUG
#define _JOSRT_ASSERT(cond) if(!(cond)) { __debugbreak(); }
#else
#define _JOSRT_ASSERT(cond)
#endif
#define _JOSRT_ALIGNED_TYPE(type, name, alignment) __declspec(align(alignment)) type name
#endif

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