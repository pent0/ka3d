#include <img/ImageWriter.h>
#include <img/ImageReader.h>
#include <gr/SurfaceFormat.h>
#include <io/IOException.h>
#include <io/FileOutputStream.h>
#include <io/FilterOutputStream.h>
#include <io/ByteArrayOutputStream.h>
#include <lang/Array.h>
#include <lang/String.h>
#include <lang/Huffman16.h>
#include <lang/algorithm/sort.h>
#include <lang/algorithm/unique.h>
#include <string.h>
#include <config.h>


#define FILE_VERSION 0x103
/*
changes:
- 0x103: color key fix
- 0x102: fixed palette size
- 0x101: fixed pixel data writing bug
*/


USING_NAMESPACE(gr)
USING_NAMESPACE(io)
USING_NAMESPACE(lang)


BEGIN_NAMESPACE(img) 


int ImageWriter::writeNTX( const String& filename, const void* bits, int w, int h, int pitch,
	SurfaceFormat format, int flags, int userflags )
{
	assert( flags >= 0 && flags < 0x10000 );
	assert( userflags >= 0 && userflags < 0x10000 );

	ByteArrayOutputStream byteout;
	FilterOutputStream out( &byteout );

	// find unique pal
	int bytesperpixel = format.bitsPerPixel()/8;
	int pixels = w*h;
	Array<uint32_t> pal( pixels );
	for ( int j = 0 ; j < h ; ++j )
	{
		for ( int i = 0 ; i < w ; ++i )
		{
			const uint8_t* s = (const uint8_t*)bits + i*bytesperpixel + j*pitch;
			pal[j*w+i] = getBytesLE( s, bytesperpixel );
		}
	}
	LANG_SORT( pal.begin(), pal.end() );
	pal.resize( lang::unique(pal.begin(),pal.end()) - pal.begin() );

	// calculate optimal needed storage size
	int size1 = w * h * bytesperpixel;
	int size2 = w * h + pal.size()*bytesperpixel;
	if ( size2 >= size1 || pal.size() > 256 )
		pal.resize( 0 );

	// write pal
	if ( pal.size() > 0 )
	{
		for ( int i = 0 ; i < pal.size() ; ++i )
		{
			uint8_t bytes[4];
			setBytesLE( bytes, pal[i], bytesperpixel );
			out.write( bytes, bytesperpixel );
		}
	}

	// write pixel data
	for ( int j = 0 ; j < h ; ++j )
	{
		for ( int i = 0 ; i < w ; ++i )
		{
			const uint8_t* s = (const uint8_t*)bits + i*bytesperpixel + j*pitch;
			uint32_t d = getBytesLE( s, bytesperpixel );

			if ( pal.size() > 0 )
			{
				int index = pal.indexOf( d );
				assert( index != -1 );
				assert( index >= 0 && index < 256 );
				uint8_t index8 = (uint8_t)index;
				out.write( &index8, 1 );
			}
			else
			{
				out.write( s, bytesperpixel );
			}
		}
	}

	// write file
	{
		Array<uint8_t> data = byteout.toByteArray();
		if ( data.size() & 1 )
			data.add( 0 );

		// write header
		FileOutputStream out( filename );
		ImageReader::NTXHeader header;
		header.version = FILE_VERSION;
		header.width = w;
		header.height = h;
		header.format = (uint8_t)format.type();
		header.palettesize = (uint16_t)pal.size();
		header.flags = (uint16_t)flags;
		header.userflags = (uint16_t)userflags;
		out.write( &header, sizeof(header) );

		// optional Huffman-compression
		/*if ( flags & NTX_COMPRESS )
		{
			Array<uint8_t> tmp;
			Huffman16 hf;
			uint16_t* data16 = (uint16_t*)&data[0];
			int datasize16 = data.size()/2;
			hf.compress( data16, datasize16, &tmp );
			data = tmp;
			out.write( &datasize16, sizeof(datasize16) );
		}*/

		// write data
		out.write( &data[0], data.size() );
		return data.size() + sizeof(ImageReader::NTXHeader);
	}
}


END_NAMESPACE() // img
