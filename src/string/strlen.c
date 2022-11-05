#include <string.h>
#include <stdint.h>

#if SLOW_BUT_SAFE
size_t strlen(const char* str) {
	size_t len = 0;
	while (str[len])
		len++;
	return len;
}
#else
#include <josrt.h>

size_t strlen(const char* s_) {
    // ported to 128 bit from https://tsunanet.net/~tsuna/strlen.c.html 
    
	// first align to 16 byte boundary
	int alignment = (int)((intptr_t)(s_) & 0xf);
	const char* s = s_;
	while (alignment--) {
		if (!s[0]) {
			return (size_t)(s - s_);
		}
		++s;
	}	
	static int _magic_set = 0;
	static __m128i lo_magic;
	if (!_magic_set) {
		// subtracting this will yield 0xff at a 0 byte location but won't do much else so this 
		// is just a fast early out
		lo_magic = _mm_set_epi64x(0x0101010101010101, 0x0101010101010101);
		_magic_set = 1;
	}
	const __m128i* s128 = (const __m128i*)s;
	while (1) {
		const __m128i diff = _mm_sub_epi8(*s128, lo_magic);
		int hi_bits = _mm_movemask_epi8(diff); //< get the "carry" bits
		if (hi_bits != 0) {
			// there might be a zero in there, but it could also be a false positive
			const char *seg = (const char*)s128;
#define CHECK_NULL_BYTE(n)\
			if (seg[(n)] == 0) return seg - s_ + (n)
			CHECK_NULL_BYTE(0);
			CHECK_NULL_BYTE(1);
			CHECK_NULL_BYTE(2);
			CHECK_NULL_BYTE(3);
			CHECK_NULL_BYTE(4);
			CHECK_NULL_BYTE(5);
			CHECK_NULL_BYTE(6);
			CHECK_NULL_BYTE(7);
			CHECK_NULL_BYTE(8);
			CHECK_NULL_BYTE(9);
			CHECK_NULL_BYTE(10);
			CHECK_NULL_BYTE(11);
			CHECK_NULL_BYTE(12);
			CHECK_NULL_BYTE(13);
			CHECK_NULL_BYTE(14);
			CHECK_NULL_BYTE(15);
		}
		s128++;
	}
}
#endif