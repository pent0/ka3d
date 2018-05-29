#ifndef _GR_DX_TEXTURE_H
#define _GR_DX_TEXTURE_H


#include "DX_common.h"
#include "DX_ContextItem.h"
#include <gr/Texture.h>
#include <gr/SurfaceFormat.h>
#include <lang/String.h>
#include <stdint.h>


BEGIN_NAMESPACE(gr) 


class DX_Context;


/**
 * DirectX 2D texture.
 */
class DX_Texture :
	public Texture,
	public DX_ContextItem
{
public:
	/**
	 * Initializes the texture.
	 */
	DX_Texture( DX_Context* context, int width, int height, const SurfaceFormat& fmt, int usageflags );

	/**
	 * Initializes the texture from file.
	 */
	explicit DX_Texture( DX_Context* context, const NS(lang,String)& filename );

	///
	~DX_Texture();

	/**
	 * Locks object for specified access. Default behaviour just returns the parameter,
	 * which in effect means that any object property can be accessed any time.
	 * Don't use this method directly, but use Lock handle/wrapper instead,
	 * which is exception-safe.
	 */
	void		lock( LockType lock );

	/**
	 * Releases access to object.
	 * Don't use this method directly, but use Lock handle/wrapper instead,
	 * which is exception-safe.
	 */
	void		unlock();

	/**
	 * Called before reseting device.
	 */
	void	deviceLost();

	/**
	 * Called after reseting device.
	 */
	void	deviceReset();

	/**
	 * Sets all pixels of this texture image.
	 * Texture must be unlocked before the method is called.
	 * @param x Destination offset x.
	 * @param y Destination offset y.
	 * @param data Pointer to source image data.
	 * @param w Width of source image in pixels.
	 * @param h Height of source image in pixels.
	 * @param pitch Distance in bytes from start of source image scanline to start of next scanline.
	 * @param fmt Pixel format of source image.
	 * @param pal Palette of source image (if any).
	 * @param palfmt Palette format of source image (or SURFACE_UNKNOWN).
	 */
	void	blt( int x, int y,
				const void* data, int pitch, int w, int h, const SurfaceFormat& fmt,
				const void* pal, const SurfaceFormat& palfmt );

	/**
	 * Clears texture surface.
	 * Texture must be unlocked before the method is called.
	 */
	void	clear();

	/**
	 * Returns texture top level surface width in pixels.
	 */
	int		width() const;

	/**
	 * Returns texture top level surface height in pixels.
	 */
	int		height() const;

	/**
	 * Returns area of the texture surface.
	 */
	Rect	rect() const;

	/**
	 * Returns surface pixel storage format of this texture.
	 */
	SurfaceFormat 	format() const;

	/**
	 * Returns object string description.
	 */
	NS(lang,String)	toString() const;

	/** 
	 * Returns current lock state of the object.
	 * Default state is LOCK_READWRITE (=object can be read
	 * or written any time).
	 */
	LockType		locked() const;

	/**
	 * Returns access to locked data. Requires that texture is locked before calling this.
	 */
	void			getData( void** bits, int* pitch ) const;

	/**
	 * Returns Direct3DTexture
	 */
	IDirect3DTexture9*	texture() const			{return m_tex;}

private:
	IDirect3DTexture9*	m_tex;
	NS(lang,String)		m_filename;
	D3DLOCKED_RECT		m_lockedRect;
	uint16_t			m_width;
	uint16_t			m_height;
	uint8_t				m_usageflags;
	SurfaceFormat		m_format;
	LockType			m_locked;

	void	create( const NS(lang,String)& filename );
	void	create( int width, int height, const SurfaceFormat& fmt, int usageflags );
	void	validateDesc( const NS(lang,String)& filename, int usageflags );

	DX_Texture( const DX_Texture& );
	DX_Texture& operator=( const DX_Texture& );
};

	
END_NAMESPACE() // gr


#endif // _GR_DX_TEXTURE_H

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
