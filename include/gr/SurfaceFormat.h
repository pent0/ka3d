#ifndef _GR_SURFACEFORMAT_H
#define _GR_SURFACEFORMAT_H


#include <lang/pp.h>
#include <stdint.h>


BEGIN_NAMESPACE(gr) 


/** 
 * Description of surface pixel bit layout.
 * Supports also conversions between pixel formats.
 * 
 * @ingroup gr
 */
class SurfaceFormat
{
public:
	/** 
	 * Type of surface pixel format.
	 */
	enum SurfaceFormatType
	{
		/** The surface format is unknown. */
		SURFACE_UNKNOWN,
		/** 24-bit RGB pixel format. */
		SURFACE_R8G8B8,
		/** 24-bit RGB pixel format. */
		SURFACE_B8G8R8,
		/** 32-bit RGB pixel format with alpha. */
		SURFACE_A8R8G8B8,
		/** 32-bit RGB pixel format where 8 bits are reserved for each color. */
		SURFACE_X8R8G8B8,
		/** 32-bit RGB pixel format where 8 bits are reserved for each color.  */
		SURFACE_X8B8G8R8,
		/** 32-bit RGB pixel format with alpha.  */
		SURFACE_A8B8G8R8,
		/** 16-bit RGB pixel format. (PS2) */
		SURFACE_R5G6B5,
		/** 16-bit RGB pixel format. */
		SURFACE_R5G5B5,
		/** 18-bit RGB pixel format (32bits per pixel). */
		SURFACE_R6G6B6,
		/** 4-bit palettized pixel format. (PC/PS2) */
		SURFACE_P4,
		/** 8-bit palettized pixel format. (PC/PS2)	*/
		SURFACE_P8,
		/** 8-bit luminosity format. (PC/PS2) */
		SURFACE_L8,
		/** 16-bit pixel format where 5 bits are reserved for color and 1 bit is reserved for transparency. */
		SURFACE_A1R5G5B5,
		/** 16-bit RGB pixel format where 4 bits are reserved for each color. */
		SURFACE_X4R4G4B4,
		/** 16-bit RGBA pixel format. */
		SURFACE_A4R4G4B4,
		/** 16-bit RGBA pixel format. */
		SURFACE_A4B4G4R4,
		/** 16-bit RGBA pixel format. */
		SURFACE_R4G4B4A4,
		/** 16-bit pixel format where 5 bits are reserved for color and 1 bit is reserved for transparency. */
		SURFACE_A1B5G5R5,
		/** 16-bit pixel format where 5 bits are reserved for color and 1 bit is reserved for transparency. */
		SURFACE_R5G5B5A1,
		/** 8-bit RGB texture format. (PS2) */
		SURFACE_R3G3B2,
		/** 8-bit RGB texture format. */
		SURFACE_R3G2B3,
		/** 8-bit alpha-only. */
		SURFACE_A8,
		/** 16-bit RGB pixel format with alpha.	*/
		SURFACE_A8R3G3B2,
		/** 16-bit RGB pixel format with alpha.	*/
		SURFACE_A8R3G2B3,
		/** DirectX compressed texture */
		SURFACE_DXT1,
		/** DirectX compressed texture */
		SURFACE_DXT3,
		/** DirectX compressed texture */
		SURFACE_DXT5,
		/** 16-bit float format, 16 bits red channel. */
		SURFACE_R16F,
		/** 32-bit float format, 16 bits red and green channels. */
		SURFACE_G16R16F,
		/** 64-bit float format, 16 bits for the alpha, blue, green, red. */
		SURFACE_A16B16G16R16F,
		/** 32-bit float format, 32 bits red channel. */
		SURFACE_R32F,
		/** 64-bit float format, 32 bits red and green channels. */
		SURFACE_G32R32F,
		/** 128-bit float format, 32 bits for the alpha, blue, green, red. */
		SURFACE_A32B32G32R32F,
		/** 32-bit depth buffer format */
		SURFACE_D32,
		/** 24-bit depth buffer format */
		SURFACE_D24,
		/** 16-bit depth buffer format */
		SURFACE_D16,
		/** 32-bit depth buffer format, depth using 24 bits and stencil 8 bits */
		SURFACE_D24S8,
		/** Surface format list terminator */
		SURFACE_LAST,
	};

	/**
	 * Creates surface format of SURFACE_UNKNOWN type.
	 */
	SurfaceFormat();

	/**
	 * Creates surface format from string description.
	 * @param desc Format description, e.g. R8G8B8.
	 * @exception GraphicsException If format unknown.
	 */
	explicit SurfaceFormat( const char* desc );

	/**
	 * Creates surface format from enumerated pixel format type. 
	 */
	SurfaceFormat( SurfaceFormatType type );

	/**
	 * Creates a surface format from bit count and 4 masks. 
	 * Format is set to SURFACE_UNKNOWN if match not found.
	 */
	SurfaceFormat( int bitCount, uint32_t redMask, uint32_t greenMask, uint32_t blueMask, uint32_t alphaMask );

	/**
	 * Returns type of surface format. 
	 */
	SurfaceFormatType	type() const;

	/** 
	 * Returns number of bits per pixel in the surface format.
	 * Returns 0 if the format is unknown or compressed.
	 */
	int			bitsPerPixel() const;

	/**
	 * Returns true if the pixels of this format have alpha channel information. 
	 */
	bool		hasAlpha() const;

	/** 
	 * Returns ith channel bit mask. 
	 * Order of channel indices are (r,g,b,a).
	 * If the format does not have specified channel then the return value is 0.
	 */
	uint32_t 	getChannelMask( int i ) const;

	/** 
	 * Returns ith channel bit count. 
	 * Order of channel indices are (r,g,b,a).
	 * If the format does not have specified channel then the return value is 0.
	 */
	int			getChannelBitCount( int i ) const;

	/**
	 * Returns number of palette entries in this pixel format.
	 * @return Number of palette entries or 0 if the format is not palettized.
	 */
	int 		paletteEntries() const;

	/**
	 * Returns true if the formats are the same. 
	 */
	bool		operator==( const SurfaceFormat& other ) const			{return m_type==other.m_type;}

	/**
	 * Returns true if the formats are different. 
	 */
	bool		operator!=( const SurfaceFormat& other ) const			{return m_type!=other.m_type;}

	/**
	 * Returns true if this is DXT compressed pixel format. 
	 */
	bool		compressed() const		{return m_type >= SURFACE_DXT1 && m_type <= SURFACE_DXT5;}

	/**
	 * Returns true if this is HDR (float) pixel format.
	 */
	bool		hdr() const				{return m_type >= SURFACE_R16F && m_type <= SURFACE_A32B32G32R32F;}

	/**
	 * Returns true if this is palettized pixel format. 
	 */
	bool		palettized() const		{return m_type == SURFACE_P8 || m_type == SURFACE_P4;}

	/** 
	 * Copies row of pixels from one surface pixel format to another.
	 * Only valid for non-compressed formats. Supports HDR formats.
	 *
	 * Usage example: (copy pixels from format RGBA8888 to RGB565)
	 * <pre>
	   SurfaceFormat dstf( SURFACE_R5G6B5 );
	   dstf.copyPixels( dst, SURFACE_A8R8G8B8, src, pixels );
	   </pre>
	 */
	void		copyPixels( void* dst, const SurfaceFormat& dstpalfmt, const void* dstpal,
					const SurfaceFormat& srcfmt, const void* src, const SurfaceFormat& srcpalfmt, const void* srcpal, 
					int pixels ) const;

	/** 
	 * Copies rectangle of pixels from one surface pixel format to another.
	 * Supports also DXT-compressed formats as source data.
	 */
	void		copyPixels( void* dst, int dstpitch, const SurfaceFormat& dstpalfmt, const void* dstpal,
					const SurfaceFormat& srcfmt, const void* src, int srcpitch, const SurfaceFormat& srcpalfmt, const void* srcpal,
					int width, int height ) const;

	/** 
	 * Returns RGBA pixel as 4-vector at specified coordinate.
	 * Supports also DXT-compressed formats as source data.
	 * Opaque white is (1,1,1,1).
	 */
	void		getPixel( int x, int y,
						const void* data, int pitch, 
						const SurfaceFormat& palfmt, const void* pal,
						float* rgba ) const;

	/** Returns memory usage of a surface of specified size. */
	int			getMemoryUsage( int w, int h ) const;

	/** Returns string description of the format. */
	const char*	toString() const;

private:
	SurfaceFormatType m_type;
};


END_NAMESPACE() // gr


#endif // _GR_SURFACEFORMAT_H

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
