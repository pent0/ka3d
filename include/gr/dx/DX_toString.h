#ifndef _GR_DX_TOSTRING_H
#define _GR_DX_TOSTRING_H


BEGIN_NAMESPACE(gr) 
	

/** Returns render state name. */
const char*	toString( D3DRENDERSTATETYPE state );

/** Returns surface format name. */
const char*	toString( D3DFORMAT fmt );

/** Returns error code description. */
const char*	toString( HRESULT hr );

	
END_NAMESPACE() // gr


#endif // _GR_DX_TOSTRING_H

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
