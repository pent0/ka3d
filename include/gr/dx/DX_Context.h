#ifndef _GR_DX_CONTEXT_H
#define _GR_DX_CONTEXT_H


#include "DX_common.h"
#include "DX_Shader.h"
#include "DX_Primitive.h"
#include "DX_ContextItem.h"
#include <gr/Rect.h>
#include <gr/Context.h>
#include <gr/SurfaceFormat.h>
#include <gr/impl/SortBuffer.h>
#include <lang/Array.h>
#include <lang/Hashtable.h>
#include <math/float4x4.h>


BEGIN_NAMESPACE(gr) 


/** 
 * Rendering context for DirectX Graphics.
 * 
 */
class DX_Context :
	public NS(gr,Context)
{
public:
	/** 
	 * Device window type. 
	 */
	enum WindowType
	{
		/** Render to desktop-window. */
		WINDOW_DESKTOP,
		/** Render to full-screen window. */
		WINDOW_FULLSCREEN
	};

	/**
	 * Device rasterizer type.
	 */
	enum RasterizerType
	{
		/** Software rasterizer. */
		RASTERIZER_SW,
		/** Hardware rasterizer. */
		RASTERIZER_HW
	};

	/**
	 * Device vertex processing type.
	 */
	enum VertexProcessingType
	{
		/** Software vertex processing. */
		VERTEXP_SW,
		/** Hardware vertex processing. */
		VERTEXP_HW,
	};

	/** 
	 * Device frame buffer surface flags. 
	 */
	enum SurfaceFlags
	{
		/** Frame	buffer color channel. */
		SURFACE_TARGET		= 1,
		/** Frame buffer depth channel. */
		SURFACE_DEPTH		= 2,
		/** Frame buffer stencil channel. */
		SURFACE_STENCIL		= 4,
	};

	/** NS(Shader,ParamType) as String objects. */
	NS(lang,Array)<NS(lang,String)>		shaderparamname;
	
	/** Global list of all shader objects. */
	NS(lang,SingleLinkedList)<DX_ContextItem> itemlist;

	/** Transform buffer for transformations needed in polygon sorting. */
	NS(lang,Array)<NS(math,float4x4)>		boneworldtm;

	/** Cached projection transform. */
	NS(math,float4x4)					projtm;

	/** Cached model-world transform, used for sorting. */
	NS(math,float4x4)					worldtm;

	/** Cached world space camera position, used for sorting. */
	NS(math,float4)					camerapos;

	/** Temporary buffer for polygon sorting. */
	SortBuffer						sortbuffer;

	/** 
	 * Initializes the device object that renders to current active window.
	 * @param width Width of the frame buffer.
	 * @param height Height of the frame buffer.
	 * @param bitsperpixel Requested bits per pixel.
	 * @param win Window type, fullscreen or desktop.
	 * @param rz Rasterizer type.
	 * @param vp Vertex processing type.
	 * @param buffers Frame buffer surface type. See SurfaceFlags.
	 * @exception DX_GraphicsException
	 */
	DX_Context( int width, int height, int bitsperpixel,
		WindowType win, RasterizerType rz, VertexProcessingType vp, int buffers );

	///
	~DX_Context();

	/**
	 * Not supported by DirectX 9.
	 */
	Palette*	createPalette( int entries );

	/**
	 * Creates context dependent geometry primitive.
	 */
	Primitive* 	createPrimitive( Primitive::PrimType prim, const VertexFormat& vf, int vertices, int indices, UsageFlags usage );

	/**
	 * Gets context dependent dynamic geometry primitive.
	 */
	Primitive* 	getDynamicPrimitive( Primitive::PrimType prim, const VertexFormat& vf, int vertices, int indices );

	/**
	 * Creates a DX texture.
	 */
	Texture*	createTexture( int width, int height, const SurfaceFormat& fmt, Palette* pal, int usageflags );

	/**
	 * Creates context dependent texture from image file.
	 * @param filename Image file name.
	 * @exception IOException
	 * @exception GraphicsException
	 */
	Texture*	createTexture( const NS(lang,String)& filename );

	/**
	 * Creates context dependent cube texture from image file.
	 * @param filename Image file name.
	 * @exception IOException
	 * @exception GraphicsException
	 */
	CubeTexture* createCubeTexture( const NS(lang,String)& filename );

	/**
	 * Creates context dependent shader by name.
	 * Exact meaning of the shader name is platform dependent,
	 * but same names can be used in all platforms.
	 * @exception GraphicsException
	 */
	Shader*		createShader( const NS(lang,String)& name, int flags );

	/** 
	 * Called before beginning scene rendering.
	 * Don't use this directly, but exception-safe RenderScene wrapper instead.
	 */
	void		beginScene();

	/** 
	 * Called after scene rendering.
	 * Don't use this directly, but exception-safe RenderScene wrapper instead.
	 */
	void		endScene();

	/** 
	 * Clears viewport on active render target.
	 * Don't need to be called for back buffer since present() takes care of this.
	 */
	void		clear( int color );

	/** 
	 * Swaps back buffer to screen and clears viewport.
	 */
	void		present( int color );

	/**
	 * Sets perspective projection.
	 * @param hfov Horizontal field-of-view
	 * @param front Front/near plane distance
	 * @param back Back/far plane distance
	 * @param aspect Viewport aspect ratio (w/h)
	 */
	void		setPerspectiveProjection( float hfov, float front, float back, float aspect );

	/**
	 * Sets orthographic projection.
	 */
	void		setOrthographicProjection( bool enabled );

	/**
	 * Returns true if device can be used for rendering.
	 * Call once per frame before using device for rendering.
	 */
	bool		ready();

	/** 
	 * Sets viewport on active render target.
	 */
	void		setViewport( const Rect& rect );

	/**
	 * Sets render target. Render target texture must be created with USAGE_RENDERTARGET
	 * flag set.
	 * @param dst Render target texture or 0 if back buffer should be re-activated.
	 */
	void		setRenderTarget( Texture* dst );

	/**
	 * Stretches rectangle from back buffer to specified render target texture.
	 * Destination texture needs to have USAGE_RENDERTARGET flag set in creation.
	 */
	//void		stretchRect( const Rect& srcrect, Texture* dst, const Rect& dstrect, FilterType filter );

	/**
	 * Sets device sorting mode.
	 */
	void		setSort( Shader::SortType sort )		{m_sort=sort;}

	/** Prints context dependent objects to debug output. */
	void		printObjects();

	/**
	 * Captures back buffer contents to a file.
	 * @param namefmt printf compatible format string of output file name.
	 * @exception GraphicsException
	 * @exception IOException
	 */
	void		capture( const NS(lang,String)& namefmt );

	/**
	 * Returns screen orientation. Default is ORIENTATION_0.
	 */
	OrientationType	orientation() const;

	/**
	 * Returns screen buffer width.
	 */
	int			width() const;

	/**
	 * Returns screen buffer height.
	 */
	int			height() const;

	/**
	 * Returns surface format of the back buffer.
	 */
	SurfaceFormat	surfaceFormat() const;

	/**
	 * Returns current active viewport of the device.
	 */
	const Rect&		viewport() const;

	PlatformType	platform() const;

	/**
	 * Returns view->screen transformation (including screen transform).
	 */
	const NS(math,float4x4)&	projectionTransform() const;

	/** Returns Direct3D Device. */
	IDirect3DDevice9*		device() const				{return m_device;}

	/** Returns DirectX 9 caps. */
	D3DCAPS9				caps() const				{return m_caps;}

	/** Returns true if hardware shaders supported. */
	bool					shaderHardware() const		{return m_caps.PixelShaderVersion>0x100;}

	/** Returns true if shader debugging is enabled. */
	bool					debugShaders() const		{return m_debugshaders;}

	/** Returns device requested sorting mode which should be used when rendering primitives. */
	Shader::SortType		sort() const				{return m_sort;}

private:
	/** 
	 * Error codes for the context initialization.
	 */
	enum Error
	{
		/** No error. */
		ERROR_NONE,
		/** Operation failed for unknown reason. */
		ERROR_GENERIC,
		/** Failed to create main DirectX object. */
		ERROR_DIRECTXNOTINSTALLED,
		/** No compatible depth buffer format. */
		ERROR_NODEPTH,
		/** No compatible stencil buffer format. */
		ERROR_NOSTENCIL,
		/** Cannot create stencil buffer without depth buffer. */
		ERROR_NOSTENCILWITHOUTSDEPTH,
		/** Failed to initialize full-screen window mode. */
		ERROR_FULLSCREENINITFAILED,
		/** Failed to initialize desktop window mode. */
		ERROR_DESKTOPINITFAILED,
		/** A window must be active before create() is called. */
		ERROR_NOACTIVEWINDOW,
		/** Effect description compilation failed. */
		ERROR_EFFECTCOMPILATIONERROR,
		/** Effect cannot be rendered on current device. */
		ERROR_EFFECTUNSUPPORTED,
		/** Palettized textures not supported. */
		ERROR_PALETTETEXTURESNOTSUPPORTED,
	};

	IDirect3D9*				m_d3d;
	IDirect3DDevice9*		m_device;
	NS(math,float4x4)			m_projtm;
	int						m_buffers;
	Rect					m_viewport;
	int						m_renderwidth;
	int						m_renderheight;
	SurfaceFormat			m_bbtargetfmt;
	D3DDISPLAYMODE			m_displaymode;
	D3DPRESENT_PARAMETERS	m_present;
	D3DCAPS9				m_caps;
	NS(lang,Hashtable)<NS(lang,String),P(NS(gr,Shader))>	m_shaders;
	NS(lang,Array)<P(DX_Primitive)>				m_dynamicPrimitives;
	int						m_capturenum;
	Shader::SortType		m_sort;
	bool					m_debugshaders;
	bool					m_lockablebuffer;

	static void			error( Error err, const char* devicedesc );
	static const char*	getErrorString( Error err );
	Error				init( int width, int height, int bitsperpixel, WindowType win, RasterizerType rz, VertexProcessingType vp, int buffers );
	void				deinit();

	DX_Context( const DX_Context& );
	DX_Context& operator=( const DX_Context& );
};


END_NAMESPACE() // gr


#endif // _GR_DX_CONTEXT_H

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
