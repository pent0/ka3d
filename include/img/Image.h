#ifndef _IMG_IMAGE_H
#define _IMG_IMAGE_H


#include <gr/SurfaceFormat.h>
#include <lang/Array.h>
#include <lang/String.h>
#include <lang/Object.h>
#include <stdint.h>


BEGIN_NAMESPACE(io) 
	class InputStream;END_NAMESPACE()

BEGIN_NAMESPACE(lang) 
	class String;END_NAMESPACE()


BEGIN_NAMESPACE(img) 


/**
 * Simple 32-bit A8R8G8B8 image.
 * @ingroup img
 */
class Image :
	public NS(lang,Object)
{	
public:
	/**
	 * Creates image explicitly. Default color is black.
	 */
	Image( int w, int h );

	/**
	 * Creates image from file.
	 */
	explicit Image( const NS(lang,String)& filename );

	/**
	 * Sets image pixel to specified color.
	 */
	void	setPixel( int x, int y, uint32_t color );

	/**
	 * Draws line using specified A8R8G8B color.
	 */
	void	drawLine( int x0, int y0, int x1, int y1, uint32_t color );

	/**
	 * Draws circle of specified radius.
	 */
	void	drawCircle( int originx, int originy, int radius, uint32_t color );

	/**
	 * Draws textured triangle from 16-bit X4R4G4B4 texture.
	 */
	void	drawTexturedTriangle( 
				int x1, int y1, int x2, int y2, int x3, int y3,
				int u1, int v1, int u2, int v2, int u3, int v3,
				const uint16_t* tex, int texwidth, int texheight, int texpitch );

	/**
	 * Saves image using specified name.
	 */
	void	save( const NS(lang,String)& filename );

	/**
	 * Returns access to pixel data.
	 */
	const uint32_t*		bits()		{return &m_bits[0];}

	/**
	 * Reads image pixel at specified coordinates. Top left is (0,0).
	 */
	uint32_t	getPixel( int x, int y ) const;

	/**
	 * Returns image width in pixels.
	 */
	int		width() const		{return m_width;}

	/**
	 * Returns image height in pixels.
	 */
	int		height() const		{return m_height;}

	/**
	 * Returns A8R8G8B8.
	 */
	NS(gr,SurfaceFormat)	format() const		{return NS(gr,SurfaceFormat)::SURFACE_A8R8G8B8;}

	/**
	 * Returns access to pixel data.
	 */
	const uint32_t*			bits() const		{return &m_bits[0];}

	/**
	 * Returns distance between two image pixel rows in bytes.
	 */
	int						pitch() const		{return m_width<<2;}

private:
	NS(lang,Array)<uint32_t>	m_bits;
	int							m_width;
	int							m_height;
};


END_NAMESPACE() // img


#endif // _IMG_IMAGE_H

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
