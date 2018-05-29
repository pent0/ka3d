#ifndef _LANG_SYSTEM_H
#define _LANG_SYSTEM_H


#include <lang/pp.h>


BEGIN_NAMESPACE(lang) 


/** 
 * System class provides running environment related information.
 * 
 * @ingroup lang
 */
class System
{
public:
	/** 
	 * Returns current system time in milliseconds. 
	 */
	static int currentTimeMillis();

private:
	System();
	System( const System& );
	System& operator=( const System& );
};


END_NAMESPACE() // lang


#endif // _LANG_SYSTEM_H

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
