#include <limits.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <stdarg.h>
#include <stdio.h>

#include "include/_vprint.h"

// implements narrow versions
#undef WIDECHAR
#include "_vprint.inc.c"

