#ifndef _LANG_SORT_H
#define _LANG_SORT_H


#include <lang/pp.h>


#if defined(PLATFORM_WINCE) || defined(PLATFORM_S60_1X_2X) || defined(PLATFORM_S60_3X) || defined(PLATFORM_NGI) || defined(PLATFORM_BREW)
#include <lang/algorithm/quicksort.h>
#define LANG_SORT NS(lang,quicksort)
#else
#include <algorithm>
#define LANG_SORT std::sort
#endif


#endif // _LANG_SORT_H
