#include <img/ImageReader.h>
#include <io/InputStream.h>
#include <io/IOException.h>
#include <string.h>
#include <stdint.h>
#include <config.h>


USING_NAMESPACE(io)
USING_NAMESPACE(gr)
USING_NAMESPACE(lang)


BEGIN_NAMESPACE(img) 


void ImageReader::readHeader_bmp()
{
	uint8_t bmpfileheader[14];
	uint8_t bmpinfoheader[64];

	readFully( m_in, bmpfileheader, sizeof(bmpfileheader) );
	if ( getUInt16LE(bmpfileheader,0) != 0x4D42 )
		throwError( IOException( Format("BMP {0} header corrupted, id invalid", m_in->toString()) ) );
	int offbits = getUInt32LE( bmpfileheader, 10 );
	
	// The infoheader might be 12 bytes (OS/2 1.x), 40 bytes (Windows),
	// or 64 bytes (OS/2 2.x).  Check the first 4 bytes to find out which.
	readFully( m_in, bmpinfoheader, 4 );
	int headerSize = getUInt32LE( bmpinfoheader, 0 );
	if ( headerSize < 12 || headerSize > 64 )
		throwError( IOException( Format("BMP {0} header corrupted, header wrong size", m_in->toString()) ) );
	readFully( m_in, bmpinfoheader+4, headerSize-4 );

	int		biWidth = 0;
	int		biHeight = 0;
	int		biPlanes = 0;
	int		biBitsPerPixel = 0;
	int		biMapEntrySize = 0;
	int		biClrUsed = 0;
	int		biCompression = 0;

	switch ( headerSize )
	{
	case 12:
		// Decode OS/2 1.x header (in Win32 BITMAPCOREHEADER)
		biWidth = getUInt16LE( bmpinfoheader, 4 );
		biHeight = getUInt16LE( bmpinfoheader, 6 );
		biPlanes = getUInt16LE( bmpinfoheader, 8 );
		biBitsPerPixel = getUInt16LE( bmpinfoheader, 10 );
		switch ( biBitsPerPixel )
		{
		case 8:
			biMapEntrySize = 3;
			break;
		case 24:
			break;
		default:
			throwError( IOException( Format("BMP {0} header corrupted, invalid bits per pixel ({1})", m_in->toString(), biBitsPerPixel) ) );
		}
		break;

	case 40:
	case 64:
		// Decode Windows 3.x header (in Win32 BITMAPINFOHEADER)
		// or OS/2 2.x header, which has additional fields that we ignore
		biWidth = getUInt16LE( bmpinfoheader, 4 );
		biHeight = getUInt16LE( bmpinfoheader, 8 );
		biPlanes = getUInt16LE( bmpinfoheader, 12 );
		biBitsPerPixel = getUInt16LE( bmpinfoheader, 14 );
		biCompression = getUInt32LE( bmpinfoheader, 16 );
		biClrUsed = getUInt32LE( bmpinfoheader, 32 );

		switch ( biBitsPerPixel )
		{
		case 4:
		case 8:
			biMapEntrySize = 4;
			break;
		case 24:
		case 32:
			break;
		case 16:
			break;
		default:
			throwError( IOException( Format("BMP {0} header corrupted, invalid bits per pixel ({1})", m_in->toString(), biBitsPerPixel) ) );
		}

		if ( 1 != biPlanes )
			throwError( IOException( Format("BMP {0} header corrupted, invalid planes ({1})", m_in->toString(), biPlanes) ) );
		if ( 0 != biCompression )
			throwError( IOException( Format("BMP {0} header corrupted, invalid compression ({1})", m_in->toString(), biCompression) ) );
		break;

	default:
		throwError( IOException( Format("BMP {0} header corrupted, invalid compression ({1})", m_in->toString(), biCompression) ) );
	}

	// offset to bitmap data
	int bPad = offbits - (headerSize+14);
	
	// read colormap if any
	if ( biMapEntrySize > 0 )
	{
		if ( biClrUsed <= 0 )
			biClrUsed = (1 << biBitsPerPixel);
		if ( biClrUsed > 256 )
			throwError( IOException( Format("BMP {0} header corrupted, invalid no of used colors ({1})", m_in->toString(), biClrUsed) ) );
		memset( &m_colormap[0][0], 0, sizeof(m_colormap) );
		readColorMap( m_in, biMapEntrySize, biClrUsed, &m_colormap[0][0] );
		bPad -= biClrUsed * biMapEntrySize;
	}
	
	// pad
	m_in->skip( bPad );

	// row pitch aligned to 4-byte boundary
	int biPitch = (biWidth * biBitsPerPixel)/8;
	biPitch += 3;
	biPitch &= ~3;
	
	// setup header info
	m_width = biWidth;
	m_height = biHeight;
	m_bitsPerPixel = biBitsPerPixel;
	m_pitch = biPitch;
	m_surfaces = 1;
	m_bottomUp = true;
	
	switch ( m_bitsPerPixel )
	{
	case 4:
		m_fmt = SurfaceFormat::SURFACE_P4;
		m_palfmt = SurfaceFormat::SURFACE_X8B8G8R8;
		break;

	case 8:
		m_fmt = SurfaceFormat::SURFACE_P8;
		m_palfmt = SurfaceFormat::SURFACE_X8B8G8R8;
		break;
	
	case 16:
		m_fmt = SurfaceFormat::SURFACE_R5G5B5;
		m_palfmt = SurfaceFormat::SURFACE_UNKNOWN;
		break;

	case 24:
		m_fmt = SurfaceFormat::SURFACE_R8G8B8;
		m_palfmt = SurfaceFormat::SURFACE_UNKNOWN;
		break;
	
	case 32:
		m_fmt = SurfaceFormat::SURFACE_X8R8G8B8;
		m_palfmt = SurfaceFormat::SURFACE_UNKNOWN;
		break;
	}
}


END_NAMESPACE() // img

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
