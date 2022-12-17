
#include <stdint.h>
#include <string.h>
#include <extensions/hex_dump.h>

#ifndef min
#define min(a,b) ((a)<(b)?(a):(b))
#define _undef_min
#endif

#define CHARS_IN_BUFFER(buff) sizeof(buff)/sizeof(char)
//NOTE: explicitly for 64 bit
#define PRINT_WIDTH 66u

typedef struct _line_ctx
{
	char		_line[128];
	char*		_wp;
	size_t		_chars_left;
	void*		_mem;
	
} line_ctx_t;

static void _hex_dump_line_init(line_ctx_t* ctx, void* mem)
{
	ctx->_wp = ctx->_line;
	ctx->_mem = mem;
	ctx->_chars_left = CHARS_IN_BUFFER(ctx->_line);
	// address prefix
	size_t n = snprintf(ctx->_wp, ctx->_chars_left,"%016llx ", (uintptr_t)mem);
	ctx->_wp += n;
	ctx->_chars_left -= n;
}

static void _hex_dump_line_write_byte(line_ctx_t* ctx, unsigned char byte, int fmtIdx)
{
	static const char* kFmt[2] = {"%02x ", "%02x"};
	size_t n = snprintf(ctx->_wp, ctx->_chars_left, kFmt[fmtIdx], byte);
	ctx->_wp += n;
	ctx->_chars_left -= n;
}

static void _hex_dump_line_write_word(line_ctx_t* ctx, unsigned short word)
{
	size_t n = snprintf(ctx->_wp, ctx->_chars_left, "%04x ", word);
	ctx->_wp += n;
	ctx->_chars_left -= n;
}

static void _hex_dump_line_write_dword(line_ctx_t* ctx, unsigned int dword)
{
	size_t n = snprintf(ctx->_wp, ctx->_chars_left, "%08lx ", dword);
	ctx->_wp += n;
	ctx->_chars_left -= n;
}

static void _hex_dump_line_write_qword(line_ctx_t* ctx, unsigned long long qword)
{
	size_t n = snprintf(ctx->_wp, ctx->_chars_left, "%016llx ", qword);
	ctx->_wp += n;
	ctx->_chars_left -= n;
}

static void _hex_dump_line_write_raw(line_ctx_t* ctx, size_t byte_run)
{
	// right aligned
	for(unsigned i = 0u; i < ctx->_chars_left; ++i) {
		ctx->_wp[i] = L' ';
	}
	
	char* wp = ctx->_line + PRINT_WIDTH;
	ctx->_chars_left = CHARS_IN_BUFFER(ctx->_line) - PRINT_WIDTH;
	const char* rp = (char*)ctx->_mem;
	for (unsigned i = 0u; i < byte_run; ++i)
	{
		const unsigned short c = (unsigned short)(*rp++ & 0xff);
		size_t n;
		if(c >= 32 && c < 127)
		{
			n = snprintf(wp, ctx->_chars_left, "%c", c);
		}
		else
		{
			n = snprintf(wp, ctx->_chars_left, ".");
		}
		ctx->_chars_left -= n;
		wp += n;
	}
}

static size_t _hex_dump_hex_line(hex_dump_write_line_func_t write_line_func, void* mem, size_t bytes, enum hex_dump_unit_size unit_size)
{
	line_ctx_t ctx;
	size_t read = 0;	
	
	if (bytes)
	{				
		_hex_dump_line_init(&ctx, mem);		
		const unsigned byte_run = (const unsigned)(bytes< 16u ? bytes:16u);

		switch (unit_size)
		{
		case k8bitInt:
		{
			unsigned char* rp = (unsigned char*)mem;
			for (unsigned i = 0u; i < byte_run; ++i)
			{
				_hex_dump_line_write_byte(&ctx, *rp++,0);
				++read;
			}
		}
		break;
		case k16bitInt:
		{
			unsigned short* rp = (unsigned short*)mem;
			unsigned run = min(byte_run/2, 8);
			for (unsigned i = 0u; i < run; ++i)
			{
				_hex_dump_line_write_word(&ctx, *rp++);
				read+=sizeof(unsigned short);
			}
			if ( byte_run & 1 )
			{
				run = byte_run & 1;
				for (unsigned i = 0u; i < run; ++i)
				{
					_hex_dump_line_write_byte(&ctx, (unsigned char)*rp++, 1);
					++read;
				}
			}
		}
		break;
		case k32bitInt:
		{
			unsigned int* rp = (unsigned int*)mem;
			unsigned run = min(byte_run/4, 4);
			for (unsigned i = 0u; i < run; ++i)
			{
				_hex_dump_line_write_dword(&ctx, *rp++);
				read+=sizeof(unsigned int);
			}
			if ( byte_run & 3 )
			{
				run = byte_run & 3;
				for (unsigned i = 0u; i < run; ++i)
				{
					_hex_dump_line_write_byte(&ctx, (unsigned char)*rp++, 1);
					++read;
				}
			}
		}
		break;
		case k64bitInt:
		{
			unsigned long long* rp = (unsigned long long*)mem;
			unsigned run = min(byte_run/8, 2);
			for (unsigned i = 0u; i < run; ++i)
			{
				_hex_dump_line_write_qword(&ctx, *rp++);
				read+=sizeof(unsigned long long);
			}
			if ( byte_run & 7 )
			{
				run = byte_run & 7;
				for (unsigned i = 0u; i < run; ++i)
				{
					_hex_dump_line_write_byte(&ctx, (unsigned char)*rp++, 1);
					++read;
				}
			}
		}
		break;
		default:;
		}

		_hex_dump_line_write_raw(&ctx, byte_run);
	}
	
	if(read)
	{
		write_line_func(ctx._line);
	}
	return read;
}

void hex_dump_mem(hex_dump_write_line_func_t write_line_func, const void* mem, size_t bytes, enum hex_dump_unit_size unit_size)
{	
	while (bytes)
	{
		size_t written = _hex_dump_hex_line(write_line_func, mem, bytes, unit_size);		
		mem = (void*)((char*)mem + written);
		bytes -= written;
	}
}

#ifdef _undef_min
#undef min
#endif
