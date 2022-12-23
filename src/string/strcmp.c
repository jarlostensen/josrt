
#include <string.h>

// from musl
int strcmp(const char *l, const char *r)
{
	for (; *l==*r && *l; l++, r++);
	return *(unsigned char *)l - *(unsigned char *)r;
}

int strncmp(const char* l, const char* r, size_t max_len) {
   for (; max_len && *l == *r && *l; l++, r++, max_len--);
    return *(unsigned char*)l - *(unsigned char*)r;
}
