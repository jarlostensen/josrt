#include <limits.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <stdarg.h>
#include <stdio.h>

#include "include/_vprint.h"

//ZZZ: this include file issue needs tidying!
#define _JOS_ASSERT(cond)

/* NOTE: WIDECHAR support is disagbled for now as it requires extensive file support and is of limited value to my projects. 
// WIDECHAR is defined: this will build the wide implementation
#define WIDECHAR
#include "_vprint.inc.c"
*/

// implements narrow versions
#undef WIDECHAR
#include "_vprint.inc.c"

