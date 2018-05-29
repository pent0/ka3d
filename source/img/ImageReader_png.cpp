#ifdef IMG_PNG_SUPPORT

#include <img/ImageReader.h>
#include <io/IOException.h>
#include <io/FileInputStream.h>
#include <string.h>
#include <stdint.h>
#include "libpng-1.2.5/png.h"
#include <config.h>


USING_NAMESPACE(io)
USING_NAMESPACE(gr)
USING_NAMESPACE(lang)


BEGIN_NAMESPACE(img) 


static void readfunc( png_structp pngptr, png_bytep data, png_size_t length )
{
	InputStream* in = reinterpret_cast<InputStream*>( png_get_io_ptr(pngptr) );
	assert( in != 0 );
	in->read( data, length );
}

static void errorhandler( struct png_struct_def*, const char* )
{
}

static void warninghandler( struct png_struct_def*, const char* )
{
}

void ImageReader::readHeader_png()
{
	// BUG: overwrites some memory?? crashes zaxdemo etc.

	png_structp pngptr = png_create_read_struct( PNG_LIBPNG_VER_STRING, 0, errorhandler, warninghandler );
	if ( !pngptr )
		throwError( IOException( Format("png_create_read_struct failed while loading \"{0}\"", m_in->toString()) ) );

	png_infop infoptr = png_create_info_struct( pngptr );
	if ( !infoptr )
	{
		png_destroy_read_struct( &pngptr, 0, 0 );
		throwError( IOException( Format("png_create_info_struct failed while loading \"{0}\"", m_in->toString()) ) );
	}

	png_set_read_fn( pngptr, m_in, readfunc );
	png_set_error_fn( pngptr, 0, errorhandler, warninghandler );
	//png_set_sig_bytes( pngptr, 8 );
	png_read_info( pngptr, infoptr );

	png_uint_32 w, h;
	int bitspp, colortype;
	png_get_IHDR( pngptr, infoptr, &w, &h, &bitspp, &colortype, 0, 0, 0 );
	// expand gray scale & 1-4 bitsperpixel -> 8 bit indexed
	if ( PNG_COLOR_TYPE_GRAY == colortype || bitspp < 8 )
		png_set_gray_1_2_4_to_8( pngptr );
	// refresh info
	png_get_IHDR( pngptr, infoptr, &w, &h, &bitspp, &colortype, 0, 0, 0 );
	
	m_width = w;
	m_height = h;
	m_bitsPerPixel = bitspp;
	m_pitch = w*bitspp >> 3;
	m_mipLevels = 1;
	m_surfaces = 1;

	switch ( bitspp )
	{
	case 8:
		m_fmt = SurfaceFormat::SURFACE_P8;
		m_palfmt = SurfaceFormat::SURFACE_A8R8G8B8;
		break;
	case 24:
		m_fmt = SurfaceFormat::SURFACE_A8R8G8B8;
		break;
	}

	// read color map
	if ( PNG_COLOR_TYPE_PALETTE == colortype )
	{
		memset( m_colormap, 0xFF, sizeof(m_colormap) );

		png_colorp pal;
		int numpal;
		png_get_PLTE( pngptr, infoptr, &pal, &numpal );
		assert( numpal <= 256 );

		for ( int i = 0 ; i < numpal ; ++i )
		{
			m_colormap[i][0] = pal[i].blue;
			m_colormap[i][1] = pal[i].green;
			m_colormap[i][2] = pal[i].red;
		}
	}

	// read transparency
	assert( !(png_get_valid(pngptr, infoptr, PNG_INFO_tRNS) && bitspp > 8) ); // weird combo?? bpp>8 and still tRNS chunk?
	if ( png_get_valid(pngptr, infoptr, PNG_INFO_tRNS) && bitspp > 8 )
		png_set_tRNS_to_alpha( pngptr );
	if ( png_get_valid(pngptr, infoptr, PNG_INFO_tRNS) && bitspp <= 8 )
	{
		png_bytep trans = 0;
		int numtrans = 0;
		png_color_16p transvalues = 0;
		png_get_tRNS( pngptr, infoptr, &trans, &numtrans, &transvalues );
		assert( numtrans <= 256 );
	
		for ( int i = 0 ; i < numtrans ; ++i )
			m_colormap[i][3] = trans[i];
	}

	// read surface
	int channels = png_get_channels( pngptr, infoptr );
	Array<png_byte*> rowptrs( m_height );
	m_surfaceBuffer.resize( 1 );
	Surface& surface = m_surfaceBuffer[0];
	surface.width = m_width;
	surface.height = m_height;
	surface.data.resize( m_height*m_pitch );
	for ( int i = 0 ; i < m_height ; ++i )
		rowptrs[i] = &surface.data[m_pitch*i];
	png_read_image(pngptr, &rowptrs[0] );

	if ( pngptr && infoptr )
		png_destroy_read_struct( &pngptr, &infoptr, 0 );
}


END_NAMESPACE() // img

#else // !IMG_PNG_SUPPORT

#include <img/ImageReader.h>
#include <io/InputStream.h>
#include <io/IOException.h>

void img::ImageReader::readHeader_png()
{
	lang::throwError( io::IOException( lang::Format("PNG file support is not enabled (file {0})", m_in->toString()) ) );
}

#endif // IMG_PNG_SUPPORT

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
