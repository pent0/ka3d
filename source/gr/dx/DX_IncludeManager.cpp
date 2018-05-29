#include <gr/dx/DX_Common.h>
#include <gr/dx/DX_IncludeManager.h>
#include <io/PathName.h>
#include <io/FileInputStream.h>
#include <lang/Debug.h>
#include <lang/String.h>
#include <lang/Exception.h>
#include <config.h>


USING_NAMESPACE(io)
USING_NAMESPACE(lang)


BEGIN_NAMESPACE(gr) 


DX_IncludeManager::DX_IncludeManager( const PathName& path ) :
	m_path( path )
{
}

HRESULT DX_IncludeManager::Open( D3DXINCLUDE_TYPE /*includetype*/, LPCSTR filename, 
	LPCVOID /*parentdata*/, LPCVOID* data, UINT* bytes )
{
	void* src = 0;

	try
	{
		FileInputStream fin( PathName(m_path.toString(),filename).toString() );
		
		int size = fin.available();
		src = new char[size];
		if ( size != fin.read(src, size) )
			throwError( Exception( Format("Failed to load shader #include file {0}", String(filename)) ) );

		*data = src;
		*bytes = size;
	}
	catch ( Throwable& )
	{
		delete[] src;
		src = 0;
		Debug::printf( "ERROR: gr/dx: Failed to load shader #include file %s\n", filename );
		return E_FAIL;
	}
	return S_OK;
}

HRESULT DX_IncludeManager::Close( LPCVOID data )
{
	const char* src = reinterpret_cast<const char*>( data );
	delete[] src;
	return S_OK;
}


END_NAMESPACE() // gr

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
