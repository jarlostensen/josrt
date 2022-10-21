
////////////////////////////////////////////////////////////////
// NOTE: this file is included by _vprint.c
// it contains code that is part macro unrolled and part hand rolled
// to generate wchar and char versions of various stdio string routines
//


#include "include/_vprint.h"

/* ***************************************************************
   NOTE: the wide versions of functions are NOT used at the moment
*/

static int _foutc(FILE* f, const char c) {
    return f->write(f, (const char*)(&c), 1);
}

static int _fout(FILE* f, const char* s, size_t l) {
    return f->write(f, s, l);
}

static int _fprintdecimal(FILE* f, long long d, int un_signed)
{
	int written = 0;
	if (!d)
	{
		return _foutc(f, TEXT('0'));
	}

	if (d < 0)
	{
		if (!un_signed)
		{
			int res = _foutc(f, TEXT('-'));
			if (res == EOF)
				return EOF;
			++written;
		}
		d *= -1;
	}
	// simple and dumb but it works...
	long long pow_10 = 1;
	long long dd = d;

	// find highest power of 10 
	while (dd > 9)
	{
		dd /= 10;
		pow_10 *= 10;
	}

	// print digits from MSD to LSD
	while (true)
	{
		int res = _foutc(f, TEXT('0') + (int)dd);
		if (res == EOF)
			return EOF;
		written+=res;
		d = d - (dd * pow_10);
		pow_10 /= 10;
		if (!pow_10)
			break;
		dd = d / pow_10;
	};
	return written;
}

static int _fprinthex(FILE* f, int width, long long d)
{
	static const CHAR* kHexDigits = TEXT("0123456789abcdef");
	int written = 0;

	if (d < 0)
	{
		d *= -1;
		//don't write a sign
	}
	if (width)
	{
		// calculate leading zeros required
		if (!d)
		{
			// all zeros; just output them
			written += width;
			while (width--)
			{
				int res = _foutc(f, TEXT('0'));
				if (res == EOF)
					return EOF;
			}
			return written;
		}

#if defined(_JOSRT_TOOLCHAIN_CLANG) || defined(_JOSRT_TOOLCHAIN_GCC)
		unsigned long d_width;
		(d_width = 63 - __builtin_clzll(d), ++d_width);
#else //TODO: check if it's WINDOWS and VS
		unsigned long d_width;
		(_BitScanReverse64(&d_width, d), ++d_width);
#endif
		d_width = (d_width / 4) + (d_width & 3 ? 1 : 0);
		if (width >= (int)d_width)
		{
			width -= d_width;
		}
	}

	if (d <= 256)
	{
		// write leading 0's
		written += width;
		while (width--)
		{
			int res = _foutc(f, TEXT('0'));
			if (res == EOF)
				return EOF;
		}

		if (d > 15)
		{
			int res = _foutc(f, kHexDigits[(d & 0xf0) >> 4]);
			if (res == EOF)
				return EOF;
			++written;
		}
		if (_foutc(f, kHexDigits[(d & 0xf)]) == EOF)
			return EOF;
		return written + 1;
	}

	// round padding down to nearest 2 since we always write pairs of digits below
	width &= ~1;
	written += width;
	while (width--)
	{
		if (_foutc(f, TEXT('0')) == EOF)
			return EOF;
	}

	int high_idx = 0;
	long long dd = d;
	while (dd > 15)
	{
		dd >>= 4;
		++high_idx;
	}
	// convert to byte offset
	high_idx >>= 1;
	// read from MSD to LSD
	const char* chars = (const char*)(&d) + high_idx;
	do
	{
		//NOTE: this will always "pad" the output to an even number of nybbles
		size_t lo = *chars & 0x0f;
		size_t hi = (*chars & 0xf0) >> 4;
		int res = _foutc(f, kHexDigits[hi]);
		res = res == EOF || _foutc(f, kHexDigits[lo]);
		if (res == EOF)
			return EOF;
		written += 2;
		--high_idx;
		--chars;
	} while (high_idx >= 0);

	return written;
}

static int _fprintbin(FILE* f, unsigned long long d)
{
	int written = 0;
	unsigned long long dd = d;
	unsigned long long bc = 0;
	while (dd)
	{
		dd >>= 1;
		++bc;
	}
	while (bc)
	{
		unsigned long dc = d & (1ull << (bc - 1));
		dc >>= (bc - 1);
		if (_foutc(f, TEXT('0' + (int)dc)) == EOF)
			return EOF;
		--bc;
		++written;
	}
	return written;
}

int _vfprint_impl(FILE* f, const CHAR* RESTRICT format, va_list parameters)
{
	int written = 0;

	while (*format != TEXT('\0')) {
		const size_t maxrem = INT_MAX - written;
		if (!maxrem) {
			// TODO: Set errno to EOVERFLOW.
			return -1;
		}
		if (format[0] != TEXT('%') || format[1] == TEXT('%'))
		{
			if (format[0] == TEXT('%'))
			{
				format++;
			}
			size_t amount = 1;
			while (format[amount] && format[amount] != TEXT('%'))
			{
				amount++;
			}
			if (maxrem < amount)
			{
				// TODO: Set errno to EOVERFLOW.
				return -1;
			}
			int result = _fout(f, format, amount);
			if (result == EOF)
			{
				return EOF;
			}
			format += (size_t)amount;
			written += result;
		}

		if (format[0])
		{
			const CHAR* format_begun_at = format++;

			//TODO: parsing, but ignoring
			char flag = 0;
			if (format[0] == TEXT('+') || format[0] == TEXT('-') || format[0] == TEXT(' ') || format[0] == TEXT('0') || format[0] == TEXT('#'))
			{
				flag = *format++;
			}
			(void)flag;

			// width modifier
			int width = 0;
			int width_digits = 0;
			int pow10 = 1;
			while (format[width_digits] >= TEXT('0') && format[width_digits] <= TEXT('9'))
			{
				++width_digits;
				pow10 *= 10;
			}
			if (width_digits)
			{
				pow10 /= 10;
				while (format[0] >= TEXT('0') && format[0] <= TEXT('9'))
				{
					width += (*format - TEXT('0')) * pow10;
					pow10 /= 10;
					++format;
				}
			}
			//TODO: parsing, but ignoring
			int precision = 0;
			if (format[0] == TEXT('.'))
			{
				++format;
				pow10 = 1;
				while (format[0] >= TEXT('0') && format[0] <= TEXT('9'))
				{
					precision += (*format - TEXT('0')) * pow10;
					pow10 *= 10;
					++format;
				}
			}

			//TODO: parsed, but only l is handled
			char length[2] = { 0,0 };
			if (format[0] == TEXT('h') || format[0] == TEXT('l') || format[0] == TEXT('L') || format[0] == TEXT('z') || format[0] == TEXT('j') || format[0] == TEXT('t'))
			{
				length[0] = *format++;
				if (format[0] == TEXT('h') || format[0] == TEXT('l'))
					length[1] = *format++;
			}

			CHAR type = *format++;

			switch (type)
			{
			case TEXT('c'):
			{
				CHAR c = (CHAR)va_arg(parameters, int);
				int result = _foutc(f, c);
				if (result == EOF)
					return EOF;
				written++;
			}
			break;
			case TEXT('s'):
			{
				const CHAR* str = va_arg(parameters, const CHAR*);
				//NOTE: this is *not* standard, supporting a width modifier for %s
				size_t len = width ? (size_t)width : STRLEN(str);
				if (maxrem < len)
				{
					// TODO: Set errno to EOVERFLOW.
					return EOF;
				}
				if (len)
				{
					int result = _fout(f, str, len);
					if (result == EOF)
					{
						// TODO: Set errno to EOVERFLOW.
						return EOF;
					}
					written += result;
				}
			}
			break;
			case TEXT('S'):
			{
				/* UNSUPPORTED widechar support: 
				const CHAR* str = va_arg(parameters, const CHAR*);
				//NOTE: this is *not* standard, supporting a width modifier for %s
				size_t len = width ? (size_t)width : STRLEN(str);
				if (maxrem < len)
				{
					// TODO: Set errno to EOVERFLOW.
					return EOF;
				}
				if (len)
				{
					int result = ctx->_print_convert(ctx->_that, str, len);
					if (result == EOF)
					{
						// TODO: Set errno to EOVERFLOW.
						return EOF;
					}
					written += result;
				}
				*/
				return EOF;
			}
			break;
			case TEXT('d'):
			case TEXT('i'):
			{
				int res;
				if (length[0] == TEXT('l'))
				{
					if (length[1] == TEXT('l'))
					{
						long long d = va_arg(parameters, long long);
						res = _fprintdecimal(f, d, 0);
					}
					else
					{
						long d = va_arg(parameters, long);
						res = _fprintdecimal(f, d, 0);
					}
				}
				else
				{
					int d = va_arg(parameters, int);
					res = _fprintdecimal(f, d, 0);
				}
				if (res == EOF)
					return EOF;
				written += res;
			}
			break;
			case TEXT('u'):
			{
				int res;
				if (length[0] == TEXT('l'))
				{
					if (length[1] == TEXT('l'))
					{
						unsigned long long d = va_arg(parameters, unsigned long long);
						res = _fprintdecimal(f, d, 1);
					}
					else
					{
						unsigned long d = va_arg(parameters, unsigned long);
						res = _fprintdecimal(f, d, 1);
					}
				}
				else
				{
					unsigned int d = va_arg(parameters, unsigned int);
					res = _fprintdecimal(f, d, 1);
				}
				if (res == EOF)
					return EOF;
				written += res;
			}
			break;
			case TEXT('x'):
			{
				int res;
				if (length[0] == TEXT('l'))
				{
					if (length[1] == TEXT('l'))
					{
						unsigned long long d = va_arg(parameters, unsigned long long);
						res = _fprinthex(f, width, d);
					}
					else
					{
						unsigned long d = va_arg(parameters, unsigned long);
						res = _fprinthex(f, width, d);
					}
				}
				else
				{
					unsigned int d = va_arg(parameters, unsigned int);
					res = _fprinthex(f, width, d);
				}
				if (res == EOF)
					return EOF;
				written += res;
			}
			break;
			// -----------------------------------------
			// NOT STANDARD
			case TEXT('b'):
			{
				int res;
				if (length[0] == TEXT('l'))
				{
					if (length[1] == TEXT('l'))
					{
						unsigned long long d = va_arg(parameters, unsigned long long);
						res = _fprintbin(f, d);
					}
					else
					{
						unsigned long d = va_arg(parameters, unsigned long);
						res = _fprintbin(f, d);
					}
				}
				else
				{
					unsigned int d = va_arg(parameters, unsigned int);
					res = _fprintbin(f, d);
				}
				if (res == EOF)
					return EOF;
				written += res;
			}
			break;
			// -----------------------------------------
			case TEXT('f'):
			{
				const float ff = (float)va_arg(parameters, double);
				int integral_part = (int)ff;
				float fractional_part = ff - (float)integral_part;
				int res = _fprintdecimal(f, (long long)integral_part, 0);
				if (res == EOF)
					return EOF;
				written += res;
				if (fractional_part != 0.0f && precision)
				{
					static CHAR c = TEXT('.');
					res = _foutc(f, c);
					if (res == EOF)
						return EOF;
					++written;
					int prec = precision;
					do
					{
						fractional_part *= 10.0f;
						res = _fprintdecimal(f, (long long)fractional_part, 0);
						if (res == EOF)
							return EOF;
						written += res;
						fractional_part -= (float)((int)fractional_part);
						//TODO: we need to use machine rounding mode for precision						
					} while (fractional_part != 0.0f && --prec);
				}
			}
			break;
			default:
			{
				format = format_begun_at;
				size_t len = STRLEN(format);
				if (maxrem < len) {
					// TODO: Set errno to EOVERFLOW.
					return EOF;
				}
				int res = _fout(f, format, len);
				if (res == EOF)
					return EOF;
				written += res;
				format += len;
			}
			break;
			}
		}
	}

	if (f->flush)
		f->flush(f);

	return written;
}

// ====================================================================================================================

#define _PRINTF_FILE_BUFFER_REM(f) (size_t)(((f)->_end) - (f)->_wp)

static int buffer_putchar_a(_printf_file_t* f, int c)
{
	const size_t rem_chars = _PRINTF_FILE_BUFFER_REM(f);
	if (rem_chars == 1)
	{
		return EOF;
	}
	*f->_wp++ = (char)c;
	return 1;
}

static int buffer_print_a(FILE* f_, const char* data, size_t length)
{
	_printf_file_t* f = (_printf_file_t*)f_;
	const size_t rem_chars = _PRINTF_FILE_BUFFER_REM(f);
	if (rem_chars < length + 1)
	{
		if (rem_chars == 1)
		{
			return EOF;
		}
		length = rem_chars - 1;
	}
	int written = 0;
	for (unsigned i = 0; i < length; ++i)
	{
		char wc = data[i];
		switch (wc)
		{
		case '\t':
		{
			if ((rem_chars-length) < 5)
			{
				// no more space
				return EOF;
			}
			// expand tabs to four spaces because It Is The Law
			static const char kTab[4] = {' ',' ',' ',' '};
			memcpy(f->_wp, kTab, sizeof(kTab));
			f->_wp += 4;
			written+=4;
		}
		break;
		//TODO: more of these, if we can be bothered...
#define _JOS_ESCAPED_CHAR_A(ec,c)\
		case ec:\
			if((rem_chars-length) < 2)\
			{\
				return EOF;\
			}\
			*f->_wp++ = c;\
			++written;\
			break
		_JOS_ESCAPED_CHAR_A('\"','"');
		default:
			*f->_wp++ = wc;
			++written;
			break;
		}
	}
	return written;
}

static int buffer_print_count_a(FILE* f, const char* data, size_t length)
{
	(void)f;
	(void)data;
	return (int)length;
}

#define SXPRINTF	snprintf
#define VSXPRINTF	vsnprintf

int VSXPRINTF(CHAR* RESTRICT buffer, size_t bufsz, const CHAR* RESTRICT format, va_list parameters)
{
	int written = _vfprint_impl((FILE*)(&(_printf_file_t) {
		._f._fd = -1,
		._f.write = bufsz ? buffer_print_a : buffer_print_count_a,
		._wp = buffer,
		._buffer = buffer,
		._end = buffer + bufsz
	}),
	format, parameters);
	buffer[written] = 0;
	return written;	
}

int SXPRINTF(CHAR* buffer, size_t bufsz, const CHAR* format, ...)
{
	if (!buffer || !format || !format[0])
		return 0;

	va_list parameters;
	va_start(parameters, format);
	int written = VSXPRINTF(buffer,bufsz,format,parameters);
	va_end(parameters);

	return written;
}
