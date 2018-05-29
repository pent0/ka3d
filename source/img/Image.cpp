#include <img/Image.h>
#include <gr/SurfaceFormat.h>
#include <io/FileInputStream.h>
#include <img/ImageReader.h>
#include <img/ImageWriter.h>
#include <lang/Debug.h>
#include <lang/Exception.h>
#include <assert.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <config.h>


USING_NAMESPACE(io)
USING_NAMESPACE(gr)
USING_NAMESPACE(img)
USING_NAMESPACE(lang)


BEGIN_NAMESPACE(img) 


Image::Image( int w, int h ) :
	m_bits( w*h, 0 ),
	m_width( w ),
	m_height( h )
{
}

Image::Image( const String& filename )
{
	FileInputStream fin( filename );

	// read image header
	ImageReader reader( &fin, ImageReader::guessFileFormat(filename) );
	int w = reader.surfaceWidth();
	int h = reader.surfaceHeight();
	SurfaceFormat fmt = SurfaceFormat::SURFACE_A8R8G8B8;

	// read image data
	int pitch = w * fmt.bitsPerPixel() / 8;
	int imgsize = h*pitch;
	m_bits.resize( w*h );
	reader.readSurface( m_bits.begin(), pitch, w, h, fmt, 0, SurfaceFormat() );

	m_width = w;
	m_height = h;
}

void Image::setPixel( int x, int y, uint32_t color )
{
	if ( x >= 0 && x < m_width && y >= 0 && y < m_height )
		m_bits[x+y*m_width] = color;
}

void Image::drawLine( int x0, int y0, int x1, int y1, uint32_t color )
{
	int count = abs(x1-x0) + abs(y1-y0);
	float incr = 1.f/float(count);
	float f = 0.f;
	int w = m_width;
	int h = m_height;

	for ( int i = 0 ; i < count ; ++i )
	{
		int xi = (int)( float(x0) + float(x1 - x0)*f );
		int yi = (int)( float(y0) + float(y1 - y0)*f );
		
		if ( xi >= 0 && xi < w && yi >= 0 && yi < h )
			m_bits[xi+yi*w] = color;

		f += incr;
	}
}

void Image::drawCircle( int originx, int originy, int radius, uint32_t color )
{
	float pi = 3.15f;
	float r = (float)radius;
	float arclen = 2.f * pi * r;
	int count = (int)arclen;
	float incr = 1.f/float(count) * 2.f * pi;
	float f = 0.f;
	int w = m_width;
	int h = m_height;

	for ( int i = 0 ; i < count ; ++i )
	{
		int xi = (int)( float(originx) + (float)cos(f)*r );
		int yi = (int)( float(originy) + (float)sin(f)*r );
		
		if ( xi >= 0 && xi < w && yi >= 0 && yi < h )
			m_bits[xi+yi*w] = color;

		f += incr;
	}
}

#ifdef IMG_WRITER_SUPPORT
void Image::save( const String& filename )
{
	ImageWriter::write( filename, SurfaceFormat::SURFACE_A8R8G8B8, m_bits.begin(), m_width, m_height, m_width*4, SurfaceFormat::SURFACE_A8R8G8B8, 0, SurfaceFormat() );
}
#endif

uint32_t Image::getPixel( int x, int y ) const
{
	assert( x >= 0 && x < m_width && y >= 0 && y < m_height );
	return m_bits[x+y*m_width];
}

static void calcEdges( int edgex[2][512], int edgeu[2][512], int edgev[2][512], 
	int x0, int x1, int y0, int y1, int u0, int u1, int v0, int v1, int w, int h )
{
	int dy = y1 - y0;
	if ( dy == 0 )
		return;

	int absdy = abs(dy);
	int dx16 = ((x1-x0) << 16) / absdy;
	int du16 = ((u1-u0) << 16) / absdy;
	int dv16 = ((v1-v0) << 16) / absdy;
	int x16 = x0 << 16;
	int u16 = u0 << 16;
	int v16 = v0 << 16;
	int yincr = dy < 0 ? -1 : 1;

	for ( int y = y0 ; y != y1 ; y += yincr )
	{
		int x = x16;
		int u = u16;
		int v = v16;

		if ( y >= 0 && y < h )
		{
			if ( x < edgex[0][y] )
			{
				edgex[0][y] = x;
				edgeu[0][y] = u;
				edgev[0][y] = v;
			}
			if ( x > edgex[1][y] )
			{
				edgex[1][y] = x;
				edgeu[1][y] = u;
				edgev[1][y] = v;
			}
			assert( edgex[0][y] <= edgex[1][y] );
		}

		x16 += dx16;
		u16 += du16;
		v16 += dv16;
	}
}

inline static uint32_t X4R4G4B4toA8R8G8B8( int c )
{
	int r = ((c>>8) & 15);
	int g = ((c>>4) & 15);
	int b = ((c) & 15);
	r = r*255/15;
	g = g*255/15;
	b = b*255/15;
	return (uint32_t)(0xFF000000 + (r<<16) + (g<<8) + b);
}

void Image::drawTexturedTriangle( 
	int x1, int y1, int x2, int y2, int x3, int y3,
	int u1, int v1, int u2, int v2, int u3, int v3,
	const uint16_t* tex, int texwidth, int texheight, int texpitch )
{
	assert( x1 > -4096 && x1 < 4096 );
	assert( y1 > -4096 && y1 < 4096 );
	assert( x2 > -4096 && x2 < 4096 );
	assert( y2 > -4096 && y2 < 4096 );
	assert( x3 > -4096 && x3 < 4096 );
	assert( y3 > -4096 && y3 < 4096 );
	assert( u1 >= 0 && u1 < texwidth );
	assert( v1 >= 0 && v1 < texheight );
	assert( u2 >= 0 && u2 < texwidth );
	assert( v2 >= 0 && v2 < texheight );
	assert( u3 >= 0 && u3 < texwidth );
	assert( v3 >= 0 && v3 < texheight );

	uint8_t* buf = (uint8_t*)&m_bits[0];
	int w = m_width;
	int h = m_height;
	int pitch = m_width<<2;

	// find min max y
	int ytop = y1;
	if ( y2 < ytop )
		ytop = y2;
	if ( y3 < ytop )
		ytop = y3;
	int ybottom = y1;
	if ( y2 > ybottom )
		ybottom = y2;
	if ( y3 > ybottom )
		ybottom = y3;
	int ycount = ybottom-ytop;
	if ( ycount >= 512 ) // too large triangle
		return;
	y1 -= ytop;
	y2 -= ytop;
	y3 -= ytop;

	// calc x and uv to edge buffers
	int edgex[2][512];
	int edgeu[2][512];
	int edgev[2][512];
	for ( int i = 0 ; i <= ycount ; ++i )
	{
		edgex[0][i] = w << 17;
		edgex[1][i] = (-w) << 16;
	}
	calcEdges( edgex,edgeu,edgev, x1,x2, y1,y2, u1,u2, v1,v2, w,h );
	calcEdges( edgex,edgeu,edgev, x2,x3, y2,y3, u2,u3, v2,v3, w,h );
	calcEdges( edgex,edgeu,edgev, x3,x1, y3,y1, u3,u1, v3,v1, w,h );

	// calc triangle du dv
	int ymid = ycount >> 1;
	int dxmid = (edgex[1][ymid] - edgex[0][ymid]) >> 8;
	if ( abs(dxmid) < 0x180 )
		return;
	int du16 = ((edgeu[1][ymid] - edgeu[0][ymid]) / dxmid)<<8;
	int dv16 = ((edgev[1][ymid] - edgev[0][ymid]) / dxmid)<<8;

	// log2(texpitch)
	int log2texpitch = 0;
	int testbit = 1;
	while ( 0 == (texpitch & testbit) )
	{
		++log2texpitch;
		testbit += testbit;
	}

	// setup other aux vars
	register const unsigned vshift = log2texpitch - 1;
	register const int umask = texwidth - 1;
	register const int vmask = texheight - 1;
	register uint8_t* dst = buf + ytop*pitch;

	// interpolate by scanline (dx,du,dv)/dy
	for ( register int y = 0 ; y < ycount ; ++y )
	{
		register int x0 = edgex[0][y] >> 16;
		register int x1 = edgex[1][y] >> 16;
		register int u16 = edgeu[0][y];
		register int v16 = edgev[0][y];

		int ytemp = y + ytop;
		if ( ytemp >= 0 && ytemp < h )
		{
			register uint32_t* d = (uint32_t*)dst;

			for ( int x = x0 ; x < x1 ; ++x )
			{
				if ( x >= 0 && x < w )
				{
					register int u = u16 >> 16;
					register int v = v16 >> 16;
					u &= umask;
					v &= vmask;
					d[x] = X4R4G4B4toA8R8G8B8( tex[ u + (v<<vshift) ] );
				}
				u16 += du16;
				v16 += dv16;
			}
		}

		dst += pitch;
	}
}


END_NAMESPACE() // img
