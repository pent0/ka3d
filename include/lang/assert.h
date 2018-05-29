// we'll allow this file to be included multiple times
// since assert() might be re-defined in many places
#include <lang/pp.h>
#include <lang/internalError.h>

// Workaround for NGI SDK 1.0, which does not define NULL in assert.h, but references it
#if defined(PLATFORM_NGI) && !defined(NULL)
#include <sys/_null.h>
#endif

// by default, we'll be using CRT assert
#include <assert.h>

// check if non-debug assert is requested
#ifdef NDEBUG_ASSERT
#undef assert
#define assert(E) if (E) {} else {NS(lang,internalError)(__FILE__,__LINE__,#E);}
#endif // NDEBUG_ASSERT
