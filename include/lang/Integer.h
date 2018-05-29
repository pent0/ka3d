#ifndef _LANG_INTEGER_H
#define _LANG_INTEGER_H


#include <lang/pp.h>


BEGIN_NAMESPACE(lang) 


/** 
 * Integer type information.
 * 
 * @ingroup lang
 */
class Integer
{
public:
	/** 
	 * Maximum signed integer value. 
	 */
	static const int		MAX_VALUE;

	/** 
	 * Minimum signed integer value. 
	 */
	static const int		MIN_VALUE;
};


END_NAMESPACE() // lang


#endif // _LANG_INTEGER_H

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
