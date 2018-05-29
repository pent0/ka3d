#include <gr/dx/DX_common.h>
#include <gr/dx/DX_Shader.h>
#include <gr/dx/DX_Context.h>
#include <gr/dx/DX_Texture.h>
#include <gr/dx/DX_Primitive.h>
#include <gr/dx/DX_CubeTexture.h>
#include <gr/dx/DX_GraphicsException.h>
#include <gr/dx/DX_SurfaceFormatUtil.h>
#include <gr/dx/DX_toString.h>
#include <gr/dx/DX_helpers.h>
#include <gr/dx/zero.h>
#include <io/PathName.h>
#include <lang/Debug.h>
#include <math/float4x4.h>
#include <string.h>
#include <stdio.h>
#include <config.h>


#pragma comment( lib, "d3d9.lib" )
#pragma comment( lib, "dxguid.lib" )
#ifdef _DEBUG
#pragma comment( lib, "d3dx9d.lib" )
#else
#pragma comment( lib, "d3dx9.lib" )
#endif


USING_NAMESPACE(lang)
USING_NAMESPACE(math)


BEGIN_NAMESPACE(gr) 


/** Allowed display mode pixel formats. */
static const D3DFORMAT DISPLAY_FORMATS[] =
{
	D3DFMT_A8R8G8B8, 
	D3DFMT_X8R8G8B8,
	D3DFMT_A2B10G10R10, 
	D3DFMT_A1R5G5B5, 
	D3DFMT_X1R5G5B5, 
	D3DFMT_R5G6B5
};

/** Allowed display mode depth buffer formats. */
static const D3DFORMAT DEPTH_FORMATS[] =
{
	D3DFMT_D32,
	D3DFMT_D24S8,
	D3DFMT_D16,
	D3DFMT_D24X8,
	D3DFMT_D24X4S4,
	D3DFMT_D16,
	D3DFMT_D15S1,
	D3DFORMAT(-1)
};

/** Allowed display mode depth/stencil buffer formats. */
static const D3DFORMAT DEPTH_STENCIL_FORMATS[] =
{
	D3DFMT_D24S8,
	D3DFMT_D24X4S4, 
	D3DFMT_D15S1,
	D3DFORMAT(-1)
};

	
static D3DDEVTYPE toD3D( DX_Context::RasterizerType rz )
{	
	switch ( rz )
	{
	case DX_Context::RASTERIZER_SW:		return D3DDEVTYPE_REF;
	case DX_Context::RASTERIZER_HW:
	default:							return D3DDEVTYPE_HAL;
	}
}

static void getClientSize( HWND hwnd, int* w, int* h )
{
	if ( !hwnd )
		hwnd = GetActiveWindow();

	RECT cr = {0,0,0,0};
	if ( hwnd )
		GetClientRect( hwnd, &cr );

	*w = (int)cr.right;
	*h = (int)cr.bottom;
}

static D3DFORMAT findDepthBuffer( const D3DFORMAT* depthformats, IDirect3D9* d3d, D3DFORMAT displaymode, D3DDEVTYPE devtype )
{
	for ( int i = 0 ; D3DFORMAT(-1) != depthformats[i] ; ++i )
	{
		HRESULT hr = d3d->CheckDeviceFormat( 
			D3DADAPTER_DEFAULT, devtype,
			displaymode,
			D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE,
			depthformats[i] );

		if ( D3D_OK == hr )
			return depthformats[i];
	}
	return D3DFMT_UNKNOWN;
}


DX_Context::DX_Context( int width, int height, int bitsperpixel,
	WindowType win, RasterizerType rz, VertexProcessingType vp, int buffers ) :
	shaderparamname(),
	itemlist(),
	boneworldtm(),
	projtm( 1.f ),
	worldtm( 1.f ),
	camerapos( 0, 0, 0, 0 ),
	sortbuffer(),
	m_d3d( 0 ),
	m_device( 0 ),
	m_buffers( 0 ),
	m_renderwidth( 0 ),
	m_renderheight( 0 ),
	m_capturenum( 0 ),
	m_sort( Shader::SORT_NONE ),
	m_debugshaders( false ),
	m_lockablebuffer( true )
{
	statistics.reset();

	Error err = init( width, height, bitsperpixel, win, rz, vp, buffers );

	if ( ERROR_NONE != err )
	{
		// make device description string
		char desc[512] = {0};
		sprintf( desc+strlen(desc), "(width=%d, height=%d, bits=%d, win=%d, rz=%d, vp=%d, buf=%d)",
			width, height, bitsperpixel, (int)win, (int)rz, (int)vp, buffers );

		deinit();
		error( err, desc );
	}
}

DX_Context::~DX_Context()
{
	deinit();
}

void DX_Context::setPerspectiveProjection( float hfov, float front, float back, float aspect )
{
	projtm.setPerspectiveProjection( hfov, front, back, aspect );
}

void DX_Context::setOrthographicProjection( bool enabled )
{
	if ( enabled )
		projtm = float4x4(1.f);
}

bool DX_Context::ready()
{
	HRESULT hr = DX_TRY( m_device->TestCooperativeLevel() );
	if ( D3D_OK != hr )
	{
		if ( D3DERR_DEVICENOTRESET == hr )
		{
			for ( DX_ContextItem* it = itemlist.last() ; it != 0 ; it = it->previous() )
			{
				it->deviceLost();
			}

			hr = DX_TRY( m_device->Reset(&m_present) );

			for ( DX_ContextItem* it = itemlist.last() ; it != 0 ; it = it->previous() )
			{
				it->deviceReset();
			}
		}
	}
	return D3D_OK == hr;
}

void DX_Context::clear( int color )
{
	DWORD clearflags = D3DCLEAR_TARGET;
	if ( SURFACE_DEPTH & m_buffers )
		clearflags |= D3DCLEAR_ZBUFFER;
	if ( SURFACE_STENCIL & m_buffers )
		clearflags |= D3DCLEAR_STENCIL;

	DX_TRY( m_device->Clear( 0, 0, clearflags, color, 1.f, 0 ) );
}

void DX_Context::present( int color )
{
	// swap back buffer to screen
	HRESULT hr = DX_TRY( m_device->Present( 0, 0, 0, 0 ) );
	
	// clear back buffer
	if ( D3D_OK == hr )
		clear( color );

	statistics.renderedFrames++;
}

void DX_Context::setViewport( const Rect& rect )
{
	D3DVIEWPORT9 vp;
	vp.Width = rect.width();
	vp.Height = rect.height();
	vp.MaxZ = 1.f;
	vp.MinZ = 0.f;
	vp.X = rect.left();
	vp.Y = rect.top();
	DX_TRY( m_device->SetViewport(&vp) );
}

void DX_Context::setRenderTarget( Texture* dst )
{
	IDirect3DSurface9* surface = 0;
	if ( dst != 0 )
	{
		DX_Texture* dsttex = static_cast<DX_Texture*>( dst );
		if ( D3D_OK != DX_TRY(dsttex->texture()->GetSurfaceLevel(0, &surface)) )
			return;
	}
	else
	{
		if ( D3D_OK != DX_TRY(m_device->GetBackBuffer(0,0,D3DBACKBUFFER_TYPE_MONO,&surface)) )
			return;
	}

	if ( D3D_OK == DX_TRY( m_device->SetRenderTarget(0,surface) ) )
	{
		D3DSURFACE_DESC desc;
		surface->GetDesc( &desc );
		setViewport( Rect(desc.Width,desc.Height) );
	}

	DX_release( surface );
}

/*void DX_Context::stretchRect( const Rect& srcrect, Texture* dst, const Rect& dstrect,
	FilterType filter )
{
	RECT srcrc;
	srcrc.left = srcrect.left();
	srcrc.top = srcrect.top();
	srcrc.bottom = srcrect.bottom();
	srcrc.right = srcrect.right();

	RECT dstrc;
	dstrc.left = dstrect.left();
	dstrc.top = dstrect.top();
	dstrc.bottom = dstrect.bottom();
	dstrc.right = dstrect.right();

	D3DTEXTUREFILTERTYPE d3dfilter = D3DTEXF_NONE;
	switch ( filter )
	{
	case FILTER_POINT:
		if ( dstrect.width() < srcrect.width() && 0 != (m_caps.StretchRectFilterCaps & D3DPTFILTERCAPS_MINFPOINT) )
			d3dfilter = D3DTEXF_POINT;
		else if ( dstrect.width() > srcrect.width() && 0 != (m_caps.StretchRectFilterCaps & D3DPTFILTERCAPS_MAGFPOINT) )
			d3dfilter = D3DTEXF_POINT;
		break;
	
	case FILTER_LINEAR:
		if ( dstrect.width() < srcrect.width() && 0 != (m_caps.StretchRectFilterCaps & D3DPTFILTERCAPS_MINFLINEAR) )
			d3dfilter = D3DTEXF_LINEAR;
		else if ( dstrect.width() > srcrect.width() && 0 != (m_caps.StretchRectFilterCaps & D3DPTFILTERCAPS_MAGFLINEAR) )
			d3dfilter = D3DTEXF_LINEAR;
		break;
	}

	DX_Texture* dsttex = static_cast<DX_Texture*>( dst );
	IDirect3DSurface9* surface = 0;
	if ( D3D_OK != DX_TRY(dsttex->texture()->GetSurfaceLevel(0, &surface)) )
		return;
	IDirect3DSurface9* backbuffer = 0;
	if ( D3D_OK != DX_TRY(m_device->GetBackBuffer(0,0,D3DBACKBUFFER_TYPE_MONO,&backbuffer)) )
		return;

	DX_TRY( m_device->StretchRect(backbuffer,&srcrc,surface,&dstrc,d3dfilter) );

	DX_release( surface );
	DX_release( backbuffer );
}*/

void DX_Context::deinit()
{
	// release cached objects
	m_dynamicPrimitives.clear();
	m_shaders.clear();

	// warn about existing objects
	for ( DX_ContextItem* it = itemlist.last() ; it != 0 ; it = it->previous() )
		Debug::printf( "ERROR: gr/fx: Context object %s exists at deinit\n", it->toString().c_str() );

	// release d3d device
	Debug::printf( "gr/dx: Releasing Direct3DDevice9 object\n" );
	DX_release( m_device );
	
	// release d3d object
	if ( m_d3d != 0 && m_d3d->Release() != 0 )
	{
		_CrtDbgBreak(); // not all objects released
	}
	m_d3d = 0;
}

DX_Context::Error DX_Context::init( int width, int height, int bitsperpixel,
	WindowType win, RasterizerType rz, VertexProcessingType vp, int buffers )
{
	// build Shader::ParamType String array (to gain advantage of refcounting)
	shaderparamname.resize( Shader::PARAM_COUNT );
	for ( int i = 0 ; i < Shader::PARAM_COUNT ; ++i )
		shaderparamname[i] = Shader::toString( (Shader::ParamType)i );

	// shader debugging (MSVC7) requires ref rasterizer and sw vshader
	if ( debugShaders() )
	{
		rz = RASTERIZER_SW;
		vp = VERTEXP_SW;
	}

	// check that we have active window
	HWND hwnd = GetActiveWindow();
	D3DDEVTYPE devtype = toD3D( rz );
	if ( !hwnd )
		return ERROR_NOACTIVEWINDOW;

	// create Direct3D9 object
	m_d3d = Direct3DCreate9( D3D_SDK_VERSION );
	if ( !m_d3d )
		return ERROR_DIRECTXNOTINSTALLED;
	m_d3d->GetAdapterDisplayMode( D3DADAPTER_DEFAULT, &m_displaymode );

	// find matching display mode
	if ( WINDOW_FULLSCREEN == win )
	{
		const int ALLOWED_DISPLAY_MODE_COUNT = sizeof(DISPLAY_FORMATS)/sizeof(DISPLAY_FORMATS[0]);
		bool modefound = false;

		for ( int k = 0 ; k < ALLOWED_DISPLAY_MODE_COUNT && !modefound ; ++k )
		{
			HRESULT hr = D3D_OK;
			for ( int i = 0 ; D3D_OK == hr ; ++i )
			{
				hr = DX_TRY( m_d3d->EnumAdapterModes( D3DADAPTER_DEFAULT, DISPLAY_FORMATS[k], i, &m_displaymode ) );
				if ( hr == D3D_OK )
				{
					SurfaceFormat pixfmt = toSurfaceFormat( m_displaymode.Format );

					if ( (int)m_displaymode.Width == width && 
						(int)m_displaymode.Height == height &&
						pixfmt.bitsPerPixel() == bitsperpixel )
					{
						modefound = true;
						break;
					}
				}
			}
		}

		if ( !modefound )
			return ERROR_FULLSCREENINITFAILED;
	}

	// find depth buffer format
	D3DFORMAT depthformat = D3DFMT_UNKNOWN;
	if ( SURFACE_DEPTH & buffers )
	{
		if ( buffers & SURFACE_STENCIL )
			depthformat = findDepthBuffer( DEPTH_STENCIL_FORMATS, m_d3d, m_displaymode.Format, devtype );
		else
			depthformat = findDepthBuffer( DEPTH_FORMATS, m_d3d, m_displaymode.Format, devtype );

		if ( depthformat == D3DFMT_UNKNOWN )
		{
			m_d3d->Release();
			m_d3d = 0;

			if ( buffers & SURFACE_STENCIL )
				return ERROR_NOSTENCIL;
			else
				return ERROR_NODEPTH;
		}
	}
	else if ( SURFACE_STENCIL & buffers )
		return ERROR_NOSTENCILWITHOUTSDEPTH;

	// set up presentation parameters
	zero( m_present );
	m_present.hDeviceWindow = hwnd;
	m_present.BackBufferFormat = m_displaymode.Format;
	m_present.BackBufferCount = 1;
	m_present.MultiSampleType = D3DMULTISAMPLE_NONE;
	m_present.SwapEffect = D3DSWAPEFFECT_DISCARD;
	m_present.EnableAutoDepthStencil = ( 0 != (SURFACE_DEPTH & buffers) );
	m_present.AutoDepthStencilFormat = depthformat;
	m_present.Windowed = (win != WINDOW_FULLSCREEN);
	m_present.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
	if ( win == WINDOW_FULLSCREEN )
	{
		m_present.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
		m_present.BackBufferWidth = width;
		m_present.BackBufferHeight = height;
	}
	if ( debugShaders() || m_lockablebuffer )
		m_present.Flags |= D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;

	// set up device creation flags
	DWORD devflags = 0;
	D3DCAPS9 devcaps;
	m_d3d->GetDeviceCaps( D3DADAPTER_DEFAULT, devtype, &devcaps );
	if ( vp != VERTEXP_SW && 0 != (D3DDEVCAPS_HWTRANSFORMANDLIGHT  & devcaps.DevCaps) )
	{
		devflags |= D3DCREATE_HARDWARE_VERTEXPROCESSING;
	}
	else
	{
		devflags |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;
	}

	// create device
	HRESULT hr = DX_TRY( m_d3d->CreateDevice( D3DADAPTER_DEFAULT, devtype, hwnd, devflags, &m_present, &m_device ) );
	if ( D3D_OK != hr )
	{
		m_d3d->Release();
		m_d3d = 0;

		if ( !m_present.Windowed )
			return ERROR_FULLSCREENINITFAILED;
		else
			return ERROR_DESKTOPINITFAILED;
	}

	// store device capabilities
	m_device->GetDeviceCaps( &m_caps );

	// store rendering surface properties
	m_bbtargetfmt = toSurfaceFormat( m_present.BackBufferFormat );
	m_buffers = buffers;
	m_renderwidth = m_present.BackBufferWidth;
	m_renderheight = m_present.BackBufferHeight;
	m_viewport = Rect(m_renderwidth,m_renderheight);
	if ( m_present.Windowed )
		getClientSize( m_present.hDeviceWindow, &m_renderwidth, &m_renderheight );

	return ERROR_NONE;
}

void DX_Context::error( Error err, const char* devicedesc )
{
	if ( err != ERROR_NONE )
	{
		const char* errstr = getErrorString(err);
		Debug::printf( "ERROR: gr/dx: %s\n", errstr );
		throwError( DX_GraphicsException( Format("{0} {1}", errstr, devicedesc) ) );
	}
}

const char* DX_Context::getErrorString( Error err )
{
	switch ( err )
	{
	case ERROR_NONE: return "No error.";
	case ERROR_GENERIC: return "Operation failed for unknown reason.";
	case ERROR_DIRECTXNOTINSTALLED: return "Failed to create main DirectX object. DirectX 9 installed?";
	case ERROR_NODEPTH: return "No compatible depth buffer format.";
	case ERROR_NOSTENCIL: return "No compatible stencil buffer format.";
	case ERROR_NOSTENCILWITHOUTSDEPTH: return "Cannot create stencil buffer without depth buffer.";
	case ERROR_FULLSCREENINITFAILED: return "Failed to initialize full-screen window mode.";
	case ERROR_DESKTOPINITFAILED: return "Failed to initialize desktop window mode.";
	case ERROR_NOACTIVEWINDOW: return "A window must be active before create() is called.";
	case ERROR_EFFECTCOMPILATIONERROR: return "Effect description compilation failed.";
	case ERROR_EFFECTUNSUPPORTED: return "Effect cannot be rendered on current device.";
	case ERROR_PALETTETEXTURESNOTSUPPORTED: return "Palettized textures not supported in DirectX 9.";
	default: return "Operation failed for unknown reason.";
	}
}

Palette* DX_Context::createPalette( int )
{
	error( ERROR_PALETTETEXTURESNOTSUPPORTED, "" );
	return 0;
}

Primitive* DX_Context::createPrimitive( Primitive::PrimType prim,
	const VertexFormat& vf, int vertices, int indices, UsageFlags usage )
{
	return new DX_Primitive( this, prim, vf, vertices, indices, usage );
}

Primitive* DX_Context::getDynamicPrimitive( Primitive::PrimType prim, const VertexFormat& vf, int vertices, int indices )
{
	for ( int i = 0 ; i < m_dynamicPrimitives.size() ; ++i )
	{
		Primitive* p = m_dynamicPrimitives[i];
		if ( p->vertexFormat() == vf && 
			p->type() == prim && 
			p->vertices() >= vertices && 
			p->indices() >= indices )
		{
			return p;
		}
	}
	m_dynamicPrimitives.add( new DX_Primitive( this, prim, vf, (vertices+31)&~31, (indices+31)&~31, Context::USAGE_DYNAMIC ) );
	return m_dynamicPrimitives.last();
}

Texture* DX_Context::createTexture( const String& filename )
{
	return new DX_Texture( this, filename );
}

CubeTexture* DX_Context::createCubeTexture( const String& filename )
{
	return new DX_CubeTexture( this, filename );
}

Shader* DX_Context::createShader( const String& name, int flags )
{
	char flagsstr[32];
	sprintf( flagsstr, "%x", flags );

	io::PathName pathname( name );
	String basename = pathname.basename();
	String hashname = basename + flagsstr;
	P(Shader) shader = m_shaders[hashname];

	if ( shader == 0 )
	{
		shader = new DX_Shader( this, basename, io::PathName(pathname.parent().toString(),basename+".fx").toString(), flags );
		m_shaders[hashname] = shader;
		return shader;
	}

	return shader->clone();
}

Texture* DX_Context::createTexture( int width, int height, 
	const SurfaceFormat& fmt, Palette*, int usageflags )
{
	return new DX_Texture( this, width, height, fmt, usageflags );
}

void DX_Context::beginScene()
{
	DX_TRY( m_device->BeginScene() );
}

void DX_Context::endScene()
{
	DX_TRY( m_device->EndScene() );
}

int DX_Context::width() const
{
	return m_renderwidth;
}

int DX_Context::height() const
{
	return m_renderheight;
}

SurfaceFormat DX_Context::surfaceFormat() const
{
	return m_bbtargetfmt;
}

const Rect& DX_Context::viewport() const
{
	return m_viewport;
}

Context::PlatformType DX_Context::platform() const
{
	return PLATFORM_DX;
}

const float4x4& DX_Context::projectionTransform() const
{
	return projtm;
}

void DX_Context::printObjects()
{
	Debug::printf( "gr/fx: Context object list:\n" );
	for ( DX_ContextItem* it = itemlist.last() ; it != 0 ; it = it->previous() )
	{
		Debug::printf( "gr/fx: %s\n", it->toString().c_str() );
	}
}

void DX_Context::capture( const String& namefmtstr )
{
	assert( m_device );
	assert( m_lockablebuffer && "Back buffer needs to be lockable for capture() to work" );

	char namefmt[256];
	namefmtstr.get( namefmt, sizeof(namefmt) );

	IDirect3DSurface9* bb = 0;
	HRESULT hr = DX_TRY( m_device->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &bb ) );
	if ( D3D_OK == hr )
	{
		char fname[256];
		sprintf( fname, namefmt, ++m_capturenum );

		D3DXIMAGE_FILEFORMAT type = D3DXIFF_JPG;
		String str = namefmtstr.toLowerCase();
		if ( str.endsWith(".bmp") )
			type = D3DXIFF_BMP;
		else if ( str.endsWith(".jpg") )
			type = D3DXIFF_JPG;
		else if ( str.endsWith(".tga") )
			type = D3DXIFF_TGA;
		else if ( str.endsWith(".png") )
			type = D3DXIFF_PNG;
		else if ( str.endsWith(".dds") )
			type = D3DXIFF_DDS;
		else if ( str.endsWith(".ppm") )
			type = D3DXIFF_PPM;
		else if ( str.endsWith(".dib") )
			type = D3DXIFF_DIB;
		else if ( str.endsWith(".hdr") )
			type = D3DXIFF_HDR;
		else if ( str.endsWith(".pfm") )
			type = D3DXIFF_PFM;

		DX_TRY( D3DXSaveSurfaceToFile(fname, type, bb, 0, 0) );

		DX_release( bb );
	}
}

DX_Context::OrientationType	DX_Context::orientation() const
{
	return ORIENTATION_0;
}


END_NAMESPACE() // gr

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
