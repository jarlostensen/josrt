#include <string.h>
#include <stdio.h>

#define _JOSRT_IMPLEMENT_ALLOCATORS

#include <extensions/hex_dump.h>
#include <extensions/arena_allocator.h>
#include <extensions/bb_page_allocator.h>

#define _JOSRT_IMPLEMENT_HIVE
#define _JOSRT_IMPLEMENT_CONTAINERS
#include <extensions/hive.h>

static void null_print_hex_line(const char* l) {
    (void)l;
}

static char a_large_buffer[0x200000];

static void test_bb_allocator(void) {
    bb_page_allocator_t allocator;
    bb_page_allocator_create(&allocator, a_large_buffer, 18, NULL);

    void* pages = bb_page_allocator_allocate(&allocator, 11);
    assert(pages);
    bb_page_allocator_free(&allocator, pages, 11);
}

static void test_hive(void) {    
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
}

int main(int argc, char* argv[]) {

    const char* message = "Hello World from josrt!";
    int result = strlen(message);    
    char buffer[1024];
    result = snprintf(buffer, sizeof(buffer), "Hello, this is %s and a %d...", "snprintf", result);

    hex_dump_mem(null_print_hex_line, buffer, result+8, k8bitInt);
    test_hive();

    test_bb_allocator();

    return result;
}
