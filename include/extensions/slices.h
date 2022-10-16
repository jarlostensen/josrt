#pragma once
// slices for different types of arrays

#include <string.h>
#include <josrt.h>

typedef struct _char_array_slice {

    const char*     _ptr;
    size_t          _length;

} char_array_slice_t;

extern const char_array_slice_t kEmptySlice;

_JOSRT_INLINE_FUNC _JOSRT_ALWAYS_INLINE bool str_is_empty(const char* str) {
    return !str || str[0]==0;
}

_JOSRT_INLINE_FUNC _JOSRT_ALWAYS_INLINE bool slice_is_empty(const char_array_slice_t* slice) {
    return !slice || slice->_length == 0 || slice->_ptr == 0;
}

_JOSRT_INLINE_FUNC _JOSRT_ALWAYS_INLINE char_array_slice_t* char_array_slice_create(char_array_slice_t* slice, const char* charray, size_t offset, size_t length) {
    slice->_ptr = charray + offset;
    slice->_length = length ? length : (strlen(charray) - offset);    
    return slice;
}

_JOSRT_API_FUNC bool char_array_slice_match_str(char_array_slice_t* slice, const char* str);
_JOSRT_API_FUNC bool char_array_slice_equals(char_array_slice_t prefix, char_array_slice_t candidate);
