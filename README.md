# josrt
RT libraries including minimal Libc for my OS/Kernel projects. 

## Build
Plain vanilla CMake. 

## Platform support
These libraries are (at present) 100% x86-64 + AVX2 and will not work otherwise. 
I want to port to aarm64 eventually but I have no intention of supporting anything "lesser".

## Functionality
* A basic set of std C library functions (like memcpy, strlen, etc.) 
* Basic FILE support for I/O.
* Minimal CRT startup code and hooks for "syscall" support of low-level functionality.
* My own extensions which include vector, queue, map, and other tidbits of usefulCness.

## Sources
I've shamelessly incorporated code from Musl, Zephyr OS, and LLVM 
