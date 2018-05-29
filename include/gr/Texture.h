#ifndef _GR_TEXTURE_H 
#define _GR_TEXTURE_H


#include <gr/Rect.h>
#include <gr/BaseTexture.h>
#include <gr/SurfaceFormat.h>


BEGIN_NAMESPACE(gr) 

	
/**
 * Base class for platform dependent 2D textures.
 * @ingroup gr
 */
class Texture :
	public BaseTexture
{
public:
	Texture();

	/**
	 * Sets all pixels of this texture image.
	 * Texture must be unlocked before the method is called.
	 * @param x Destination offset x.
	 * @param y Destination offset y.
	 * @param data Pointer to source image data.
	 * @param pitch Distance in bytes from start of source image scanline to start of next scanline.
	 * @param w Width of source image in pixels.
	 * @param h Height of source image in pixels.
	 * @param fmt Pixel format of source image.
	 * @param pal Palette of source image (if any).
	 * @param palfmt Palette format of source image (or SURFACE_UNKNOWN).
	 */
	virtual void	blt( int x, int y,
						const void* data, int pitch, int w, int h, const SurfaceFormat& fmt, 
						const void* pal, const SurfaceFormat& palfmt ) = 0;

	/**
	 * Clears texture surface.
	 * Texture must be unlocked before the method is called.
	 */
	virtual void			clear() = 0;

	/**
	 * Returns texture top level surface width in pixels.
	 */
	virtual int				width() const = 0;

	/**
	 * Returns texture top level surface height in pixels.
	 */
	virtual int				height() const = 0;

	/**
	 * Returns area of the texture surface.
	 */
	virtual Rect			rect() const = 0;

	/**
	 * Returns surface pixel storage format of this texture.
	 */
	virtual SurfaceFormat 	format() const = 0;

	/**
	 * Returns access to locked data. Requires that texture is locked before calling this.
	 */
	virtual void			getData( void** bits, int* pitch ) const = 0;
};


END_NAMESPACE() // gr


#endif // _GR_TEXTURE_H

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
