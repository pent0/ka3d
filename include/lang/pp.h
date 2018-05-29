#ifndef _LANG_PP
#define _LANG_PP


/*
* Idea of PLATFORM_xx definition is that there is single one defined / one platform.
* - #include<lang/pp_once.h>
* 		Make sure no more than one platform is defined.
* - #include<lang/pp_once_emu.h>
*   	Similar, but also defines EMULATOR_BUILD if build is for emulator (not target device)
* - Uncomment #pragma message() lines for debugging (or replace with #warning if your preprocessor does not support the pragma)
*/

// Setup
#undef PLATFORM_DEFINED
#undef EMULATOR_BUILD


/*************************************************************
 * Platforms with emulators.
 *************************************************************/
 
// PLATFORM_NGI
#if !defined(PLATFORM_NGI) && defined(__NGI)
//#pragma message("Defining PLATFORM_NGI")
#define PLATFORM_NGI
#include <lang/pp_once_emu.h>
#endif

// PLATFORM_S60_1X_2X
#if !defined(PLATFORM_S60_1X_2X) && defined(__SYMBIAN32__) && !defined(__NGI) && !defined(__SERIES60_3X__)
//#pragma message("Defining PLATFORM_S60_1X_2X")
#define PLATFORM_S60_1X_2X
#include <lang/pp_once_emu.h>
#endif

// PLATFORM_S60_3X
#if !defined(PLATFORM_S60_3X) && defined(__SERIES60_3X__) && !defined(__NGI)
//#pragma message("Defining PLATFORM_S60_3X")
#define PLATFORM_S60_3X
#include <lang/pp_once_emu.h>
#endif

// PLATFORM_BREW
#if !defined(PLATFORM_BREW) && ( defined(AEE_SIMULATOR) || defined(DYNAMIC_APP))
//#pragma message("Defining PLATFORM_BREW")
#define PLATFORM_BREW
#include <lang/pp_once_emu.h>
#endif


/*************************************************************
 * Platforms without emulators.
 *************************************************************/

// PLATFORM_WINCE
#if !defined(PLATFORM_WINCE) && defined(_WIN32_WCE)
//#pragma message("Defining PLATFORM_WINCE")
#define PLATFORM_WINCE
#include <lang/pp_once.h>
#endif

// PLATFORM_PS2
#if !defined(PLATFORM_PS2) && (defined(__ee__) && defined(__GCC__)) || defined(SN_TARGET_PS2)
//#pragma message("Defining PLATFORM_PS2")
#define PLATFORM_PS2
#include <lang/pp_once.h>
#endif

// PLATFORM_WIN32
#if !defined(PLATFORM_WIN32) && (defined(WIN32) || defined(_WIN32)) && !defined(_WIN32_WCE) && !defined(EMULATOR_BUILD)
//#pragma message("Defining PLATFORM_WIN32")
#define PLATFORM_WIN32
#include <lang/pp_once.h>
#endif


/*************************************************************
 * Cleanup
 *************************************************************/

// Check that a platform has been defined
// NOTE: If you want to force building for a specific target using compiler options,
// (e.g. gcc -DPLATFORM_WIN32) you also need to define PLATFORM_DEFINED to avoid this error.
#ifndef PLATFORM_DEFINED
#error No platform defined.
#endif

// Cleanup
#undef PLATFORM_DEFINED


/*************************************************************
 * Platform feature support
 *************************************************************/

#ifndef PLATFORM_BREW
#define PLATFORM_SUPPORTS_NAMESPACES
#define PLATFORM_SUPPORTS_RTTI
#endif

#if defined(PLATFORM_WIN32) || defined(PLATFORM_NGI)
#define PLATFORM_SUPPORTS_FINDFILE
#endif

#if defined(PLATFORM_S60_1X_2X) || defined(PLATFORM_S60_3X) || defined(PLATFORM_WINCE) || defined(PLATFORM_S60_3X) || defined(PLATFORM_BREW)
#define LANG_NOEXCEPTIONS
#else
#define LANG_EXCEPTIONS
#endif


/*************************************************************
 * Namespace support
 *************************************************************/

#ifdef PLATFORM_SUPPORTS_NAMESPACES
	#define BEGIN_NAMESPACE(X) namespace X {
	#define END_NAMESPACE() }
	#define USING_NAMESPACE(X) using namespace X;
	#define NS(X,Y) X::Y
#else
	#define BEGIN_NAMESPACE(X)
	#define END_NAMESPACE()
	#define USING_NAMESPACE(X)
	#define NS(X,Y) Y
#endif


#endif // _LANG_PP
