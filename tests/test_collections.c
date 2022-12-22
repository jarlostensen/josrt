#include <stdio.h>
#include <extensions/string.h>

#define _JOS_IMPLEMENT_ALLOCATORS
#include <extensions/hex_dump.h>
#include <extensions/linear_allocator.h>

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

static void test_list(void) {

    char stack_heap[0x1000];
    linear_allocator_t *allocator = linear_allocator_create(stack_heap, sizeof(stack_heap));
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

    return 1;
}
