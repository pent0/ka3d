#ifndef _IMG_IMAGEREADER_H
#define _IMG_IMAGEREADER_H


#include <gr/SurfaceFormat.h>
#include <lang/Array.h>
#include <lang/Object.h>
#include <stdint.h>


BEGIN_NAMESPACE(io) 
	class InputStream;END_NAMESPACE()

BEGIN_NAMESPACE(lang) 
	class String;END_NAMESPACE()


BEGIN_NAMESPACE(img) 


/**
 * Low level image file reader.
 * @ingroup img
 */
class ImageReader :
	public NS(lang,Object)
{	
public:
	/**
	 * Supported image file formats.
	 */
	enum FileFormat
	{
		/** Unknown image file format. */
		FILEFORMAT_UNKNOWN,
		/** BMP image file. */
		FILEFORMAT_BMP,
		/** TGA image file. */
		FILEFORMAT_TGA,
		/** JPG image file. */
		FILEFORMAT_JPG,
		/** DDS image file. */
		FILEFORMAT_DDS,
		/** PSD image file. */
		FILEFORMAT_PSD,
		/** PNG image file. */
		FILEFORMAT_PNG,
		/** PCX image file. */
		FILEFORMAT_PCX,
		/** PNM image file. */
		FILEFORMAT_PNM,
		/** GIF image file. */
		FILEFORMAT_GIF,
		/** TIFF image file. */
		FILEFORMAT_TIF,
		/** NTX image file. */
		FILEFORMAT_NTX,
	};

	/** NTX file format header. Used by implementation. */
	struct NTXHeader
	{
		/** NTX image flags. */
		enum Flags
		{
			/** Enable color keying. */
			FLAG_COLORKEY	= 1,
			/** Enable compression. */
			FLAG_COMPRESS	= 2,
		};

		/** Version number of the file. Currently 0x102. */
		uint16_t	version;
		/** Width of image in pixels. */
		uint16_t	width;
		/** Height of image in pixels. */
		uint16_t	height;
		/** SurfaceFormat::SurfaceFormatType. Format of pixel data or palette data. If palettesize > 0 then pixel data is 8-bit indexed and this is format of the palette. */
		uint16_t	format;
		/** If this is > 0 then palette follows (precedes pixel data). Format of palette is defined by previous format field. */
		uint16_t	palettesize;
		/** See Flags. */
		uint16_t	flags;
		/** User defined flags. */
		uint16_t	userflags;
	};

	ImageReader();

	/**
	 * Starts reading image file using specified format.
	 * Note that ImageReader does not add a reference to InputStream,
	 * so you must make sure InputStream exists as long as the image is read from it.
	 * @exception IOException
	 */
	ImageReader( NS(io,InputStream)* in, FileFormat filefmt );

	///
	~ImageReader();

	/**
	 * Starts reading image file using specified format.
	 * Note that ImageReader does not add a reference to InputStream,
	 * so you must make sure InputStream exists as long as the image is read from it.
	 * @exception IOException
	 */
	void	reset( NS(io,InputStream)* in, FileFormat filefmt );

	/**
	 * Reads data of current active surface and iterates reading to next surface.
	 *
	 * Iteration order for cubemaps:
	 * <ol>
	 * <li>All mipmaps for +X surface
	 * <li>All mipmaps for -X surface
	 * <li>All mipmaps for +Y surface
	 * <li>All mipmaps for -Y surface
	 * <li>All mipmaps for +Z surface
	 * <li>All mipmaps for -Z surface
	 * <li>...
	 * </ol>
	 *
	 * @exception IOException
	 */
	void 				readSurface( void* bits, int pitch, int w, int h, NS(gr,SurfaceFormat) fmt,
							const void* pal, NS(gr,SurfaceFormat) palfmt );
	
	/**
	 * Returns total number of surfaces to be read.
	 * This includes all mipmap levels too,
	 * i.e. for cubemap with three mipmap levels surfaces() will be 18.
	 */
	int					surfaces() const;

	/**
	 * Returns index of current surface being read.
	 */
	int					surfaceIndex() const;

	/**
	 * Returns true if the image is cube map.
	 */
	bool				cubeMap() const;

	/**
	 * Returns number of mipmap levels in image.
	 * Minimum returned value is 1 (this level).
	 */
	int					mipLevels() const;

	/**
	 * Returns pixel format of the image.
	 */
	NS(gr,SurfaceFormat)	format() const;

	/**
	 * Returns pointer to palette data if any.
	 * Palette data is file format dependent.
	 * @return 0 if no palette.
	 */
	const void*			paletteData() const;

	/**
	 * Returns pixel format of the palette if any.
	 * @return SURFACE_UNKNOWN if palette data format is non-standard.
	 */
	NS(gr,SurfaceFormat)	paletteFormat() const;
	
	/**
	 * Returns width of current active surface in pixels.
	 */
	int					surfaceWidth() const;
	
	/**
	 * Returns height of current active surface in pixels.
	 */
	int					surfaceHeight() const;

	/**
	 * Returns true if color keying is enabled for this image.
	 */
	bool				colorKeyEnabled() const			{return m_colorKeyEnabled;}

	/**
	 * Guesses image file type based on file name.
	 * @return FILEFORMAT_UNKNOWN if format cannot be guessed from file name suffix.
	 */
	static FileFormat	guessFileFormat( const NS(lang,String)& filename );

	/*
	 * Reads Little-Endian 16-bit Unsigned Integer.
	 */
	static uint16_t		getUInt16LE( const void* data, int offset );

	/*
	 * Reads Little-Endian 32-bit Unsigned Integer.
	 */
	static uint32_t		getUInt32LE( const void* data, int offset );

private:
	class Surface
	{
	public:
		NS(lang,Array)<uint8_t>	data;
		int						width;
		int						height;
	};

	NS(io,InputStream)*		m_in;
	int						m_width;
	int						m_height;
	int						m_bitsPerPixel;
	int						m_pitch;
	int						m_mipLevels;
	int						m_surfaces;
	int						m_surfaceIndex;
	uint8_t					m_colormap[256][4];
	NS(lang,Array)<uint8_t>	m_scanlinebuffer;
	NS(gr,SurfaceFormat)	m_fmt;
	NS(gr,SurfaceFormat)	m_palfmt;
	FileFormat				m_filefmt;
	bool					m_bottomUp;
	bool					m_useRLE;
	bool					m_cubeMap;
	bool					m_colorKeyEnabled;
	uint32_t				m_ffbuffer[200]; // file format specific buffer
	NS(lang,Array)<Surface>	m_surfaceBuffer;

	static void			readFully( NS(io,InputStream)* in, void* buf, int bytes );
	static void			readColorMap( NS(io,InputStream)* in, int entrysize, int entries, uint8_t* colormap );

	void	readHeader_ntx();
	void	readHeader_bmp();
	void	readHeader_tga();
	void	readHeader_jpg();
	void	readHeader_png();
	void	readHeader_il();
	void 	readSurface_jpg( void* bits, int pitch, int w, int h, NS(gr,SurfaceFormat) fmt, const void* pal, NS(gr,SurfaceFormat) palfmt );
	void 	readSurfaceFromSurfaceBuffer( void* bits, int pitch, int w, int h, NS(gr,SurfaceFormat) fmt, const void* pal, NS(gr,SurfaceFormat) palfmt );
	void 	readScanlines( void* bits, int pitch, int w, int h, NS(gr,SurfaceFormat) fmt, const void* pal, NS(gr,SurfaceFormat) palfmt );
	void	readScanline_jpg( void* buffer );
	void	readScanline_tgaRLE( uint8_t* buffer );
	void	finish_jpg();

	ImageReader( const ImageReader& );
	ImageReader& operator=( const ImageReader& );
};


END_NAMESPACE() // img


#endif // _IMG_IMAGEREADER_H

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
