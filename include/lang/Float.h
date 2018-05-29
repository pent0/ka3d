#ifndef _LANG_FLOAT_H
#define _LANG_FLOAT_H


#include <lang/pp.h>


BEGIN_NAMESPACE(lang) 


/** 
 * Float type information.
 * 
 * @ingroup lang
 */
class Float
{
public:
	/** 
	 * Maximum float value.
	 */
	static const float	MAX_VALUE;

	/** 
	 * Minimum float value. 
	 * This is smallest (positive) value that returns valid results when inverted.
	 */
	static const float	MIN_VALUE;
};


END_NAMESPACE() // lang


#endif // _LANG_FLOAT_H

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
