#include <josrt.h>
#include <extensions/string.h>

size_t josrt_ascii_strlen(const char* s_) {
	// first align to 16 byte boundary
	int alignment = (int)((intptr_t)(s_) & 0xf);
	const char* s = s_;
	while (alignment--) {
		if (!s[0]) {
			return (size_t)(s - s_);
		}
		++s;
	}
	// adding this to each of the 16 bytes we've read will carry if and only if the byte != 0
	const __m128i carry_mask = _mm_set_epi64x(0x7f7f7f7f7f7f7f7f, 0x7f7f7f7f7f7f7f7f);
	const __m128i* s128 = (const __m128i*)s;
	while (1) {		
		const __m128i masked = _mm_add_epi8(*s128, carry_mask);
		unsigned long mask = (unsigned long)_mm_movemask_epi8(masked); //< get the "carry" bits
		if (mask!=0xffff) {
			// there's a zero in there somewhere!
			mask = (~mask) & 0xffff;
#if defined(_JOSRT_TOOLCHAIN_CLANG) || defined(_JOSRT_TOOLCHAIN_GCC)            
			unsigned long i =__builtin_ffs(mask)-1;
#else
	#error "na" 
#endif
			return (size_t)((const char*)s128 - s_) + i;
		}
		s128++;
	}
}
