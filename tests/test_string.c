#include <string.h>
#include <stdio.h>

extern void ispc_memcpy(void* dest, const void* src, size_t len);

int main(int argc, char* argv[]) {

    ispc_memcpy(0, 0, 0);

    return 0;
}