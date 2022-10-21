
export void ispc_memcpy(int8 * uniform dst, const int8 * uniform src, const uniform int64 count) {
    foreach (index = 0 ... count) {        
        dst[index] = src[index];
    }
}
