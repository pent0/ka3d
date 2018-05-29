// Include in pp.h after platform definition to ensure that only one platform is defined
// Only use for platforms which have an emulator.
// Defines EMULATOR_BUILD if detected.
// Uncomment pragma message() for debugging (or replace with #warning if your preprocessor does not support the pragma)

// Verify that only one platform is defined
#include <lang/pp_once.h>

// Define EMULATOR_BUILD if detected
// TODO: Edit conditions as necessary
// Currently assumes all Windows builds to be for an emulator

#if ( defined(WIN32) || defined(_WIN32) || defined(AEE_SIMULATOR) )
//	#pragma message("Defined EMULATOR_BUILD")
	#define EMULATOR_BUILD
#endif

