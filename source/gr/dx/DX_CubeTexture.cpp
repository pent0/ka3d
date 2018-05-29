#include <gr/dx/DX_common.h>
#include <gr/dx/DX_CubeTexture.h>
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


DX_CubeTexture::DX_CubeTexture( DX_Context* context, const String& filename ) :
	DX_ContextItem( context, CLASSID_CUBETEXTURE ),
	m_tex( 0 ),
	m_filename( filename ),
	m_format( SurfaceFormat::SURFACE_UNKNOWN )
{
	create( filename );
}

void DX_CubeTexture::create( const String& filename )
{
	assert( !m_tex );

	DX_Context* context = m_context;
	IDirect3DDevice9* dev = context->device();
	Array<uint8_t> data;
	io::FileInputStream in( filename );
	data.resize( in.available() );
	in.read( data.begin(), data.size() );

	HRESULT hr = DX_TRY( D3DXCreateCubeTextureFromFileInMemory(dev,data.begin(),data.size(),&m_tex) );
	if ( D3D_OK != hr )
	{
		// try to create cube texture from 2D texture
		IDirect3DTexture9* tex = 0;
		hr = DX_TRY( D3DXCreateTextureFromFileInMemoryEx( dev, data.begin(), data.size(),
			D3DX_DEFAULT, D3DX_DEFAULT, 0, 0,
			D3DFMT_UNKNOWN, D3DPOOL_SYSTEMMEM, D3DX_DEFAULT, D3DX_DEFAULT, 0, 0, 0,
			&tex ) );

		if ( hr == D3D_OK )
		{
			D3DSURFACE_DESC desc;
			tex->GetLevelDesc( 0, &desc );
			
			hr = D3DXCreateCubeTexture( dev, desc.Width, D3DX_DEFAULT, 0, desc.Format, D3DPOOL_MANAGED, &m_tex );
			if ( hr == D3D_OK )
			{
				IDirect3DSurface9* sf0 = 0;
				hr = tex->GetSurfaceLevel( 0, &sf0 );
				if ( hr == D3D_OK )
				{
					for ( int k = 0 ; k < (int)m_tex->GetLevelCount() ; ++k )
					{
						for ( int i = 0 ; i < 6 ; ++i )
						{
							IDirect3DSurface9* sf1 = 0;
							hr = m_tex->GetCubeMapSurface( D3DCUBEMAP_FACES(i), k, &sf1 );
							if ( hr != D3D_OK )
								break;

							hr = D3DXLoadSurfaceFromSurface( sf1, 0, 0, sf0, 0, 0,D3DX_DEFAULT, 0 );
							DX_release( sf1 );
							if ( hr != D3D_OK )
								break;
						}
					}
					DX_release( sf0 );
				}
			}
			DX_release( tex );
		}

		if ( hr != D3D_OK )
		{
			DX_release( m_tex );
			throwError( DX_GraphicsException( Format("Failed to create texture ({0}): {1}", filename, gr::toString(hr)) ) );
		}
	}

	validateDesc( filename, 0 );
}

DX_CubeTexture::~DX_CubeTexture()
{
	DX_release( m_tex );
}

void DX_CubeTexture::validateDesc( const String& filename, int usageflags )
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
		throwError( DX_GraphicsException( Format("Failed to store cube texture \"{0}\" creation parameters (w={1}, h={2}, fmt={3}, usage=0x{4,x}): {5}", filename, (int)desc.Width, (int)desc.Height, gr::toString(desc.Format), usageflags, gr::toString(hr)) ) );
	}
}

void DX_CubeTexture::deviceLost()
{
	DX_release( m_tex );
}

void DX_CubeTexture::deviceReset()
{
	assert( m_filename != "" );
	create( m_filename );
}

int DX_CubeTexture::width() const
{
	return m_width;
}

int DX_CubeTexture::height() const
{
	return m_height;
}

SurfaceFormat DX_CubeTexture::format() const
{
	return m_format;
}

String DX_CubeTexture::toString() const
{
	if ( m_filename != "" )
		return Format( "CubeTexture( \"{0}\" )", m_filename ).format();
	else
		return Format( "CubeTexture( w={0}, h={1}, fmt={2}, usage=0x{3,x} )", this->width(), this->height(), this->format().toString(), m_usageflags ).format();
}


END_NAMESPACE() // gr

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
