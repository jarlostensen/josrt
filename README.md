# josrt
RT libraries including minimal Libc for my OS/Kernel projects. 

## Build
Plain vanilla CMake and Clang (*required*).
If you need IO support (i.e. stuff like printf) then you must define this for CMake as `cmake .. -DJOSRT_REQUIRES_IO=1`. 

This is an example of a full build with all the tests
```
cmake .. -DCMAKE_BUILD_TYPE=Debug -DCMAKE_C_COMPILER=/usr/bin/clang -DJOSRT_BUILD_TESTS=1
cmake --build .
```

### Building for bare metal
If you want to use the library for bare metal builds without a CRT (i.e. OS kernels) you need to provide `-DJOSRT_BARE_METAL=1` as a `cmake` argument. In this mode there is no safety net!

## Platform support
These libraries are (at present) 100% x86-64 + AVX2 and will not work otherwise. 
I want to port to aarm64 eventually but I have no intention of supporting anything "lesser".

## Functionality
* A basic set of std C library functions (like memcpy, strlen, etc.) 
* Basic FILE support for I/O.
* Minimal CRT startup code and hooks for "syscall" support of low-level functionality.
* My own extensions which include vector, queue, map, and other tidbits of usefulCness.

## Memory allocation & the Heap
josRt has *no* malloc or free functionality.
It is expected that the caller (either a program or a kernel) provides heap allocation and any code inside josRt that requires dynamic memory explicitly takes an `allocator` argument. 

## Sources
I've shamelessly incorporated code from Musl, Zephyr OS, and LLVM 


