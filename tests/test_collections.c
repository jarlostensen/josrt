#include <stdio.h>
#include <extensions/string.h>

#define _JOSRT_IMPLEMENT_ALLOCATORS
#include <extensions/hex_dump.h>
#include <extensions/linear_allocator.h>
#include <extensions/arena_allocator.h>

#define _JOS_IMPLEMENT_HIVE
#define _JOS_IMPLEMENT_CONTAINERS
#include <extensions/hive.h>
#include <extensions/list.h>

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

static char a_large_buffer[0x200000];

static void test_hive() {

    printf("========================== test_hive\n");

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


static void test_list(void) {

    printf("========================== test_list\n");
    linear_allocator_t *allocator = linear_allocator_create(a_large_buffer, sizeof(a_large_buffer));
    list_t list;
    list_create(&list, sizeof(int), 4, (generic_allocator_t*)allocator);
    const int count = 8;
    for (int i = 0; i < count; ++i) {
        list_push_back(&list, &i);
    }

    for (int i = 0; i < count; ++i) {
        int j = *(int*)list_at(&list, i);
        printf("[%d] = %d\n", i, j);
    }

    list_iterator_t iter;
    list_iterator_begin(&list, &iter);
    do {
        printf("element %d\n", *(int*)list_iterator_at(&iter));
    } while (list_iterator_next(&iter));

    //list_destroy(&list);
}

int main(int argc, char* argv[]) {

    test_list();
    //test_hive();

    return 1;
}
