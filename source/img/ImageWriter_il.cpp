#ifdef IMG_IL_SUPPORT

#include <img/ImageWriter.h>
#include <gr/SurfaceFormat.h>
#include <io/IOException.h>
#include <io/FileOutputStream.h>
#include <lang/Array.h>
#include <lang/String.h>
#include "il/il.h"
#include <config.h>


USING_NAMESPACE(gr)
USING_NAMESPACE(io)
USING_NAMESPACE(lang)


BEGIN_NAMESPACE(img) 


void ImageWriter::write( const String& filename, SurfaceFormat dstformat,
	const void* bits, int width, int height, int pitch,
	SurfaceFormat srcformat, const void* srcpal, SurfaceFormat srcpalformat )
{
	assert( width > 0 );
	assert( height > 0 );
	assert( bits != 0 );
	assert( dstformat != SurfaceFormat::SURFACE_UNKNOWN );
	assert( srcformat != SurfaceFormat::SURFACE_UNKNOWN );

	// init
	ilInit();
	ilEnable( IL_FILE_OVERWRITE );
	ILuint img = 0;
	ilGenImages( 1, &img );
	ilBindImage( img );

	// surface
	SurfaceFormat surfaceformat( SurfaceFormat::SURFACE_A8B8G8R8 );
	ILubyte bytesperpixel = 4;
	ILenum format = IL_RGBA;
	ILenum type = IL_UNSIGNED_BYTE;
	ILboolean ok = ilTexImage( width, height, 1, bytesperpixel, format, type, 0 );
	assert( ok );

	// set surface data
	Array<uint32_t> buf( width );
	for ( int j = 0 ; j < height ; ++j )
	{
		const void* data = reinterpret_cast<const int8_t*>(bits) + j*pitch;
		surfaceformat.copyPixels( &buf[0], SurfaceFormat(), 0,
			srcformat, data, srcpalformat, srcpal, width );

		ilSetPixels( 0, (height-1)-j, 0, width, 1, 1, format, type, &buf[0] );
	}
	
	// DXT compression
	if ( dstformat.compressed() )
	{
		switch ( dstformat.type() )
		{
		case SurfaceFormat::SURFACE_DXT1:
			ilSetInteger( IL_DXTC_FORMAT, IL_DXT1 );
			break;
		case SurfaceFormat::SURFACE_DXT3:
			ilSetInteger( IL_DXTC_FORMAT, IL_DXT3 );
			break;
		case SurfaceFormat::SURFACE_DXT5:
			ilSetInteger( IL_DXTC_FORMAT, IL_DXT5 );
			break;
		}
	}

	// image file settings
	if ( !dstformat.hasAlpha() || !srcformat.hasAlpha() )
		ilConvertImage( IL_RGB, IL_UNSIGNED_BYTE );
	if ( dstformat.palettized() )
		ilConvertImage( IL_COLOR_INDEX, IL_UNSIGNED_BYTE );

	// save image to file
	char filenamebuf[1024];
	filename.get( filenamebuf, sizeof(filenamebuf) );
	ilSaveImage( filenamebuf );

	// cleanup
	ILenum err = ilGetError();
	ilDeleteImages( 1, &img );
	ilShutDown();

	// error occured?
	if ( err != IL_NO_ERROR )
		throwError( IOException( Format("Failed to write image file \"{0}\"", filename) ) );
}


END_NAMESPACE() // img

#endif // IMG_IL_SUPPORT

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
