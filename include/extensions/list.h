
#ifndef _JOSRT_LIST_H
#define _JOSRT_LIST_H

#include <josrt.h>

// A simple list with paged internal structure.
// The paging ensures that the list can grow without reallocations.
//
// Usage:
//
//  list_t list;
//  list_create(&list, sizeof(int), 4);
//
//  const int count = 8;
//  for (int i = 0; i < count; ++i) {
//      list_push_back(&list, &i);
//  }
//
//  for (int i = 0; i < count; ++i) {
//         int j = *(int*)list_at(&list, i);
//      printf("[%d] = %d\n", i, j);
//  }
//
//  list_iterator_t iter;
//  list_iterator_begin(&list, &iter);
//  do {
//         printf("element %d\n", *(int*)list_iterator_at(&iter));
//   } while (list_iterator_next(&iter));
//
// list_destroy(&list);
//

typedef struct tag_list_page_t {

	struct _list_page_t* _next;	
	size_t _item_count;
} _list_page_t;

typedef struct _list_t {

	_list_page_t* _head;
	_list_page_t* _tail;
	size_t		 _page_capacity;
	size_t		 _element_size;
    generic_allocator_t* _allocator;

} list_t;

typedef struct _list_iterator_t {

	list_t*		  _list;
	_list_page_t* _page;
	int			  _i;

} list_iterator_t;

_JOSRT_API_FUNC void list_create(list_t* list, size_t element_size, size_t min_count, generic_allocator_t* allocator);
_JOSRT_API_FUNC void list_destroy(list_t* list);
_JOSRT_API_FUNC void list_push_back(list_t* list, void* element);
_JOSRT_API_FUNC void* list_at(list_t* list, size_t i);
_JOSRT_API_FUNC void list_iterator_begin(list_t* list, list_iterator_t* iter);
_JOSRT_API_FUNC int list_iterator_next(list_iterator_t* iter);
_JOSRT_INLINE_FUNC  void* list_iterator_at(list_iterator_t* iter) {
	if (!iter->_page) {
		return NULL;
	}
	return (void*)((char*)(iter->_page + 1) + iter->_i * iter->_list->_element_size);
}

#if defined(_JOS_IMPLEMENT_CONTAINERS) && !defined(_JOSRT_LIST_IMPLEMENTED)
#define _JOSRT_LIST_IMPLEMENTED

_JOSRT_API_FUNC void list_create(list_t* list, size_t element_size, size_t min_count, generic_allocator_t* allocator) {
	
	list->_page_capacity = (min_count > 1 ? min_count : 1);
	list->_element_size = element_size;
    list->_allocator = allocator;
	list->_head = (_list_page_t*)list->_allocator->alloc(list->_allocator, 
            element_size * min_count + sizeof(_list_page_t));
	list->_tail = list->_head;
	list->_head->_next = NULL;
	list->_head->_item_count = 0;
}

_JOSRT_API_FUNC void list_push_back(list_t* list, void* element) {
	assert(list);
	assert(list->_tail);
	if (list->_tail->_item_count == list->_page_capacity) {
		// add another page
		_list_page_t* new_page = (_list_page_t*)list->_allocator->alloc(list->_allocator, 
            list->_element_size * list->_page_capacity + sizeof(_list_page_t));
		new_page->_item_count = 0;
		new_page->_next = NULL;		
		list->_tail->_next = new_page;
		list->_tail = new_page;
	}

	char* slot = (char*)(list->_tail + 1) + (list->_tail->_item_count * list->_element_size);
	memcpy(slot, element, list->_element_size);
	++list->_tail->_item_count;
}

_JOSRT_API_FUNC void list_iterator_begin(list_t* list, list_iterator_t* iter) {
	assert(list);
	assert(list->_head);
	iter->_i = 0;
	iter->_list = list;
	iter->_page = list->_head;
}

_JOSRT_API_FUNC int list_iterator_next(list_iterator_t* iter) {
	
	if (!iter->_page || iter->_i == iter->_page->_item_count) {
		// at end
		return 0;
	}
	if (++iter->_i == iter->_list->_page_capacity) {
		iter->_page = iter->_page->_next;
		iter->_i = 0;
		return iter->_page != NULL;
	}
	return (iter->_i < iter->_page->_item_count);
}

_JOSRT_API_FUNC void* list_at(list_t* list, size_t i) {
	assert(list);
	assert(list->_head);

	if (i < list->_page_capacity) {
		return (void*)((char*)(list->_head + 1) + i * list->_element_size);
	}

	_list_page_t* page = list->_head;
	int pages_to_traverse = i / list->_page_capacity;
	while (page && pages_to_traverse) {
		page = page->_next;
		--pages_to_traverse;
	}
	if (!pages_to_traverse) {
		return (void*)((char*)(page + 1) + (i % list->_page_capacity) * list->_element_size);
	}

	return NULL;
}

_JOSRT_API_FUNC void list_destroy(list_t* list) {
	if (!list || !list->_head) {
		return;
	}
	_list_page_t* page = list->_head;
	while (page) {
		_list_page_t* this_page = page;
		page = page->_next;
		list->_allocator->free(list->_allocator, this_page);
	}
	list->_head = list->_tail = NULL;
}

#endif // defined(_JOS_IMPLEMENT_CONTAINERS) && !defined(_JOSRT_LIST_IMPLEMENTED)

#endif // _JOSRT_LIST_H