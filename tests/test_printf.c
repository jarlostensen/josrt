#include <stdio.h>
#include <extensions/string.h>

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

    const char* message = "Hello World from josrt!";
    size_t len = strlen(message);
    printf("%s (%d)\n", message, len);

    char buffer[1024];
    int result = snprintf(buffer, sizeof(buffer), "Hello, this is %s and a %d...", "snprintf", 42);
    printf("The result is \"%s\" returned %d\n", buffer, result);

    return 0;
}
