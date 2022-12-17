# josrt
RT libraries including minimal Libc for my OS/Kernel projects. 

## Build
Plain vanilla CMake. 
If you need IO support (i.e. stuff like printf) then you must define this for CMake as `cmake .. -DJOSRT_REQUIRES_IO=1`. 

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


