#ifndef _GR_DX_GRAPHICSEXCEPTION_H
#define _GR_DX_GRAPHICSEXCEPTION_H


#include <gr/GraphicsException.h>


BEGIN_NAMESPACE(gr) 


/** DirectX Graphics exception. */
class DX_GraphicsException :
	public GraphicsException
{
public:
	/** Creates an exception with the specified error description. */
	DX_GraphicsException( const NS(lang,Format)& msg )				: GraphicsException(msg) {}
};


END_NAMESPACE() // gr


#endif // _GR_DX_GRAPHICSEXCEPTION_H

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
