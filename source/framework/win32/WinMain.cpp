//#define FRAMEWORK_WIN32_DX
//#define FRAMEWORK_WIN32_EGL
//#define FRAMEWORK_WIN32_SW
#define FRAMEWORK_WIN32_N3D

#include <io/PathName.h>
#include <io/FindFile.h>
#include <io/FileInputStream.h>
#include <gr/Context.h>

#if defined(FRAMEWORK_WIN32_DX)
#include <gr/dx/DX_Context.h>

#elif defined(FRAMEWORK_WIN32_SW)
#include <gr/sw/SW_Context.h>

#elif defined(FRAMEWORK_WIN32_N3D)
#include <gr/n3d/N3D_Context.h>
#include <framework/ngi/NGISystem.h>

#elif defined(FRAMEWORK_WIN32_EGL)
BEGIN_NAMESPACE(gr)  NS(gr,Context)* EGL_createContext( void* window ); END_NAMESPACE()

#else
#error No platform specific NS(gr,Context) included in WinMain.cpp
#endif

#include <hgr/Globals.h>
#include <hgr/ParticleSystem.h>
#include <hgr/DefaultResourceManager.h>
#include <lang/Debug.h>
#include <lang/System.h>
#include <lang/Globals.h>
#include <lang/Exception.h>
#include <framework/App.h>
#include <stdio.h>
#include <direct.h>
#include <windows.h>
#include <crtdbg.h>
#include <config.h>


USING_NAMESPACE(io)
USING_NAMESPACE(gr)
USING_NAMESPACE(hgr)
USING_NAMESPACE(lang)
USING_NAMESPACE(framework)


class OSWin32 :
	public NS(framework,OSInterface)
{
public:
	void			loadShaders( const NS(lang,String)& path, bool recursesubdirs );
	NS(lang,String)	getDefaultDataPath();
};

static const int s_keyToVk[] =
{
	0,
	'A',
	'B',
	'C',
	'D',
	'E',
	'F',
	'G',
	'H',
	'I',
	'J',
	'K',
	'L',
	'M',
	'N',
	'O',
	'P',
	'Q',
	'R',
	'S',
	'T',
	'U',
	'V',
	'W',
	'X',
	'Y',
	'Z',
	VK_NEXT,
	VK_PRIOR,
	VK_HOME,
	VK_END,
	VK_INSERT,
	VK_DELETE,
	VK_RETURN,
	VK_SHIFT,
	VK_CONTROL,
	VK_TAB,
	VK_SPACE,
	VK_BACK,
	VK_PAUSE,
	VK_ESCAPE,
	VK_F1,
	VK_F2,
	VK_F3,
	VK_F4,
	VK_F5,
	VK_F6,
	VK_F7,
	VK_F8,
	VK_F9,
	VK_F10,
	VK_F11,
	VK_F12,
	VK_LEFT,
	VK_RIGHT,
	VK_UP,
	VK_DOWN,
	0,
	0,
	0,
	VK_NUMPAD0,
	VK_NUMPAD1,
	VK_NUMPAD2,
	VK_NUMPAD3,
	VK_NUMPAD4,
	VK_NUMPAD5,
	VK_NUMPAD6,
	VK_NUMPAD7,
	VK_NUMPAD8,
	VK_NUMPAD9,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0
};

static P(Context)					s_context			= 0;
static P(DefaultResourceManager)	s_defResMgr			= 0;
static bool							s_active			= false;
static Array<String>				s_shaderfullnames;
static Array<String>				s_shaderbasenames;
static Array<String>				s_particlefullnames;
static Array<String>				s_particlebasenames;
static HWND							s_hwnd;
static bool							s_captureMouse;
static App::Configuration			s_config;

// NOTE: These globals *need* to be defined this way, N3D uses these directly, uh...
int*						g_FrontBuffer = 0;
int							g_FrontWidth;
int							g_FrontHeight;

static void deinit()
{
	s_defResMgr = 0;

	// make sure memory from static buffers are released
	{Array<String> tmp; s_shaderbasenames.swap( tmp );}
	{Array<String> tmp; s_shaderfullnames.swap( tmp );}
	{Array<String> tmp; s_particlebasenames.swap( tmp );}
	{Array<String> tmp; s_particlefullnames.swap( tmp );}
	s_config = App::Configuration();

	if ( App::initialized() )
		App::deleteApp();

	NS(hgr,Globals)::cleanup();
	assert( 1 == s_context->references() );
	s_context = 0;

	delete[] g_FrontBuffer;
	g_FrontBuffer = 0;
}

/** Translates Win32 Virtual Key codes to KeyType. */
static App::KeyType translateVKCode( int vk )
{
	assert( sizeof(s_keyToVk)/sizeof(s_keyToVk[0]) == App::KEY_COUNT );

	if ( vk >= '1' && vk <= '9' || vk == '0' )
	{
		if ( vk >= '1' && vk <= '9' )
			return (App::KeyType)( App::KEY_1 + (vk - '1') );
		else
			return App::KEY_0;
	}

	for ( int i = 0 ; i < App::KEY_COUNT ; ++i )
	{
		if ( s_keyToVk[i] == vk )
			return (App::KeyType)i;
	}

	return App::KEY_COUNT;
}

/** Sets key state and signals application by calling keyDown/keyUp if state changed */
static void setKeyState( App* a, App::KeyType key, bool keydown )
{
	if ( a != 0 )
	{
		if ( keydown )
		{
			if ( !a->isKeyDown(key) )
				a->keyDown( key );
		}
		else
		{
			a->keyUp( key );
		}
		a->setKeyDown( key, keydown );
	}
}

static LRESULT wndproc( HWND hwnd, UINT msg, WPARAM wp, LPARAM lp )
{
	App* a = 0;
	if ( App::initialized() )
		a = App::get();

	switch ( msg )
	{
	case WM_SETCURSOR:
		if ( s_captureMouse )
		{
			SetCursor( 0 );
			return TRUE;
		}
		break;

	case WM_CLOSE:
		deinit();
		PostQuitMessage(0);
		return 0;

	case WM_LBUTTONDOWN:
		setKeyState( a, App::KEY_LBUTTON, true );
		break;

	case WM_LBUTTONUP:
		setKeyState( a, App::KEY_LBUTTON, false );
		break;

	case WM_MBUTTONDOWN:
		setKeyState( a, App::KEY_MBUTTON, true );
		break;

	case WM_MBUTTONUP:
		setKeyState( a, App::KEY_MBUTTON, false );
		break;

	case WM_RBUTTONDOWN:
		setKeyState( a, App::KEY_RBUTTON, true );
		break;

	case WM_RBUTTONUP:
		setKeyState( a, App::KEY_RBUTTON, false );
		break;

	case WM_MOUSEMOVE:
		if ( a != 0 )
		{
			int cx = 0;
			int cy = 0;

			if ( s_captureMouse )
			{
				RECT rc;
				GetClientRect( hwnd, &rc );
				cx = rc.right/2;
				cy = rc.bottom/2;
			}

			POINT p;
			p.x = short(lp&0xFFFF);
			p.y = short((lp>>16)&0xFFFF);
			App::get()->mouseMove( p.x-cx, p.y-cy );
		}
		return 0;

	case WM_KEYDOWN:
	case WM_KEYUP:{
		App::KeyType key = translateVKCode((int)wp);
		if ( key != App::KEY_COUNT )
			setKeyState( a, key, WM_KEYDOWN==msg );
		break;}

	case WM_ACTIVATE:{
		s_active = ( LOWORD(wp) != WA_INACTIVE );

		char name[256];
		GetWindowText( hwnd, name, sizeof(name) );
		Debug::printf( "framework: Window \"%s\" %s\n", name, (s_active?"activated":"deactivated") );

		if ( App::initialized() )
			App::get()->activate( s_active );
		break;}
	}

	return DefWindowProc( hwnd, msg, wp, lp );
}

static HWND createMainWindow( const char* classname, const char* name,
	int w, int h, bool fullscreen, HINSTANCE instance, int iconresid )
{
	DWORD style = WS_VISIBLE|WS_OVERLAPPEDWINDOW;
	DWORD exstyle = 0;
	if ( fullscreen )
	{
		style = WS_VISIBLE|WS_POPUP;
		exstyle = WS_EX_TOPMOST;
	}

	HWND parenthwnd = 0;
	HICON icon = 0;
	if ( iconresid > 0 )
		icon = LoadIcon( instance, MAKEINTRESOURCE(iconresid) );

	WNDCLASSEXA wndclassex;
	wndclassex.cbSize			= sizeof(WNDCLASSEXA); 
	wndclassex.style			= CS_HREDRAW | CS_VREDRAW;
	wndclassex.lpfnWndProc		= (WNDPROC)wndproc;
	wndclassex.cbClsExtra		= 0;
	wndclassex.cbWndExtra		= 0;
	wndclassex.hInstance		= (HINSTANCE)instance;
	wndclassex.hIcon			= icon;
	wndclassex.hCursor			= s_captureMouse ? 0 : LoadCursor(NULL, IDC_ARROW);
	wndclassex.hbrBackground	= (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclassex.lpszMenuName		= 0;
	wndclassex.lpszClassName	= classname;
	wndclassex.hIconSm			= 0;
	bool ok = RegisterClassExA(&wndclassex) != 0;
	assert( ok );

	void* userparam = 0;
	HWND hwnd = CreateWindowExA( exstyle, classname, name, style, 0, 0, w, h, parenthwnd, 0, instance, userparam );
	if ( !hwnd )
	{
		UnregisterClass( classname, instance );
		return 0;
	}

	RECT cr;
	GetClientRect( hwnd, &cr );
	RECT wr;
	GetWindowRect( hwnd, &wr );
	MoveWindow( hwnd, wr.left, wr.top, w+(w-cr.right), h+(h-cr.bottom), TRUE );

	if ( fullscreen )
		ShowCursor( FALSE );
	return hwnd;
}

static bool flushMessages()
{
	MSG msg;
	if ( PeekMessage(&msg,0,0,0,PM_NOREMOVE) )
	{
		while ( WM_QUIT != msg.message &&
			PeekMessage(&msg,0,0,0,PM_REMOVE) )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}

		return WM_QUIT != msg.message;
	}
	return true;
}

static bool isUsed( const String& basename, const char* const* usedlist )
{
	if ( usedlist != 0 )
	{
		for ( int i = 0 ; 0 != usedlist[i] ; ++i )
		{
			if ( basename == usedlist[i] )
				return true;
		}
		return false;
	}
	return true;
}

#ifdef FRAMEWORK_WIN32_DX
static void loadShaders( Context* context, const String& path, 
	bool recursesubdirs, ResourceManager* res, const char* const* usedshaders )
{
	P(Shader) fx;
	for ( FindFile find(PathName(path,"*.fx").toString()) ; find.more() ; find.next() )
	{
		// ensure that shader basename is unique
		String shadername = find.data().path.toString();
		String basename = find.data().path.basename();

		if ( !isUsed(basename,usedshaders) )
			continue;

		int cloneix = s_shaderbasenames.indexOf( basename );
		if ( -1 != cloneix )
			throwError( Exception( Format("Shader basenames must be unique, but \"{0}\" and \"{1}\" have identical basenames", s_shaderfullnames[cloneix], shadername) ) );
		s_shaderfullnames.add( shadername );
		s_shaderbasenames.add( basename );

		fx = res->getShader( shadername, 0 );
		fx = res->getShader( shadername, 1 );
	}
}

static void loadParticles( Context* context, const String& path, 
	bool recursesubdirs, ResourceManager* res, const char* const* usedparticles )
{
	P(ParticleSystem) prt;
	for ( FindFile find(PathName(path,"*.prs").toString()) ; find.more() ; find.next() )
	{
		// ensure that particle basename is unique
		String particlename = find.data().path.toString();
		String basename = find.data().path.basename();

		if ( !isUsed(basename,usedparticles) )
			continue;

		int cloneix = s_particlebasenames.indexOf( basename );
		if ( -1 != cloneix )
			throwError( Exception( Format("Particle basenames must be unique, but \"{0}\" and \"{1}\" have identical basenames", s_particlefullnames[cloneix], particlename) ) );
		s_particlefullnames.add( particlename );
		s_particlebasenames.add( basename );

		String filename = find.data().path.toString();
		String texpath = find.data().path.parent().toString();
		String shaderpath = find.data().path.parent().toString();
		prt = res->getParticleSystem( filename, texpath, shaderpath );
	}
}
#endif

static void swCreateFrameBuffer( HWND hwnd, int width, int height )
{
	const bool USE_WINDOW_CLIENT_AREA_SIZE = false;

	assert( hwnd != 0 );
	RECT cr;
	GetClientRect( hwnd, &cr );
	if ( USE_WINDOW_CLIENT_AREA_SIZE )
	{
		width = cr.right;
		height = cr.bottom;
	}
	assert( width > 0 );
	assert( height > 0 );

	// create frame buffer
	int pixels = width*height;
	int framebufferbytes = pixels;
	delete[] g_FrontBuffer;
	g_FrontBuffer = new int[ pixels ];
	for ( int i = 0 ; i < pixels ; ++i )
		g_FrontBuffer[i] = 0;

	g_FrontWidth = width;
	g_FrontHeight = height;
}

// XRGB 4444
void swBltBackBuffer12( HWND hwnd, uint16_t* backbuf, int* zbuf, int w, int h, int backbufferpitch )
{
	int* framebuf = g_FrontBuffer;
	for ( int j = 0 ; j < h ; ++j )
	{
		for ( int i = 0 ; i < w ; ++i )
		{
			int c = backbuf[i];
			//int c = zbuf[i];
			//if ( c != 0x7FFFFFFF )
			//	c = 0xFFFFFFFF;
			int r = (c>>8)&0xF;
			int g = (c>>4)&0xF;
			int b = (c)&0xF;
			r = (r*255)/15;
			g = (g*255)/15;
			b = (b*255)/15;
			framebuf[i] = int( (r<<16) + (g<<8) + (b) );
		}
		backbuf = (uint16_t*)( (uint8_t*)backbuf + backbufferpitch );
		zbuf += w;
		framebuf += w;
	}

	// framebuffer -> window
	char bminfomem[ sizeof(BITMAPINFO) + 16 ];
	memset( &bminfomem, 0, sizeof(bminfomem) );
	BITMAPINFO bminfo = *reinterpret_cast<BITMAPINFO*>(bminfomem);
	bminfo.bmiHeader.biSize = sizeof( BITMAPINFOHEADER );
	bminfo.bmiHeader.biWidth = w;
	bminfo.bmiHeader.biHeight = -h;
	bminfo.bmiHeader.biPlanes = 1;
	bminfo.bmiHeader.biBitCount = 32;
	bminfo.bmiHeader.biCompression = BI_RGB;
    bminfo.bmiHeader.biSizeImage = 0;
    bminfo.bmiHeader.biXPelsPerMeter = 0;
    bminfo.bmiHeader.biYPelsPerMeter = 0;
    bminfo.bmiHeader.biClrUsed = 0;
    bminfo.bmiHeader.biClrImportant = 0;
	//((unsigned long*)bminfo.bmiColors)[0] = 0xFF0000;
	//((unsigned long*)bminfo.bmiColors)[1] = 0xFF00;
	//((unsigned long*)bminfo.bmiColors)[2] = 0xFF;

	HDC hdc = GetDC( hwnd );
	if ( 0 != hdc )
	{
		StretchDIBits( hdc, 0, 0, w, h, 0, 0, w, h, g_FrontBuffer, &bminfo, DIB_RGB_COLORS, SRCCOPY );
		ReleaseDC( hwnd, hdc );
	}
}

// RGB 565
static void swBltBackBuffer16( HWND hwnd, uint16_t* backbuf, int w, int h, int backbufferpitch )
{
	int* framebuf = g_FrontBuffer;
	for ( int j = 0 ; j < h ; ++j )
	{
		for ( int i = 0 ; i < w ; ++i )
		{
			int c = backbuf[i];
			if ( c != 0 )
			{
				c = c;
			}
			int r = (c>>11)&0x1F;
			int g = (c>>5)&0x3F; // rrrrrggggggbbbbb
			int b = (c)&0x1F;
			r = (r*255)/31;
			g = (g*255)/63;
			b = (b*255)/31;
			framebuf[i] = int( (r<<16) + (g<<8) + (b) );
		}
		backbuf = (uint16_t*)( (uint8_t*)backbuf + backbufferpitch );
		framebuf += w;
	}

	// framebuffer -> window
	char bminfomem[ sizeof(BITMAPINFO) + 16 ];
	memset( &bminfomem, 0, sizeof(bminfomem) );
	BITMAPINFO bminfo = *reinterpret_cast<BITMAPINFO*>(bminfomem);
	bminfo.bmiHeader.biSize = sizeof( BITMAPINFOHEADER );
	bminfo.bmiHeader.biWidth = w;
	bminfo.bmiHeader.biHeight = -h;
	bminfo.bmiHeader.biPlanes = 1;
	bminfo.bmiHeader.biBitCount = 32;
	bminfo.bmiHeader.biCompression = BI_RGB;
    bminfo.bmiHeader.biSizeImage = 0;
    bminfo.bmiHeader.biXPelsPerMeter = 0;
    bminfo.bmiHeader.biYPelsPerMeter = 0;
    bminfo.bmiHeader.biClrUsed = 0;
    bminfo.bmiHeader.biClrImportant = 0;
	//((unsigned long*)bminfo.bmiColors)[0] = 0xFF0000;
	//((unsigned long*)bminfo.bmiColors)[1] = 0xFF00;
	//((unsigned long*)bminfo.bmiColors)[2] = 0xFF;

	HDC hdc = GetDC( hwnd );
	if ( 0 != hdc )
	{
		StretchDIBits( hdc, 0, 0, w, h, 0, 0, w, h, g_FrontBuffer, &bminfo, DIB_RGB_COLORS, SRCCOPY );
		ReleaseDC( hwnd, hdc );
	}
}

// XRGB 8888
static void swBltBackBuffer32( HWND hwnd, int* backbuf, int w, int h, int backbufferpitch )
{
	assert( w == g_FrontWidth );

	int* framebuf = g_FrontBuffer;
	for ( int j = 0 ; j < h ; ++j )
	{
		for ( int i = 0 ; i < w ; ++i )
		{
			int c = backbuf[i];
			framebuf[i] = c;
		}
		backbuf = (int*)( (uint8_t*)backbuf + backbufferpitch );
		framebuf += w;
	}

	// framebuffer -> window
	char bminfomem[ sizeof(BITMAPINFO) + 16 ];
	memset( &bminfomem, 0, sizeof(bminfomem) );
	BITMAPINFO& bminfo = *reinterpret_cast<BITMAPINFO*>(bminfomem);
	bminfo.bmiHeader.biSize = sizeof( BITMAPINFOHEADER );
	bminfo.bmiHeader.biPlanes = 1;
	bminfo.bmiHeader.biBitCount = 32;
	bminfo.bmiHeader.biCompression = BI_RGB;
	bminfo.bmiHeader.biWidth = w;
	bminfo.bmiHeader.biHeight = -h;
	((unsigned long*)bminfo.bmiColors)[0] = 0xFF0000;
	((unsigned long*)bminfo.bmiColors)[1] = 0xFF00;
	((unsigned long*)bminfo.bmiColors)[2] = 0xFF;

	HDC hdc = GetDC( hwnd );
	if ( 0 != hdc )
	{
		StretchDIBits( hdc, 0, 0, w, h, 0, 0, w, h, g_FrontBuffer, &bminfo, DIB_RGB_COLORS, SRCCOPY );
		ReleaseDC( hwnd, hdc );
	}
}

#ifdef FRAMEWORK_WIN32_SW
static void swBltBackBuffer( HWND hwnd, SW_Context* swcontext )
{
	int width = swcontext->width();
	int height = swcontext->height();

	// backbuffer -> framebuffer
	uint16_t* backbuf;
	int w, h, backbufferpitch;
	swcontext->renderContext()->getBuffer( (void**)&backbuf, &w, &h, &backbufferpitch );
	int* zbuf;
	swcontext->renderContext()->getDepthBuffer( &zbuf );
	assert( w == swcontext->width() );
	assert( h == swcontext->height() );

	swBltBackBuffer12( hwnd, backbuf, zbuf, w, h, backbufferpitch );

	swcontext->clear( swcontext->clearColor() );
}
#endif

void rotateScreen(App* app, gr::Context::OrientationType ort)
{
	int w = s_context->height();
	int h = s_context->width();
	static_cast<N3D_Context*>(s_context.ptr())->reset( 0, w, h, ort );
	swCreateFrameBuffer( s_hwnd, w, h );

	RECT cr, wr;
	POINT ptDiff;
	GetClientRect(s_hwnd, &cr);
	GetWindowRect(s_hwnd, &wr);
	ptDiff.x = (wr.right - wr.left) - cr.right;
	ptDiff.y = (wr.bottom - wr.top) - cr.bottom;
	MoveWindow( s_hwnd, wr.left, wr.top, w+ptDiff.x, h+ptDiff.y, TRUE );

	app->orientationChanged();
}

static int run( HINSTANCE instance )
{
	try
	{
		OSWin32 os;

		char cwd[512];
		_getcwd( cwd, sizeof(cwd) );
		Debug::printf( "Current working directory is %s\n", cwd );

		int keycodes = sizeof(s_keyToVk)/sizeof(s_keyToVk[0]);
		int keytypes = App::KEY_COUNT;
		assert( keycodes == keytypes && "App::KeyType codes don't match Win32 virtual key codes" );
		
		// application specific config
		s_config.name = "Example Application";
		s_config.width = 800;
		s_config.height = 600;
		s_config.bits = 32;
		s_config.fullscreen = false;
		s_config.stencilbuffer = true;
		s_config.captureMouse = false;
		s_config.iconResourceID = 0;
#ifdef _DEBUG
		s_config.debugMemoryAllocs = true;
#else
		s_config.debugMemoryAllocs = false;
#endif
		configure( s_config );

		if ( s_config.debugMemoryAllocs )
		{
			_CrtSetDbgFlag( _CRTDBG_CHECK_ALWAYS_DF | _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) );
		}

		s_captureMouse = s_config.captureMouse;
		
		int winw = s_config.width;
		int winh = s_config.height;
#ifdef FRAMEWORK_WIN32_N3D
		// create only square windows on N3D (NOT NEEDED IF WINDOW IS RESIZED DYNAMICALLY)
		//winw = winw > winh ? winw : winh;
		//winh = winw;
#endif

		// create main window
		s_hwnd = createMainWindow( __FILE__, "Loading...", winw, winh, s_config.fullscreen, instance, s_config.iconResourceID );
		if ( !s_hwnd )
		{
			MessageBox( 0, "Failed to create main window", "Error", MB_OK );
			return 1;
		}

		// init rendering context
#ifdef FRAMEWORK_WIN32_DX
		int surfaces = DX_Context::SURFACE_TARGET + DX_Context::SURFACE_DEPTH;
		if ( s_config.stencilbuffer )
			surfaces += DX_Context::SURFACE_STENCIL;
		s_context = new DX_Context( s_config.width, s_config.height, s_config.bits, 
			s_config.fullscreen ? DX_Context::WINDOW_FULLSCREEN : DX_Context::WINDOW_DESKTOP,
			DX_Context::RASTERIZER_HW,
			DX_Context::VERTEXP_HW,
			surfaces );

#elif defined(FRAMEWORK_WIN32_EGL)
		s_context = EGL_createContext( s_hwnd );

#elif defined(FRAMEWORK_WIN32_SW)
		s_context = SW_createContext( s_hwnd, s_config.width, s_config.height );
		swCreateFrameBuffer( s_hwnd, s_config.width, s_config.height );

#elif defined(FRAMEWORK_WIN32_N3D)
		s_context = N3D_createContext( 0, s_config.width, s_config.height );
		swCreateFrameBuffer( s_hwnd, s_config.width, s_config.height );
#else
#error Context creation not defined on this platform
#endif

		#ifdef _DEBUG
		s_config.name = s_config.name + " (DEBUG/" + String(s_context->platformString()).toUpperCase() + ")";
		SetWindowText( s_hwnd, s_config.name.c_str() );
		#endif

		// set default resource manager
		s_defResMgr = new DefaultResourceManager( s_context );
		DefaultResourceManager::set( s_defResMgr );

		// find resources from current working directory
		s_defResMgr->findTextureResources( "*" );

		// try to load shader set first from data/shaders, then from KA3D/shaders
#ifdef FRAMEWORK_WIN32_DX
		String shaderpath = PathName( os.getDefaultDataPath(), "shaders").toString();
		if ( FindFile("data/shaders",0).more() )
			shaderpath = "data/shaders";
		::loadShaders( s_context, shaderpath, true, DefaultResourceManager::get(s_context), s_config.usedShaders );
		//DefaultResourceManager::findShaderResources( PathName(shaderpath,"*.fx").toString(), s_context );

		// try to load particle set
		String particlepath = PathName( os.getDefaultDataPath(), "particles").toString();
		if ( FindFile("data/particles",0).more() )
			particlepath = "data/particles";
		::loadParticles( s_context, particlepath, true, DefaultResourceManager::get(s_context), s_config.usedParticles );
		//DefaultResourceManager::findParticleResources( PathName(particlepath,"*.prs").toString(), s_context );
#endif

		// init app instance
		App* app = init( &os, s_context );
		assert( app != 0 );
		SetWindowText( s_hwnd, s_config.name.c_str() );

		// prepare for mouse input
		MOUSEMOVEPOINT prevmousepoint;
		ZeroMemory( &prevmousepoint, sizeof(prevmousepoint) );

		// main loop
		int oldtime = System::currentTimeMillis();
		while ( flushMessages() )
		{ 
			if ( s_config.captureMouse )
			{
				if ( s_active && s_context->ready() && !app->quitRequested() )
				{
					SetCapture( s_hwnd );
					ShowCursor( FALSE );

					RECT rc;
					GetClientRect( s_hwnd, &rc );
					POINT c;
					c.x = rc.right/2;
					c.y = rc.bottom/2;
					ClientToScreen( s_hwnd, &c );
					SetCursorPos( c.x, c.y );
				}
				else
				{
					ReleaseCapture();
					ShowCursor( TRUE );
				}
			}

			int time = System::currentTimeMillis();
			if ( time-oldtime < 1000/30 )
				continue;
			float dt = float(time-oldtime)*1e-3f;
			oldtime = time;

			if ( s_active && s_context->ready() )
			{
				if ( dt > 1.f )
					dt = 1.f;

#if defined(FRAMEWORK_WIN32_N3D)
				static bool resetdown = false;
				if ( GetKeyState('R') < 0 && !resetdown )
				{
					Context::OrientationType ort = (Context::OrientationType)(s_context->orientation() + 1);
					if(ort == Context::ORIENTATION_270 + 1)
						ort = Context::ORIENTATION_0;

					rotateScreen(app, ort);
					resetdown = true;
				}
				if ( GetKeyState('R') >= 0 )
					resetdown = false;
#endif

				app->update( dt, s_context );

				if ( app->quitRequested() )
				{
					ReleaseCapture();
					deinit();
					break;
				}
				else
				{
#if defined(FRAMEWORK_WIN32_SW)
					swBltBackBuffer( s_hwnd, static_cast<SW_Context*>(s_context.ptr()) );
#endif
				}
			}
		}
	}
	catch ( Throwable& e )
	{	
		deinit();

		// minimize window so that it doesn't overlap message box
		if ( s_hwnd != 0 )
		{
			ShowCursor( TRUE );
			MoveWindow( s_hwnd, 0, 0, 4, 4, TRUE );
		}

		MessageBox( s_hwnd, e.getMessage().format().c_str(), "Error", MB_OK );
	}

	if ( s_hwnd )
	{
		DestroyWindow( s_hwnd ); 
		s_hwnd = 0;
	}

    return 0;
}

int WINAPI WinMain( HINSTANCE instance, HINSTANCE, LPSTR /*commandline*/, int /*commandshow*/ )
{
	int* allocmarker = new int(0x78563412);
	int retval = run( instance );
	Globals::cleanup();
	lang_Globals::cleanup();
	GlobalStorage::release();
	delete allocmarker;
	_CrtDumpMemoryLeaks();
	return retval;
}

String OSWin32::getDefaultDataPath()
{
	// check if KA3D is defined, use that as default data path, otherwise data/
	const char* env = getenv("KA3D");
	if ( env == 0 )
		return "data/";
	else
		return PathName(env,"data").toString();
}

void OSWin32::loadShaders( const String& path, bool recursesubdirs )
{
#ifdef FRAMEWORK_WIN32_DX
	::loadShaders( s_context, path, recursesubdirs, DefaultResourceManager::get(s_context), 0 );
#endif
}

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
