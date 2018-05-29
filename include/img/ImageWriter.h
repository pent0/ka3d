#ifndef _IMG_IMAGEWRITER_H
#define _IMG_IMAGEWRITER_H


#include <gr/SurfaceFormat.h>
#include <img/ImageReader.h>
#include <lang/Array.h>
#include <lang/Object.h>
#include <stdint.h>


BEGIN_NAMESPACE(lang) 
	class String;END_NAMESPACE()


BEGIN_NAMESPACE(img) 


/**
 * Image file writer.
 * @ingroup img
 */
class ImageWriter :
	public NS(lang,Object)
{	
public:
	/** NTX file header flags. */
	enum NTXFlags
	{
		/** Enable color keying. */
		NTX_COLORKEY	= ImageReader::NTXHeader::FLAG_COLORKEY,
		/** Enable Huffman-compression. */
		NTX_COMPRESS	= ImageReader::NTXHeader::FLAG_COMPRESS,
	};

	/**
	 * Saves image as NTX file.
	 * @param flags User defined flags to store to the file.
	 * @param userflags User defined flags. Doesn't affect the image in any way.
	 * @exception IOException
	 * @return Number of bytes written to the stream.
	 */
	static int	writeNTX( const NS(lang,String)& filename,
					const void* bits, int width, int height, int pitch, NS(gr,SurfaceFormat) format,
					int flags, int userflags );

	/**
	 * Saves image as PNG file.
	 * @exception IOException
	 */
	static void	writePNG( const NS(lang,String)& filename,
					const void* bits, int width, int height, int pitch,
					NS(gr,SurfaceFormat) srcformat, const void* srcpal, NS(gr,SurfaceFormat) srcpalformat );

	/**
	 * Saves image file and tries to guess format by filename.
	 * @exception IOException
	 */
	static void	write( const NS(lang,String)& filename, NS(gr,SurfaceFormat) dstformat,
					const void* bits, int width, int height, int pitch,
					NS(gr,SurfaceFormat) srcformat, const void* srcpal, NS(gr,SurfaceFormat) srcpalformat );


	/* Reads 1-4 bytes in Little-Endian byte order. */
	static uint32_t		getBytesLE( const uint8_t* src, int bytesperpixel );

	/* Writes 1-4 bytes in Little-Endian byte order. */
	static void			setBytesLE( uint8_t* dst, uint32_t c, int bytesperpixel );
};


END_NAMESPACE() // img


#endif // _IMG_IMAGEWRITER_H

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
