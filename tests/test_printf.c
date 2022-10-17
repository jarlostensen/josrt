#include <stdio.h>

extern void _syscall_write(const char* str, int len);

int main(int argc, char* argv[], char* envp[]) {
    const char* message = "Hello, syscall\n";
    _syscall_write(message, strlen(message));
    //printf("Hello %s!\n", "world");
    return 0;
}
