#ifndef _GR_GRAPHICSEXCEPTION_H
#define _GR_GRAPHICSEXCEPTION_H


#include <lang/Exception.h>


BEGIN_NAMESPACE(gr) 


/** 
 * Graphics library exception. 
 * @ingroup gr
 */
class GraphicsException :
	public NS(lang,Exception)
{
public:
	/** Creates an exception with the specified error description. */
	GraphicsException( const NS(lang,Format)& msg )			: Exception(msg) {}
};


END_NAMESPACE() // gr


#endif // _GR_GRAPHICSEXCEPTION_H

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
