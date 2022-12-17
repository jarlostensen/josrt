#include <string.h>
#include <stdint.h>
#include <josrt.h>

// used as a fallback
void* musl_memcpy(void* RESTRICT dest, const void* RESTRICT src, size_t n)
{
	unsigned char* d = dest;
	const unsigned char* s = src;

#if __BYTE_ORDER == __LITTLE_ENDIAN
#define LS >>
#define RS <<
#else
#define LS <<
#define RS >>
#endif

#ifdef __GNUC__
	typedef uint32_t __attribute__((__may_alias__)) u32;
#else
	typedef uint32_t u32;
#endif
	uint32_t w, x;

	for (; (uintptr_t)s % 4 && n; n--) *d++ = *s++;

	if ((uintptr_t)d % 4 == 0) {
		for (; n >= 16; s += 16, d += 16, n -= 16) {
			*(u32*)(d + 0) = *(u32*)(s + 0);
			*(u32*)(d + 4) = *(u32*)(s + 4);
			*(u32*)(d + 8) = *(u32*)(s + 8);
			*(u32*)(d + 12) = *(u32*)(s + 12);
		}
		if (n & 8) {
			*(u32*)(d + 0) = *(u32*)(s + 0);
			*(u32*)(d + 4) = *(u32*)(s + 4);
			d += 8; s += 8;
		}
		if (n & 4) {
			*(u32*)(d + 0) = *(u32*)(s + 0);
			d += 4; s += 4;
		}
		if (n & 2) {
			*d++ = *s++; *d++ = *s++;
		}
		if (n & 1) {
			*d = *s;
		}
		return dest;
	}

	if (n >= 32) switch ((uintptr_t)d % 4) {
	case 1:
		w = *(u32*)s;
		*d++ = *s++;
		*d++ = *s++;
		*d++ = *s++;
		n -= 3;
		for (; n >= 17; s += 16, d += 16, n -= 16) {
			x = *(u32*)(s + 1);
			*(u32*)(d + 0) = (w LS 24) | (x RS 8);
			w = *(u32*)(s + 5);
			*(u32*)(d + 4) = (x LS 24) | (w RS 8);
			x = *(u32*)(s + 9);
			*(u32*)(d + 8) = (w LS 24) | (x RS 8);
			w = *(u32*)(s + 13);
			*(u32*)(d + 12) = (x LS 24) | (w RS 8);
		}
		break;
	case 2:
		w = *(u32*)s;
		*d++ = *s++;
		*d++ = *s++;
		n -= 2;
		for (; n >= 18; s += 16, d += 16, n -= 16) {
			x = *(u32*)(s + 2);
			*(u32*)(d + 0) = (w LS 16) | (x RS 16);
			w = *(u32*)(s + 6);
			*(u32*)(d + 4) = (x LS 16) | (w RS 16);
			x = *(u32*)(s + 10);
			*(u32*)(d + 8) = (w LS 16) | (x RS 16);
			w = *(u32*)(s + 14);
			*(u32*)(d + 12) = (x LS 16) | (w RS 16);
		}
		break;
	case 3:
		w = *(u32*)s;
		*d++ = *s++;
		n -= 1;
		for (; n >= 19; s += 16, d += 16, n -= 16) {
			x = *(u32*)(s + 3);
			*(u32*)(d + 0) = (w LS 8) | (x RS 24);
			w = *(u32*)(s + 7);
			*(u32*)(d + 4) = (x LS 8) | (w RS 24);
			x = *(u32*)(s + 11);
			*(u32*)(d + 8) = (w LS 8) | (x RS 24);
			w = *(u32*)(s + 15);
			*(u32*)(d + 12) = (x LS 8) | (w RS 24);
		}
		break;
	}
	if (n & 16) {
		*d++ = *s++; *d++ = *s++; *d++ = *s++; *d++ = *s++;
		*d++ = *s++; *d++ = *s++; *d++ = *s++; *d++ = *s++;
		*d++ = *s++; *d++ = *s++; *d++ = *s++; *d++ = *s++;
		*d++ = *s++; *d++ = *s++; *d++ = *s++; *d++ = *s++;
	}
	if (n & 8) {
		*d++ = *s++; *d++ = *s++; *d++ = *s++; *d++ = *s++;
		*d++ = *s++; *d++ = *s++; *d++ = *s++; *d++ = *s++;
	}
	if (n & 4) {
		*d++ = *s++; *d++ = *s++; *d++ = *s++; *d++ = *s++;
	}
	if (n & 2) {
		*d++ = *s++; *d++ = *s++;
	}
	if (n & 1) {
		*d = *s;
	}
	return dest;
}

void* memcpy(void* RESTRICT dest_, const void* RESTRICT src_, size_t n) {

	#define WORD_SIZE 32
	#define WORD_ALIGNMENT (WORD_SIZE - 1)

	uint8_t* dest = (uint8_t*)dest_;
	const uint8_t* src = (uint8_t*)src_;

	if (n <= WORD_SIZE) {		
#define _COPY_ONE(n)\
case n:\
{\
*dest++ = *src++;\
}
		switch (n) {
			_COPY_ONE(32)
			_COPY_ONE(31)
			_COPY_ONE(30)
			_COPY_ONE(29)
			_COPY_ONE(28)
			_COPY_ONE(27)
			_COPY_ONE(26)
			_COPY_ONE(25)
			_COPY_ONE(24)
			_COPY_ONE(23)
			_COPY_ONE(22)
			_COPY_ONE(21)
			_COPY_ONE(20)
			_COPY_ONE(19)
			_COPY_ONE(18)
			_COPY_ONE(17)
			_COPY_ONE(16)
			_COPY_ONE(15)
			_COPY_ONE(14)
			_COPY_ONE(13)
			_COPY_ONE(12)
			_COPY_ONE(11)
			_COPY_ONE(10)
			_COPY_ONE(9)
			_COPY_ONE(8)
			_COPY_ONE(7)
			_COPY_ONE(6)
			_COPY_ONE(5)
			_COPY_ONE(4)
			_COPY_ONE(3)
			_COPY_ONE(2)
			_COPY_ONE(1)
		default: break;
		}
		return dest_;
	}

	intptr_t dst_alignment = (intptr_t)(dest_) & WORD_ALIGNMENT;
	intptr_t src_alignment = (intptr_t)(src_) & WORD_ALIGNMENT;

	if (dst_alignment != src_alignment) {
		// for now, cop out...
		return musl_memcpy(dest_, src_, n);
	}

	while (dst_alignment--) {
		*dest++ = *src++;
	}

	__m256i* dest256 = (__m256i*)dest;
	const __m256i* src256 = (const __m256i*)src;
	n -= src_alignment;
	int runs = n / WORD_SIZE;
	int rem = n % WORD_SIZE;
	while (runs--) {
		*dest256++ = *src256++;
	}

	if (rem) {
		dest = (uint8_t*)dest256;
		src = (const uint8_t*)dest256;
		while (rem--) {
			*dest++ = *src++;
		}
	}
	
	return dest_;
}
