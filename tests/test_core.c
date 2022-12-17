#include <string.h>
#include <stdio.h>
#include <extensions/hex_dump.h>

void null_print_hex_line(const char* l) {
    (void)l;
}

int main(int argc, char* argv[]) {

    const char* message = "Hello World from josrt!";
    int result = strlen(message);    
    char buffer[1024];
    result = snprintf(buffer, sizeof(buffer), "Hello, this is %s and a %d...", "snprintf", result);

    hex_dump_mem(null_print_hex_line, buffer, result+8, k8bitInt);

    return result;
}
