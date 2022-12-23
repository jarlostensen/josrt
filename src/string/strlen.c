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

// unsafe because it requires that s_ terminates within 16 readable bytes (or this will segfault)
size_t josrt_strlen_unsafe(const char* s_) {
    // ported to 128 bit from https://tsunanet.net/~tsuna/strlen.c.html (see below)
    
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
	static __m128i lo_magic __aligned(16);
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

// ===========================================================================================
/* Copyright (C) 1991, 1993, 1997, 2000, 2003 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Written by Torbjorn Granlund (tege@sics.se),
   with help from Dan Sahlin (dan@sics.se);
   commentary by Jim Blandy (jimb@ai.mit.edu).

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, write to the Free
   Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
   02111-1307 USA.  */

#include <string.h>
#include <stdlib.h>

//TODO: optimise
int strnlen(const char* s, size_t max_len) {
  int l = 0;
  while(*s++ && max_len--) ++l;
  return l;
}

#undef strlen

/* Return the length of the null-terminated string STR.  Scan for
   the null terminator quickly by testing four bytes at a time.  */
size_t
strlen (str)
     const char *str;
{
  const char *char_ptr;
  const unsigned long int *longword_ptr;
  unsigned long int longword, magic_bits, himagic, lomagic;

  /* Handle the first few characters by reading one character at a time.
     Do this until CHAR_PTR is aligned on a longword boundary.  */
  for (char_ptr = str; ((unsigned long int) char_ptr
                        & (sizeof (longword) - 1)) != 0;
       ++char_ptr)
    if (*char_ptr == '\0')
      return char_ptr - str;

  /* All these elucidatory comments refer to 4-byte longwords,
     but the theory applies equally well to 8-byte longwords.  */

  longword_ptr = (unsigned long int *) char_ptr;

  /* Bits 31, 24, 16, and 8 of this number are zero.  Call these bits
     the "holes."  Note that there is a hole just to the left of
     each byte, with an extra at the end:

     bits:  01111110 11111110 11111110 11111111
     bytes: AAAAAAAA BBBBBBBB CCCCCCCC DDDDDDDD

     The 1-bits make sure that carries propagate to the next 0-bit.
     The 0-bits provide holes for carries to fall into.  */
  magic_bits = 0x7efefeffL;
  himagic = 0x80808080L;
  lomagic = 0x01010101L;
  if (sizeof (longword) > 4)
    {
      /* 64-bit version of the magic.  */
      /* Do the shift in two steps to avoid a warning if long has 32 bits.  */
      magic_bits = ((0x7efefefeL << 16) << 16) | 0xfefefeffL;
      himagic = ((himagic << 16) << 16) | himagic;
      lomagic = ((lomagic << 16) << 16) | lomagic;
    }
  if (sizeof (longword) > 8)
    abort ();

  /* Instead of the traditional loop which tests each character,
     we will test a longword at a time.  The tricky part is testing
     if *any of the four* bytes in the longword in question are zero.  */
  for (;;)
    {
      /* We tentatively exit the loop if adding MAGIC_BITS to
         LONGWORD fails to change any of the hole bits of LONGWORD.

         1) Is this safe?  Will it catch all the zero bytes?
         Suppose there is a byte with all zeros.  Any carry bits
         propagating from its left will fall into the hole at its
         least significant bit and stop.  Since there will be no
         carry from its most significant bit, the LSB of the
         byte to the left will be unchanged, and the zero will be
         detected.

         2) Is this worthwhile?  Will it ignore everything except
         zero bytes?  Suppose every byte of LONGWORD has a bit set
         somewhere.  There will be a carry into bit 8.  If bit 8
         is set, this will carry into bit 16.  If bit 8 is clear,
         one of bits 9-15 must be set, so there will be a carry
         into bit 16.  Similarly, there will be a carry into bit
         24.  If one of bits 24-30 is set, there will be a carry
         into bit 31, so all of the hole bits will be changed.

         The one misfire occurs when bits 24-30 are clear and bit
         31 is set; in this case, the hole at bit 31 is not
         changed.  If we had access to the processor carry flag,
         we could close this loophole by putting the fourth hole
         at bit 32!

         So it ignores everything except 128's, when they're aligned
         properly.  */

      longword = *longword_ptr++;

      if (
#if 0
          /* Add MAGIC_BITS to LONGWORD.  */
          (((longword + magic_bits)

            /* Set those bits that were unchanged by the addition.  */
            ^ ~longword)

           /* Look at only the hole bits.  If any of the hole bits
              are unchanged, most likely one of the bytes was a
              zero.  */
           & ~magic_bits)
#else
          ((longword - lomagic) & himagic)
#endif
          != 0)
        {
          /* Which of the bytes was the zero?  If none of them were, it was
             a misfire; continue the search.  */

          const char *cp = (const char *) (longword_ptr - 1);

          if (cp[0] == 0)
            return cp - str;
          if (cp[1] == 0)
            return cp - str + 1;
          if (cp[2] == 0)
            return cp - str + 2;
          if (cp[3] == 0)
            return cp - str + 3;
          if (sizeof (longword) > 4)
            {
              if (cp[4] == 0)
                return cp - str + 4;
              if (cp[5] == 0)
                return cp - str + 5;
              if (cp[6] == 0)
                return cp - str + 6;
              if (cp[7] == 0)
                return cp - str + 7;
            }
        }
    }
}
//TODO: libc_hidden_builtin_def (strlen)

#endif