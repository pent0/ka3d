#include <gr/dx/DX_common.h>
#include <gr/dx/DX_Texture.h>
#include <gr/dx/DX_Context.h>
#include <gr/dx/DX_SurfaceFormatUtil.h>
#include <gr/dx/DX_GraphicsException.h>
#include <gr/dx/DX_helpers.h>
#include <gr/dx/DX_toString.h>
#include <io/FileInputStream.h>
#include <stdint.h>
#include <config.h>


USING_NAMESPACE(io)
USING_NAMESPACE(lang)


BEGIN_NAMESPACE(gr) 


static VOID WINAPI fillZero(
	D3DXVECTOR4* pOut, 
	CONST D3DXVECTOR2* /*pTexCoord*/, 
	CONST D3DXVECTOR2* /*pTexelSize*/, 
	LPVOID /*pData*/ )
{
	pOut->x = 0.f;
	pOut->y = 0.f;
	pOut->z = 0.f;
	pOut->w = 0.f;
}

	
DX_Texture::DX_Texture( DX_Context* context, int width, int height, 
	const SurfaceFormat& fmt, int usageflags ) :
	DX_ContextItem( context, CLASSID_TEXTURE ),
	m_tex( 0 ),
	m_filename(),
	m_width( 0 ),
	m_height( 0 ),
	m_usageflags( 0 ),
	m_format( SurfaceFormat::SURFACE_UNKNOWN ),
	m_locked( LOCK_NONE )
{
	create( width, height, fmt, usageflags );
}

DX_Texture::DX_Texture( DX_Context* context, const String& filename ) :
	DX_ContextItem( context, CLASSID_TEXTURE ),
	m_tex( 0 ),
	m_filename( filename ),
	m_width( 0 ),
	m_height( 0 ),
	m_usageflags( 0 ),
	m_format( SurfaceFormat::SURFACE_UNKNOWN ),
	m_locked( LOCK_NONE )
{
	create( filename );
}

void DX_Texture::create( int width, int height, 
	const SurfaceFormat& fmt, int usageflags )
{
	assert( !m_tex );

	DX_Context* context = m_context;
	IDirect3DDevice9* dev = context->device();
	UINT miplevels = D3DX_DEFAULT;
	D3DPOOL pool = D3DPOOL_MANAGED;

	D3DFORMAT d3dfmt = toD3D( fmt );
	if ( d3dfmt == D3DFMT_UNKNOWN )
		throwError( DX_GraphicsException( Format("Failed to create texture {0}x{1}, surface format {2} unsupported", width, height, fmt.toString()) ) );

	DWORD d3dusage = 0;
	if ( usageflags & Context::USAGE_RENDERTARGET )
	{
		d3dusage |= D3DUSAGE_RENDERTARGET;
		miplevels = 1;
		pool = D3DPOOL_DEFAULT;
	}

	HRESULT hr = DX_TRY( D3DXCreateTexture(dev,width,height,miplevels,d3dusage,d3dfmt,pool,&m_tex) );
	if ( D3D_OK != hr )
		throwError( DX_GraphicsException( Format("Failed to create texture (w={0}, h={1}, fmt={2}, usage=0x{3,x}): {4}", width, height, fmt.toString(), usageflags, gr::toString(hr)) ) );

	validateDesc( "", usageflags );

	clear();
}

void DX_Texture::create( const String& filename )
{
	assert( !m_tex );

	DX_Context* context = m_context;
	IDirect3DDevice9* dev = context->device();
	Array<uint8_t> data;
	io::FileInputStream in( filename );
	data.resize( in.available() );
	in.read( data.begin(), data.size() );

	HRESULT hr = DX_TRY( D3DXCreateTextureFromFileInMemory(dev,data.begin(),data.size(),&m_tex) );
	if ( D3D_OK != hr )
		throwError( DX_GraphicsException( Format("Failed to create texture ({0}): {1}", filename, gr::toString(hr)) ) );

	validateDesc( filename, 0 );
}

DX_Texture::~DX_Texture()
{
	DX_release( m_tex );
}

void DX_Texture::validateDesc( const String& filename, int usageflags )
{
	D3DSURFACE_DESC desc;
	HRESULT hr = DX_TRY( m_tex->GetLevelDesc(0,&desc) );

	m_filename = filename;
	m_width = (uint16_t)desc.Width;
	m_height = (uint16_t)desc.Height;
	m_usageflags = (uint8_t)usageflags;
	m_format = toSurfaceFormat( desc.Format );

	if ( m_usageflags != usageflags || 
		m_format == SurfaceFormat::SURFACE_UNKNOWN || 
		m_width != desc.Width || 
		m_height != desc.Height )
	{
		DX_release( m_tex );
		throwError( DX_GraphicsException( Format("Failed to store texture \"{0}\" creation parameters (w={1}, h={2}, fmt={3}, usage=0x{4,x}): {5}", filename, (int)desc.Width, (int)desc.Height, gr::toString(desc.Format), usageflags, gr::toString(hr)) ) );
	}

	m_context->statistics.allocatedTextureMemory += m_format.getMemoryUsage(m_width,m_height);
	m_context->statistics.allocatedTextures += 1;
}

void DX_Texture::deviceLost()
{
	DX_release( m_tex );
}

void DX_Texture::deviceReset()
{
	if ( m_filename != "" )
		create( m_filename );
	else
		create( m_width, m_height, m_format, m_usageflags );
}

void DX_Texture::blt( int x, int y,
	const void* data, int pitch, int w, int h, const SurfaceFormat& fmt, 
	const void* pal, const SurfaceFormat& palfmt )
{
	D3DFORMAT d3dfmt = toD3D( fmt );
	DWORD filter = D3DX_FILTER_LINEAR;

	PALETTEENTRY d3dpal[256];
	if ( pal )
		SurfaceFormat(SurfaceFormat::SURFACE_A8B8G8R8).copyPixels( d3dpal, SurfaceFormat(), 0, palfmt, pal, SurfaceFormat(), 0, fmt.paletteEntries() );

	RECT rect;
	rect.left = 0;
	rect.top = 0;
	rect.right = w;
	rect.bottom = h;

	D3DSURFACE_DESC desc0 = D3DSURFACE_DESC();
	int miplevels = m_tex->GetLevelCount();
	for ( int i = 0 ; i < miplevels ; ++i )
	{
		IDirect3DSurface9* surf = 0;
		HRESULT hr = DX_TRY( m_tex->GetSurfaceLevel(i, &surf) );
		if ( hr == D3D_OK )
		{
			D3DSURFACE_DESC desc;
			surf->GetDesc( &desc );
			if ( 0 == i )
				desc0 = desc;

			RECT dstrect;
			dstrect.left = (x * desc.Width) / desc0.Width;
			dstrect.top = (y * desc.Height) / desc0.Height;
			dstrect.right = ((x+w) * desc.Width) / desc0.Width;
			dstrect.bottom = ((y+h) * desc.Height) / desc0.Height;
			bool ok = ( int(dstrect.bottom-dstrect.top) >= 1 && int(dstrect.right-dstrect.left) >= 1 );

			if ( ok )
			{
				DX_TRY( D3DXLoadSurfaceFromMemory(surf,0,&dstrect,data,d3dfmt,pitch,pal?d3dpal:0,&rect,filter,0xFF000000) );
			}

			DX_release( surf );
		}
	}
}

int DX_Texture::width() const
{
	return m_width;
}

int DX_Texture::height() const
{
	return m_height;
}

Rect DX_Texture::rect() const
{
	return Rect( m_width, m_height );
}

SurfaceFormat DX_Texture::format() const
{
	return m_format;
}

String DX_Texture::toString() const
{
	if ( m_filename != "" )
		return Format( "Texture( \"{0}\" )", m_filename ).format();
	else
		return Format( "Texture( w={0}, h={1}, fmt={2}, usage=0x{3,x} )", this->width(), this->height(), this->format().toString(), m_usageflags ).format();
}

void DX_Texture::clear()
{
	assert( m_tex != 0 );
	DX_TRY( D3DXFillTexture(m_tex, fillZero, 0) );
}

void DX_Texture::lock( LockType lock )
{
	assert( m_tex != 0 );

	int flags = 0;
	if ( lock == LOCK_READ )
		flags = D3DLOCK_READONLY;
	else if ( lock == LOCK_WRITE )
		flags = D3DLOCK_DISCARD;

	HRESULT hr = DX_TRY( m_tex->LockRect( 0, &m_lockedRect, 0, flags ) );
	if ( D3D_OK == hr )
		m_locked = lock;
}

void DX_Texture::unlock()
{
	assert( m_tex != 0 );

	if ( m_locked != LOCK_NONE )
	{
		DX_TRY( m_tex->UnlockRect(0) );
	}

	m_locked = LOCK_NONE;
}

DX_Texture::LockType DX_Texture::locked() const
{
	return m_locked;
}

void DX_Texture::getData( void** bits, int* pitch ) const
{
	assert( locked() != LOCK_NONE );

	*bits = m_lockedRect.pBits;
	*pitch = m_lockedRect.Pitch;
}


END_NAMESPACE() // gr

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
