#ifdef IMG_PNG_SUPPORT

#include <img/ImageWriter.h>
#include <gr/SurfaceFormat.h>
#include <io/IOException.h>
#include <io/FileOutputStream.h>
#include <lang/String.h>
#include "libpng-1.2.5/png.h"
#include <config.h>


USING_NAMESPACE(gr)
USING_NAMESPACE(io)
USING_NAMESPACE(lang)


BEGIN_NAMESPACE(img) 


static void writefunc( struct png_struct_def* pngptr, unsigned char* data, png_size_t size )
{
	OutputStream* out = reinterpret_cast<OutputStream*>( png_get_io_ptr(pngptr) );
	out->write( data, size );
}

static void flushfunc( png_structp )
{
}

static void errorhandler( struct png_struct_def*, const char* )
{
}

static void warninghandler( struct png_struct_def*, const char* )
{
}

void ImageWriter::writePNG( const String& filename, const void* bits, int width, int height, int pitch,
	SurfaceFormat srcformat, const void* srcpal, SurfaceFormat srcpalformat )
{
	FileOutputStream fileout( filename );
	OutputStream* out = &fileout;

	// write header
	png_structp pngptr = png_create_write_struct( PNG_LIBPNG_VER_STRING, (png_voidp)errorhandler, errorhandler, warninghandler );
	if ( !pngptr )
		throwError( IOException( Format("png_create_write_struct failed while saving \"{0}\" ({1}x{2}, srcformat {3})", out->toString(), width, height, srcformat.toString()) ) );

	png_infop infoptr = png_create_info_struct( pngptr );
	if ( !infoptr )
	{
		png_destroy_write_struct( &pngptr, 0 );
		throwError( IOException( Format("png_create_info_struct failed while saving \"{0}\" ({1}x{2}, format {3})", out->toString(), width, height, srcformat.toString()) ) );
	}

	png_set_write_fn( pngptr, out, writefunc, flushfunc );
	SurfaceFormat outformat = srcformat;
	png_colorp pngpal = 0;
	if ( srcformat.palettized() )
	{
		int pal[256];
		for ( int i = 0 ; i < 256 ; ++i )
			pal[i] = 0x800000+i;
		SurfaceFormat palfmt = SurfaceFormat(SurfaceFormat::SURFACE_A8R8G8B8);
		palfmt.copyPixels( pal, SurfaceFormat::SURFACE_UNKNOWN, 0, 
			srcpalformat, srcpal, SurfaceFormat::SURFACE_UNKNOWN, 0, srcformat.paletteEntries() );

		int palsize = 1 << srcformat.bitsPerPixel();
		pngpal = (png_colorp)png_malloc( pngptr, palsize*sizeof(png_color) );
		for ( int i = 0 ; i < palsize ; ++i )
		{
			pngpal[i].red	= (unsigned char)(pal[i]>>16);
			pngpal[i].green	= (unsigned char)(pal[i]>>8);
			pngpal[i].blue	= (unsigned char)(pal[i]);
		}

		png_set_IHDR( pngptr, infoptr, width, height, srcformat.bitsPerPixel(),
			PNG_COLOR_TYPE_PALETTE, PNG_INTERLACE_NONE, 
			PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE );

		png_set_PLTE( pngptr, infoptr, pngpal, palsize );

		if ( srcpalformat.hasAlpha() )
		{
			assert( palsize <= 256 );
			unsigned char trans[256];
			int transcount = palsize;
			for ( int i = 0 ; i < palsize ; ++i )
				trans[i] = (unsigned char)(pal[i]>>24);
			png_set_tRNS( pngptr, infoptr, trans, transcount, 0 );
		}
	}
	else
	{
		int pngcolortype = srcformat.hasAlpha() ? PNG_COLOR_TYPE_RGBA : PNG_COLOR_TYPE_RGB;
		outformat = SurfaceFormat::SURFACE_R8G8B8;
		if ( srcformat.hasAlpha() )
			outformat = SurfaceFormat::SURFACE_A8R8G8B8;

		png_set_IHDR( pngptr, infoptr, width, height, 8, pngcolortype,
			PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE );

		png_set_bgr( pngptr );
	}

	png_write_info( pngptr, infoptr );

	// write image
	Array<uint8_t> row( width*outformat.bitsPerPixel()/8 );
	for ( int i = 0 ; i < (int)height ; ++i )
	{
		const uint8_t* data = reinterpret_cast<const uint8_t*>(bits) + i*pitch;
		outformat.copyPixels( row.begin(), srcpalformat, srcpal, srcformat, data, srcpalformat, srcpal, width );
		png_write_row( pngptr, reinterpret_cast<png_bytep>(row.begin()) );
	}

	png_write_end( pngptr, infoptr );
	if ( pngpal )
		png_free( pngptr, pngpal );
	png_destroy_write_struct( &pngptr, &infoptr );
}


END_NAMESPACE() // img

#endif // IMG_PNG_SUPPORT

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
