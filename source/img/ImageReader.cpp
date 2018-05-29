#include <img/ImageReader.h>
#include <io/InputStream.h>
#include <io/IOException.h>
#include <lang/Array.h>
#include <lang/TempBuffer.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>
#include <config.h>


USING_NAMESPACE(io)
USING_NAMESPACE(gr)
USING_NAMESPACE(lang)


BEGIN_NAMESPACE(img) 


ImageReader::ImageReader() :
	m_in( 0 ),
	m_width( 0 ),
	m_height( 0 ),
	m_bitsPerPixel( 0 ),
	m_pitch( 0 ),
	m_mipLevels( 1 ),
	m_surfaces( 0 ),
	m_surfaceIndex( 0 ),
	m_scanlinebuffer(),
	m_fmt(),
	m_palfmt(),
	m_filefmt( FILEFORMAT_UNKNOWN ),
	m_bottomUp( false ),
	m_useRLE( false ),
	m_cubeMap( false ),
	m_colorKeyEnabled( false )
{
}

ImageReader::ImageReader( InputStream* in, FileFormat filefmt )
{
	reset( in, filefmt );
}

void ImageReader::reset( InputStream* in, FileFormat filefmt )
{
	m_in = in;
	m_width = 0;
	m_height = 0;
	m_bitsPerPixel = 0;
	m_pitch = 0;
	m_mipLevels = 0;
	m_surfaces = 0;
	m_surfaceIndex = 0;
	m_filefmt = filefmt;
	m_bottomUp = false;
	m_useRLE = false;
	m_cubeMap = false;
	m_colorKeyEnabled = false;

	switch ( m_filefmt )
	{
	case FILEFORMAT_BMP:	readHeader_bmp(); break;
	case FILEFORMAT_TGA:	readHeader_tga(); break;
	case FILEFORMAT_NTX:	readHeader_ntx(); break;
#ifdef IMG_JPG_SUPPORT
	case FILEFORMAT_JPG:	readHeader_jpg(); break;
#endif
#ifdef IMG_IL_SUPPORT
	case FILEFORMAT_PNG:	//readHeader_png(); break;
	case FILEFORMAT_DDS:
	case FILEFORMAT_PSD:
	case FILEFORMAT_PCX:	
	case FILEFORMAT_PNM:	
	case FILEFORMAT_GIF:
	case FILEFORMAT_TIF:	readHeader_il(); break;
#endif // IMG_IL_SUPPORT
	default:				throwError( IOException( Format("Unsupported image file format while reading {0}", m_in->toString()) ) );
	}
}

ImageReader::~ImageReader()
{
}

void ImageReader::readSurface( void* bits, int pitch, int w, int h, SurfaceFormat fmt,
	const void* pal, SurfaceFormat palfmt )
{
	assert( w == surfaceWidth() );
	assert( h == surfaceHeight() );
	
	switch ( m_filefmt )
	{
	case FILEFORMAT_NTX:
	case FILEFORMAT_BMP:
	case FILEFORMAT_TGA:
		readScanlines( bits, pitch, w, h, fmt, pal, palfmt );
		break;

#ifdef IMG_JPG_SUPPORT
	case FILEFORMAT_JPG:
		readSurface_jpg( bits, pitch, w, h, fmt, pal, palfmt );
		break;
#endif // IMG_JPG_SUPPORT

	default:
		readSurfaceFromSurfaceBuffer( bits, pitch, w, h, fmt, pal, palfmt );
		break;
	}
}

int ImageReader::surfaces() const
{
	return m_surfaces;
}

int	ImageReader::surfaceIndex() const
{
	return m_surfaceIndex;
}

bool ImageReader::cubeMap() const
{
	return m_cubeMap;
}

int ImageReader::mipLevels() const
{
	return m_mipLevels;
}

SurfaceFormat ImageReader::format() const
{
	return m_fmt;
}

const void* ImageReader::paletteData() const
{
	return &m_colormap[0][0];
}

SurfaceFormat ImageReader::paletteFormat() const
{
	return m_palfmt;
}

int ImageReader::surfaceWidth() const
{
	return m_width;
}

int ImageReader::surfaceHeight() const
{
	return m_height;
}

ImageReader::FileFormat ImageReader::guessFileFormat( const String& filename )
{
	ImageReader::FileFormat ff = ImageReader::FILEFORMAT_UNKNOWN;

	int len = filename.length();
	if ( len >= 5 )
	{
		char ext[4];
		filename.getChars( len-3, len, ext );
		ext[3] = 0;

		for ( int i = 0 ; i < 3 ; ++i )
			ext[i] = tolower( ext[i] );

		if ( !strcmp(ext,"bmp") )
			ff = ImageReader::FILEFORMAT_BMP;
		else if ( !strcmp(ext,"tga") )
			ff = ImageReader::FILEFORMAT_TGA;
		else if ( !strcmp(ext,"jpg") )
			ff = ImageReader::FILEFORMAT_JPG;
		else if ( !strcmp(ext,"dds") )
			ff = ImageReader::FILEFORMAT_DDS;
		else if ( !strcmp(ext,"psd") )
			ff = ImageReader::FILEFORMAT_PSD;
		else if ( !strcmp(ext,"png") )
			ff = ImageReader::FILEFORMAT_PNG;
		else if ( !strcmp(ext,"pcx") )
			ff = ImageReader::FILEFORMAT_PCX;
		else if ( !strcmp(ext,"pnm") )
			ff = ImageReader::FILEFORMAT_PNM;
		else if ( !strcmp(ext,"gif") )
			ff = ImageReader::FILEFORMAT_GIF;
		else if ( !strcmp(ext,"tif") )
			ff = ImageReader::FILEFORMAT_TIF;
		else if ( !strcmp(ext,"ntx") )
			ff = ImageReader::FILEFORMAT_NTX;
	}

	return ff;
}

void ImageReader::readFully( InputStream* in, void* buf, int bytes )
{
	if ( in->read(buf,bytes) != bytes )
		throwError( IOException( Format("Failed to read {0} bytes from {1}", bytes, in->toString()) ) );
}

void ImageReader::readColorMap( InputStream* in, int entrysize, int entries, uint8_t* colormap )
{
	uint8_t colr[4];
	for ( int i = 0 ; i < entries ; ++i )
	{
		memset( colr, 0, sizeof(colr) );
		readFully( in, colr, entrysize );
		colormap[i*4+0] = colr[2];
		colormap[i*4+1] = colr[1];
		colormap[i*4+2] = colr[0];
		colormap[i*4+3] = colr[3];
	}
}

uint16_t ImageReader::getUInt16LE( const void* data, int offset )
{
	const uint8_t* bytes = reinterpret_cast<const uint8_t*>(data) + offset;
	return uint16_t( ( unsigned(bytes[1]) << 8 ) + unsigned(bytes[0]) );
}

uint32_t ImageReader::getUInt32LE( const void* data, int offset )
{
	const uint8_t* bytes = reinterpret_cast<const uint8_t*>(data) + offset;
	return ( uint32_t(bytes[3]) << 24 ) + ( uint32_t(bytes[2]) << 16 ) + ( uint32_t(bytes[1]) << 8 ) + uint32_t(bytes[0]);
}

void ImageReader::readScanlines( void* bits, int pitch, int w, int h, SurfaceFormat fmt,
	const void* pal, SurfaceFormat palfmt )
{
	assert( w == m_width );
	assert( h == m_height );

	// optimized loading (of NTX files)
	if ( m_filefmt == FILEFORMAT_NTX )
	{
		if ( fmt == m_fmt && palfmt == SurfaceFormat::SURFACE_UNKNOWN && pitch == w*(m_fmt.bitsPerPixel()>>3) )
		{
			int bytesperpixel = (m_fmt.bitsPerPixel()>>3);
			readFully( m_in, bits, w*h*bytesperpixel );
			return;
		}
		if ( m_palfmt == fmt && 
			m_fmt == SurfaceFormat::SURFACE_P8 && 
			pitch == w*(m_palfmt.bitsPerPixel()>>3) )
		{
			const int ROWS_AT_ONCE = 8;
			ByteTempBuffer tempbuffer( w*ROWS_AT_ONCE );
			uint8_t* const buf = (uint8_t*)tempbuffer.buffer();

			for ( int k = 0 ; k < h ; )
			{
				const int rows = k+ROWS_AT_ONCE <= h ? ROWS_AT_ONCE : h-k;
				const int pixels = w*rows;

				readFully( m_in, buf, pixels );

				switch ( (m_palfmt.bitsPerPixel()>>3) )
				{
				case 1:{
					uint8_t* pal = (uint8_t*)&m_colormap[0][0];
					uint8_t* d = (uint8_t*)bits;
					for ( int i = 0 ; i < pixels ; ++i )
						d[i] = pal[ buf[i] ];
					break;}
				case 2:{
					uint16_t* pal = (uint16_t*)&m_colormap[0][0];
					uint16_t* d = (uint16_t*)bits;
					for ( int i = 0 ; i < pixels ; ++i )
						d[i] = pal[ buf[i] ];
					break;}
				case 3:{
					uint8_t* pal = (uint8_t*)&m_colormap[0][0];
					uint8_t* d = (uint8_t*)bits;
					for ( int i = 0 ; i < pixels ; ++i )
					{
						d[i*3+0] = pal[ buf[i]*3+0 ];
						d[i*3+1] = pal[ buf[i]*3+0 ];
						d[i*3+2] = pal[ buf[i]*3+0 ];
					}
					break;}
				case 4:{
					uint32_t* pal = (uint32_t*)&m_colormap[0][0];
					uint32_t* d = (uint32_t*)bits;
					for ( int i = 0 ; i < pixels ; ++i )
						d[i] = pal[ buf[i] ];
					break;}
				}

				bits = (uint8_t*)bits + pitch*rows;
				k += rows;
			}
			return;
		}
		//assert( false );
	}

	// make sure we have big enough scanline buffer,
	// but use dynamic allocation only if absolutely necessary
	ByteTempBuffer tempbuffer( m_pitch*2 );
	uint8_t* scanlinebuffer = (uint8_t*)tempbuffer.buffer();

	// read pixels
	for ( int j = 0 ; j < m_height ; ++j )
	{
		switch ( m_filefmt )
		{
		case FILEFORMAT_BMP:
		case FILEFORMAT_TGA:
		case FILEFORMAT_NTX:
			if ( m_useRLE )
				readScanline_tgaRLE( scanlinebuffer );
			else
				readFully( m_in, scanlinebuffer, m_pitch );
			break;
#ifdef IMG_JPG_SUPPORT
		case FILEFORMAT_JPG:
			readScanline_jpg( scanlinebuffer );
			break;
#endif // IMG_JPG_SUPPORT
		default:
			throwError( IOException( Format("Unsupported image file format while reading {0}", m_in->toString()) ) );
		}

		int j1 = j;
		if ( m_bottomUp )
			j1 = m_height - j - 1;
		uint8_t* dst = reinterpret_cast<uint8_t*>(bits) + j1*pitch;

		if ( fmt.type() == m_fmt.type() )
			memcpy( dst, scanlinebuffer, (m_width*m_fmt.bitsPerPixel()+7)>>3 );
		else
			fmt.copyPixels( dst, palfmt, pal, m_fmt, scanlinebuffer, m_palfmt, &m_colormap[0][0], m_width );
	}
}

void ImageReader::readSurfaceFromSurfaceBuffer( void* bits, int pitch, 
	int w, int h, SurfaceFormat fmt, const void* pal, SurfaceFormat palfmt )
{
	assert( m_surfaceIndex >= 0 && m_surfaceIndex < m_surfaceBuffer.size() );
	
	Surface& s = m_surfaceBuffer[m_surfaceIndex];
	assert( s.width == w );
	assert( s.height == h );

	fmt.copyPixels( bits, pitch, palfmt, pal,
		m_fmt, &s.data[0], s.width*m_fmt.bitsPerPixel()/8, m_palfmt, m_colormap,
		w, h );

	// iterate to next surface
	m_surfaceIndex++;
	if ( m_surfaceIndex < m_surfaceBuffer.size() )
	{
		m_width = m_surfaceBuffer[m_surfaceIndex].width;
		m_height = m_surfaceBuffer[m_surfaceIndex].height;
	}
}

void ImageReader::readHeader_ntx()
{
	NTXHeader header;
	readFully( m_in, &header, sizeof(NTXHeader) );

	const int VERSION = 0x103;
	if ( header.version != VERSION )
		throwError( IOException( Format("NTX file {0} has incorrect version ({1}, expected {2}", m_in->toString(), (int)header.version, VERSION) ) );

	m_width = header.width;
	m_height = header.height;
	m_fmt = SurfaceFormat( (SurfaceFormat::SurfaceFormatType)header.format );
	m_palfmt = SurfaceFormat();
	m_colorKeyEnabled = (header.flags & NTXHeader::FLAG_COLORKEY) != 0;

	if ( header.palettesize > 0 )
	{
		m_palfmt = m_fmt;
		m_fmt = SurfaceFormat::SURFACE_P8;

		readFully( m_in, &m_colormap[0][0], header.palettesize * m_palfmt.bitsPerPixel()/8 );
	}

	m_pitch = m_width * (m_fmt.bitsPerPixel()>>3);
}

END_NAMESPACE() // img

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
