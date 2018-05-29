// Include in pp.h after platform definition to ensure that only one platform is defined


#ifdef PLATFORM_DEFINED
	#error Attempting to define more than one platform.
#else
	#define PLATFORM_DEFINED
#endif
