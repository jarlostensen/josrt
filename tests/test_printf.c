#include <stdio.h>

extern void _syscall_write(const char* str, int len);

int sys_write(int fd, const char* data, size_t len) {
    // stdout/stderr
    if (fd == 1 || fd == 2) {
        _syscall_write(data, len);
        return 0;
    }
    // else error...
    return -1;
}

int main(int argc, char* argv[], char* envp[]) {
    const char* message = "Hello, josrt!\n";
    printf("%s", message);
    return 0;
}
