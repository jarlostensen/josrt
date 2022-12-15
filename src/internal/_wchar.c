
#include <wchar.h>
#include "include/libc_internal.h"

size_t wcslen( const int *str )
{
	if( !str || str[0]==0 )
		return 0;

    size_t n = 0;
	while(*str!=0) 
	{
		++n;
		++str;
	}
	return n;
}
