#include <string.h>
#include <stdint.h>
#include <josrt.h>

void* memcpy(void* RESTRICT dstptr, const void* RESTRICT srcptr, size_t size) {

	//NOTE: 64 bit only!
	uint64_t* dst = (uint64_t*) dstptr;
	size_t rem = size & 7;
	size >>= 3;
	const uint64_t* src = (uint64_t*)srcptr;
	for(size_t n = 0; n < size; ++n)
		*dst++ = *src++;
	unsigned char* cdst = (unsigned char*) dst;
	const unsigned char* csrc = (const unsigned char*) src;
	while(rem--)
		*cdst++ = *csrc++;
	return dstptr;
}
