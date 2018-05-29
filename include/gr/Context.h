#ifndef _GR_CONTEXT_H
#define _GR_CONTEXT_H


#include <gr/Shader.h>
#include <gr/Primitive.h>
#include <gr/SurfaceFormat.h>
#include <lang/Array.h>
#include <lang/String.h>
#include <lang/Object.h>
#include <math/float3x4.h>


BEGIN_NAMESPACE(math) 
	class float4x4;END_NAMESPACE()


BEGIN_NAMESPACE(gr) 


class Rect;
class Palette;
class Texture;
class Primitive;
class CubeTexture;
class VertexFormat;
	

/** 
 * Base class for platform dependent rendering device contexts.
 * Manages rendering device state and rendering device dependent 
 * resources like textures and vertex buffers.
 * @ingroup gr
 */
class Context :
	public NS(lang,Object)
{
public:
	/**
	 * Rendering platform id
	 */
	enum PlatformType
	{
		/** Platform is DirectX. */
		PLATFORM_DX,
		/** Platform is OpenGL ES. */
		PLATFORM_EGL,
		/** Platform is Playstation Portable. */
		PLATFORM_PSP,
		/** Platform is software pipeline. */
		PLATFORM_SW,
		/** Platform is N3D. */
		PLATFORM_N3D,
		/** Number of platform types. */
		PLATFORM_COUNT
	};

	/** Filtering type for image scaling. */
	enum FilterType
	{
		/** Filter disabled. */
		FILTER_NONE,
		/** Point filter. */
		FILTER_POINT,
		/** Linear filter. */
		FILTER_LINEAR
	};

	/** Resource usage flags. */
	enum UsageFlags
	{
		/** Use default options for resource usage. */
		USAGE_DEFAULT			= 0,
		/** Set for texture that needs to be used as render target. */
		USAGE_RENDERTARGET		= 1,
		/** Resource is relatively non-changing (not locked every frame). */
		USAGE_STATIC			= 2,
		/** Resource is locked frequently (every frame). */
		USAGE_DYNAMIC			= 4,
	};

	/** Screen orientation. */
	enum OrientationType
	{
		/** Screen is not rotated. */
		ORIENTATION_0,
		/** Screen is rotated 90 degrees counter-clockwise. */
		ORIENTATION_90,
		/** Screen is rotated 180 degrees counter-clockwise. */
		ORIENTATION_180,
		/** Screen is rotated 270 degrees counter-clockwise. */
		ORIENTATION_270,
	};

	/** Helper class for exception-safe usage of beginScene and endScene functions. */
	class RenderScene
	{
	public:
		/** Calls beginScene for the context. */
		explicit RenderScene( Context* context );
	
		/** Calls endScene for the context. */
		~RenderScene();
	
	private:
		P(Context) m_context;
	
		RenderScene( const RenderScene& );
		RenderScene& operator=( const RenderScene& );
	};

	/**
	 * Rendering context statistics.
	 * Note that all rendering context implementations might not
	 * update all statistical information.
	 */
	struct Statistics
	{
		/** Number of frames rendered. */
		int renderedFrames;
		/** Number of lines rendered since last reset. */
		int renderedLines;
		/** Number of points rendered since last reset. */
		int renderedPoints;
		/** Number of triangles rendered since last reset. */
		int renderedTriangles;
		/** Number of primitives rendered since last reset. */
		int renderedPrimitives;
		/** Allocated texture memory in bytes. */
		int allocatedTextureMemory;
		/** Allocated number of textures. */
		int allocatedTextures;

		/** Sets all statistics counters to 0. */
		void reset();
	};

	/** Rendering context statistics. */
	Statistics statistics;

	Context();
	~Context();

	/**
	 * Creates context dependent shader by name.
	 * Shader name consists of optional path prefix and
	 * actual shader name following '/' character.
	 * Path prefix is only meaningful if the shader hasn't been
	 * used before and if the platform supports external file base shaders.
	 * Exact meaning of the shader name is platform dependent,
	 * but same names can be used in all platforms.
	 * @param name Shader name.
	 * @param flags Compilation flags. See NS(Shader,Flags).
	 * @exception GraphicsException
	 */
	virtual Shader*		createShader( const NS(lang,String)& name, int flags=0 ) = 0;

	/**
	 * Creates context dependent palette.
	 * @param entries Number of entries in the palette. Must be either 16 or 256.
	 * @exception GraphicsException
	 */
	virtual Palette*	createPalette( int entries ) = 0;

	/**
	 * Creates context dependent geometry primitive.
	 * @param prim Primitive type.
	 * @param vf Geometry vertex format.
	 * @param vertices Number of vertices.
	 * @param indices Number of indices.
	 * @param usage Intended usage. Select USAGE_DYNAMIC if the buffer is locked in every frame. Default is USAGE_STATIC.
	 * @exception GraphicsException
	 */
	virtual Primitive* 	createPrimitive( Primitive::PrimType prim, const VertexFormat& vf, int vertices, int indices, UsageFlags usage=USAGE_STATIC ) = 0;

	/**
	 * Gets context dependent dynamic geometry primitive.
	 * Dynamic primitives need to be re-requested always before rendering.
	 * @param prim Primitive type.
	 * @param vf Geometry vertex format.
	 * @param vertices Number of vertices.
	 * @param indices Number of indices.
	 * @exception GraphicsException
	 */
	virtual Primitive* 	getDynamicPrimitive( Primitive::PrimType prim, const VertexFormat& vf, int vertices, int indices ) = 0;

	/**
	 * Creates context dependent texture.
	 * @param width Width of the texture in pixels.
	 * @param height Height of the texture in pixels.
	 * @param fmt Pixel format of the texture.
	 * @param pal Palette if fmt is palettized pixel format.
	 * @param usageflags Texture usage flags. See UsageFlags.
	 * @exception GraphicsException
	 */
	virtual Texture*	createTexture( int width, int height, const SurfaceFormat& fmt, Palette* pal, int usageflags ) = 0;

	/**
	 * Creates context dependent texture from image file.
	 * @param filename Image file name.
	 * @exception IOException
	 * @exception GraphicsException
	 */
	virtual Texture*	createTexture( const NS(lang,String)& filename ) = 0;

	/**
	 * Creates context dependent cube texture from image file.
	 * @param filename Image file name.
	 * @exception IOException
	 * @exception GraphicsException
	 */
	virtual CubeTexture* createCubeTexture( const NS(lang,String)& filename ) = 0;

	/**
	 * Stretches rectangle from back buffer to specified render target texture.
	 * Destination texture needs to have USAGE_RENDERTARGET flag set in creation.
	 */
	//virtual void		stretchRect( const Rect& srcrect, Texture* dst, const Rect& dstrect, FilterType filter ) = 0;

	/** 
	 * Sets viewport on active render target.
	 */
	virtual void		setViewport( const Rect& rect ) = 0;

	/**
	 * Sets perspective projection.
	 * @param hfov Horizontal field-of-view
	 * @param front Front/near plane distance
	 * @param back Back/far plane distance
	 * @param aspect Viewport aspect ratio (w/h)
	 */
	virtual void		setPerspectiveProjection( float hfov, float front, float back, float aspect ) = 0;

	/**
	 * Sets orthographic projection.
	 */
	virtual void		setOrthographicProjection( bool enabled ) = 0;

	/**
	 * Sets render target. Render target texture must be created with USAGE_RENDERTARGET
	 * flag set.
	 * @param dst Render target texture or 0 if back buffer should be re-activated.
	 */
	virtual void		setRenderTarget( Texture* dst ) = 0;

	/** 
	 * Sets wireframe rendering enabled/disabled. Debug usage only.
	 * Note that all platform might not support this function (nothing happens).
	 */
	virtual void		setWireframeEnabled( bool enabled );

	/** 
	 * Called before beginning scene rendering.
	 * Don't use this directly, but exception-safe NS(Context,RenderScene) wrapper instead.
	 */
	virtual void		beginScene() = 0;

	/** 
	 * Called after scene rendering.
	 * Don't use this directly, but exception-safe NS(Context,RenderScene) wrapper instead.
	 */
	virtual void		endScene() = 0;

	/** 
	 * Clears viewport on active render target.
	 * Don't need to be called for back buffer since present() takes care of this.
	 * @param color 32-bit ARGB color
	 */
	virtual void		clear( int color=0 ) = 0;

	/** 
	 * Swaps back buffer to screen and clears viewport.
	 * @param color 32-bit ARGB color
	 */
	virtual void		present( int color=0 ) = 0;

	/**
	 * Returns true if device can be used for rendering.
	 * Call once per frame before using device for rendering.
	 */
	virtual bool		ready() = 0;

	/**
	 * Captures back buffer contents to a file.
	 * List of supported formats is platform dependent.
	 * Currently Win32/DirectX supports BMP, JPG, TGA, PNG, DDS, PPM, DIB, HDR and PFM.
	 * @param namefmt printf compatible format string of output file name.
	 * @exception GraphicsException
	 * @exception IOException
	 */
	virtual void		capture( const NS(lang,String)& namefmt ) = 0;

	/**
	 * Returns screen buffer width.
	 */
	virtual int			width() const = 0;

	/**
	 * Returns screen buffer height.
	 */
	virtual int			height() const = 0;

	/**
	 * Returns surface format of the back buffer.
	 */
	virtual SurfaceFormat	surfaceFormat() const = 0;

	/**
	 * Returns current active viewport of the device.
	 */
	virtual const Rect&	viewport() const = 0;

	/**
	 * Returns view->screen transformation (including screen transform).
	 */
	virtual const NS(math,float4x4)&	projectionTransform() const = 0;

	/**
	 * Returns screen buffer width divided by height.
	 */
	float					aspect() const;

	/**
	 * Returns platform id.
	 */
	virtual PlatformType	platform() const = 0;

	/**
	 * Returns screen orientation. Default is ORIENTATION_0.
	 */
	virtual OrientationType	orientation() const = 0;

	/**
	 * Returns platform string id.
	 */
	const char*				platformString() const;

	/**
	 * Returns platform string id (max 3 characters).
	 */
	static const char*		getString( PlatformType platform );

	/**
	 * Returns platform description (can be used for in UIs etc).
	 */
	static const char*		getDescription( PlatformType platform );

private:
	Context( const Context& );
	Context& operator=( const Context& );
};


END_NAMESPACE() // gr


#endif // _GR_CONTEXT_H

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
