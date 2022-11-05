#include <string.h>
#include <stdint.h>

#ifdef SLOW_AND_SAFE
// https://github.com/nadavrot/memset_benchmark/tree/main/src
void* memset(void* bufptr, int value, size_t size) {
	uint32_t v32 = (value * 0x01010101);
	size_t s = size >> 3;
	uint32_t* ptr32 = (uint32_t*)bufptr;
	while(s--)
	{
		*ptr32++ = v32;
		*ptr32++ = v32;
	}

	s = (size & 7)>>2;
	while(s--)
	{
		*ptr32++ = v32;
	}

	s = size & 3;
	unsigned char* cbuf = (unsigned char*)ptr32;
	while(s--)
	{
		*cbuf++ = (unsigned char)value;
	}

	return bufptr;
}
#else
#include <josrt.h>

void* memset(void* dest, int val, size_t size) {
	#define _STRIDE 32
	// TODO: a bit more science; how many YMM stores can we do in one loop effectively?
	#define _UNROLL_FACTOR 4
	if (size > _STRIDE) {
		int alignment = (int)((intptr_t)(dest) & 0xf);
		unsigned char* d8 = (unsigned char*)dest;
		while (alignment--) {
			*d8++ = (char)val;
		}
		const __m256i v256 = _mm256_set_epi32(val, val, val, val, val, val, val, val);
		__m256i* d256 = (__m256i*)d8;
		size_t rem = size % (_STRIDE * _UNROLL_FACTOR);
		size /= (_STRIDE * _UNROLL_FACTOR);
		while (size--) {
			*d256++ = v256;
			*d256++ = v256;
			*d256++ = v256;
			*d256++ = v256;
		}
		d8 = (unsigned char*)d256;
		while (rem--) {
			*d8++ = (unsigned char)val;
		}
	}
	else {
		unsigned int* d32 = (unsigned int*)dest;
		size_t rem = size & 3;
		size >>= 2;
		while (size--) {
			*d32++ = val;
		}
		unsigned char* d8 = (unsigned char*)d32;
		while (rem--) {
			*d8++ = (unsigned char)val;
		}
	}
	return dest;
}
#endif