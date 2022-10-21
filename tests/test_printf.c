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

extern int TEST_snprintf(char* buffer, size_t bufsz, const char* format, ...);

int main(int argc, char* argv[], char* envp[]) {
    const char* message = "Hello, josrt!\n";
    printf("%s", message);

    char buffer[1024];
    int result = snprintf(buffer, sizeof(buffer), "Hello, this is %s and a %d...", "snprintf", 42);
    printf("The result is \"%s\" returned %d\n", buffer, result);

    return 0;
}
