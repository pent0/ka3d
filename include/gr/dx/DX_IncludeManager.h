#ifndef _DX_INCLUDEMANAGER_H
#define _DX_INCLUDEMANAGER_H


#include <io/PathName.h>
#include <lang/String.h>


BEGIN_NAMESPACE(gr) 


/**
 * Class for loading shader #include files.
 */
class DX_IncludeManager :
	public ID3DXInclude
{
public:
	DX_IncludeManager( const NS(io,PathName)& path );

    STDMETHOD(Open)( D3DXINCLUDE_TYPE includetype, LPCSTR filename, LPCVOID parentdata, LPCVOID* data, UINT* bytes );
    STDMETHOD(Close)( LPCVOID data );

private:
	NS(io,PathName) m_path;
};


END_NAMESPACE() // gr


#endif // _DX_INCLUDEMANAGER_H

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
