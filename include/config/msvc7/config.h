#if (_MSC_VER<1310)
#error Tried to include msvc7/config.h without using MSVC7
#endif

// not much to configure, make sure some macros haven't been defined
#undef PI
#undef max
#undef min
#undef abs

// use debug memory allocation routines to find leaks and invalid memory usage
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)

#include <lang/assert.h>

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
