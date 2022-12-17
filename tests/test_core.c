#include <string.h>
#include <stdio.h>

int main(int argc, char* argv[]) {

    const char* message = "Hello World from josrt!";
    int result = strlen(message);    
    char buffer[1024];
    result = snprintf(buffer, sizeof(buffer), "Hello, this is %s and a %d...", "snprintf", result);
    return result;
}
