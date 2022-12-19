#include <stdio.h>
#include <extensions/string.h>

#define _JOSRT_IMPLEMENT_ALLOCATORS

#include <extensions/hex_dump.h>
#include <extensions/arena_allocator.h>

#define _JOS_IMPLEMENT_HIVE
#define _JOS_IMPLEMENT_CONTAINERS
#include <extensions/hive.h>

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

static void print_hex_line(const char* line) {
    printf("%s\n", line);
}

static void test_hive() {
    static char a_large_buffer[0x200000];

    arena_allocator_t* allocator = arena_allocator_create(a_large_buffer, sizeof(a_large_buffer));
    hive_t hive;
    hive_create(&hive, (generic_allocator_t*)allocator);
    hive_set(&hive, "foo", HIVE_VALUE_INT(1001), HIVE_VALUE_STR("bar"), HIVE_VALUELIST_END);
    hive_set(&hive, "foo", HIVE_VALUE_INT(42), HIVE_VALUE_STR("bar"), HIVE_VALUE_INT(999999), HIVE_VALUELIST_END);
    // create list1 and set some values
	hive_lpush(&hive, "list1",
		HIVE_VALUE_INT(1), HIVE_VALUE_STR("one"),
		HIVE_VALUE_INT(2), HIVE_VALUE_STR("two"),
		HIVE_VALUE_INT(3), HIVE_VALUE_STR("three"),
		HIVE_VALUELIST_END);

	// this will append values to list1
	hive_lpush(&hive, "list1",
		HIVE_VALUE_INT(4), HIVE_VALUE_STR("four"),
		HIVE_VALUE_INT(5), HIVE_VALUE_STR("five"),
		HIVE_VALUE_INT(6), HIVE_VALUE_STR("six"),
		HIVE_VALUELIST_END);

	vector_t values;
	vector_create(&values, 10, sizeof(hive_value_t), (generic_allocator_t*)allocator);
	hive_lget(&hive, "list1", &values);

    // this prints
	//  "1 one 2 two 3 three 4 four 5 five 6 six"
	
	const size_t num_elements = vector_size(&values);
	for (size_t n = 0; n < num_elements; ++n) {
		hive_value_t* hive_value = (hive_value_t*)vector_at(&values, n);
		hive_value_type_t type = hive_value->type;
		switch (type) {
		case kHiveValue_Int:
		{
			printf("%lld ", hive_value->value.as_int);
		}
		break;
		case kHiveValue_Str:
		{
			printf("%s ", hive_value->value.as_str);
		}
		break;
		default:;
		}
	}
    printf("\n");
}


int test_printf(const char* format, ...)
{
  char buf[8192];
  va_list aptr;
  va_start(aptr, format);
  int res = vsnprintf(buf, sizeof(buf), format, aptr);
  printf(buf);
  va_end(aptr);
  return res;
}

int main(int argc, char* argv[], char* envp[]) {    

    const char* message = "Hello World from josrt!";
    size_t len = strlen(message);
    printf("%s (%d)\n", message, len);
    char buffer[1024];
    int result = snprintf(buffer, sizeof(buffer), "Hello, this is %s and a %d!", "snprintf", 42);
    test_printf("The result is \"%s\" returned %d\n", buffer, result);

    hex_dump_mem(print_hex_line, buffer, result+8, k8bitInt);
    test_hive();

    return 0;
}
