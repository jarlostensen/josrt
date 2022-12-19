#include <josrt.h>
#include <libc_hooks.h>

FUNC_NORETURN void __assert_fail(const char *expr, const char *file, int line, const char *func)
{
	//TODO: fprintf(stderr, "Assertion failed: %s (%s: %s: %d)\n", expr, file, func, line);
	__exit(-1);
}
