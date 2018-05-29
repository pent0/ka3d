#include <io/PathName.h>
#include <io/FileInputStream.h>
#include <io/FileOutputStream.h>
#include <gr/SurfaceFormat.h>
#include <img/ImageReader.h>
#include <img/ImageWriter.h>
#include <math/float2.h>
#include <math/float3.h>
#include <math/RandomUtil.h>
#include <lang/all.h>
#include <stdio.h>
#include <stdint.h>
#include <config.h>


#define countof(A) (sizeof(A)/sizeof(A[0]))


USING_NAMESPACE(io)
USING_NAMESPACE(gr)
USING_NAMESPACE(img)
USING_NAMESPACE(lang)
USING_NAMESPACE(math)


BEGIN_NAMESPACE(img) 


static uint32_t rgb32( int r, int g, int b, int a )
{
	if ( r < 0 )
		r = 0;
	else if ( r > 255 )
		r = 255;
	if ( g < 0 )
		g = 0;
	else if ( g > 255 )
		g = 255;
	if ( b < 0 )
		b = 0;
	else if ( b > 255 )
		b = 255;
	if ( a < 0 )
		a = 0;
	else if ( a > 255 )
		a = 255;
	return (a<<24) + (r<<16) + (g<<8) + b;
}

static uint32_t rgb32f( float r, float g, float b, float a )
{
	if ( r < 0.f )
		r = 0.f;
	else if ( r > 1.f )
		r = 1.f;
	if ( g < 0.f )
		g = 0.f;
	else if ( g > 1.f )
		g = 1.f;
	if ( b < 0.f )
		b = 0.f;
	else if ( b > 1.f )
		b = 1.f;
	if ( a < 0.f )
		a = 0.f;
	else if ( a > 1.f )
		a = 1.f;
	return rgb32( int(r*255.f), int(g*255.f), int(b*255.f), int(a*255.f) );
}

static void setPixel( Array<uint32_t>& data, int w, int h, float3 point, uint32_t color )
{
	int x = (int)point.x;
	int y = (int)point.y;
	if ( x >= 0 && x < w && y >= 0 && y < h )
		data[y*w+x] = color;
}

static void writePhongMap( const String& filename, float shininess, SurfaceFormat dstformat )
{
	int w = 256;
	int h = 256;
	SurfaceFormat fmt = SurfaceFormat::SURFACE_X8R8G8B8;
	SurfaceFormat palfmt = SurfaceFormat::SURFACE_UNKNOWN;
	Array<uint32_t> paldata( 256 );
	Array<uint32_t> data( w*h );

	for ( int j = 0 ; j < h ; ++j )
		for ( int i = 0 ; i < w ; ++i )
		{
			float NdotL = float(i)/float(w);
			float NdotH = float(j)/float(h);
			float diffuse = Math::max( NdotL, 0.f );
			float specular = Math::pow( NdotH, shininess );
			if (NdotL <= 0.f) 
				specular = 0.f;

			data[j*w+i] = rgb32f( diffuse, diffuse, diffuse, specular );
		}

	ImageWriter::write( filename, dstformat, data.begin(), w, h, w*fmt.bitsPerPixel()/8, fmt, paldata.begin(), palfmt );
}

static void savePNG24( const String& datapath )
{
	int w = 16;
	int h = 16;
	SurfaceFormat fmt = SurfaceFormat::SURFACE_A8R8G8B8;
	SurfaceFormat palfmt = SurfaceFormat::SURFACE_UNKNOWN;
	Array<uint32_t> paldata( 256 );
	Array<uint32_t> data( w*h );

	for ( int j = 0 ; j < h ; ++j )
		for ( int i = 0 ; i < w ; ++i )
			data[j*w+i] = rgb32( j*(256/h), i*(256/w), 0, 0x80 );

	ImageWriter::writePNG( PathName(datapath,"images/out-test-24b.png").toString(), 
		data.begin(), w, h, w*fmt.bitsPerPixel()/8, fmt, paldata.begin(), palfmt );
}

static void savePNG8( const String& datapath )
{
	int w = 8;
	int h = 32;
	SurfaceFormat fmt = SurfaceFormat::SURFACE_P8;
	SurfaceFormat palfmt = SurfaceFormat::SURFACE_A8R8G8B8;
	Array<uint32_t> paldata( 256 );
	for ( int i = 0 ; i < paldata.size() ; ++i )
		paldata[i] = rgb32(i,0,0,0x80);
	Array<uint8_t> data( w*h );

	for ( int j = 0 ; j < h ; ++j )
		for ( int i = 0 ; i < w ; ++i )
			data[j*w+i] = (uint8_t)(j*(256/h));

	ImageWriter::writePNG( PathName(datapath,"images/out-test-8b.png").toString(), 
		data.begin(), w, h, w*fmt.bitsPerPixel()/8, fmt, paldata.begin(), palfmt );
}

/*static int getChannel( int rgb32, int channelindex )
{
	int shift = channelindex << 3;
	return ((rgb32>>shift)&0xFF);
}*/

static bool isBrighterChannel( int color1, int color2, int channelindex )
{
	int shift = channelindex << 3;
	return ((color1>>shift)&0xFF) > ((color2>>shift)&0xFF);
}

static void loadBmpAndTga( const String& datapath )
{
	// load images/ and check that each has 
	// white center and corner colors
	// topleft, topright, bottomleft, bottomright
	// and 0x80 alpha if any
	String filenames[] = {
		"rgb_corners-4b.bmp",
		"rgb_corners-8b.bmp",
		"rgb_corners-24b.bmp",
		"rgb_corners-8b.tga",
		"rgb_corners-24b.tga",
		"rgb_corners-32b.tga",
	};
	SurfaceFormat expectedFormat[] = {
		SurfaceFormat::SURFACE_P4,
		SurfaceFormat::SURFACE_P8,
		SurfaceFormat::SURFACE_R8G8B8,
		SurfaceFormat::SURFACE_P8,
		SurfaceFormat::SURFACE_R8G8B8,
		SurfaceFormat::SURFACE_A8R8G8B8,
	};
	SurfaceFormat expectedPaletteFormat[] = {
		SurfaceFormat::SURFACE_X8B8G8R8,
		SurfaceFormat::SURFACE_X8B8G8R8,
		SurfaceFormat::SURFACE_UNKNOWN,
		SurfaceFormat::SURFACE_X8B8G8R8,
		SurfaceFormat::SURFACE_UNKNOWN,
		SurfaceFormat::SURFACE_UNKNOWN,
	};

	assert( countof(expectedPaletteFormat) == countof(expectedFormat) );
	assert( countof(filenames) == countof(expectedFormat) );
	for ( int k = 0 ; k < (int)(sizeof(filenames)/sizeof(filenames[0])) ; ++k )
	{
		String filename = filenames[k];
		PathName imgpath(datapath,"images");
		FileInputStream in( PathName(imgpath.toString(), filename).toString() );
		ImageReader rd( &in, ImageReader::guessFileFormat(filename) );

		int w = rd.surfaceWidth();
		int h = rd.surfaceHeight();
		SurfaceFormat fmt = rd.format();
		assert( rd.surfaces() == 1 );
		assert( fmt == expectedFormat[k] );
		assert( rd.paletteFormat() == expectedPaletteFormat[k] );
		assert( w == 16 );
		assert( h == 8 );
		
		Array<uint32_t> data;
		data.resize( w*h );
		rd.readSurface( data.begin(), w*4, w, h, SurfaceFormat::SURFACE_A8R8G8B8, 0, SurfaceFormat::SURFACE_UNKNOWN );
		/*Debug::print( "Image:\n" );
		for ( int j = 0 ; j < h ; ++j )
		{
			for ( int i = 0 ; i < w ; ++i )
				Debug::print( "0x{0,00000000,x}, ", data[i+w*j] );
			Debug::print( "\n" );
		}
		Debug::print( "\n" );*/

		uint32_t a = 0xFF<<24;
		if ( fmt.hasAlpha() )
			a = 0x80<<24;

		assert( data[(0)+(0)*w] == a+0xFF0000 );
		assert( data[(w-1)+(0)*w] == a+0x00FF00 );
		assert( data[(0)+(h-1)*w] == a+0x0000FF );
		assert( data[(w-1)+(h-1)*w] == a+0x000000 );
		assert( data[(w/2)+(h/2)*w] == a+0xFFFFFF );
	}
}

static void loadJpeg( const String& datapath )
{
	// test jpg (red gradient)
	String filename = "red_grad.jpg";
	PathName imgpath(datapath,"images/");
	FileInputStream in( PathName(imgpath.toString(), filename).toString() );
	ImageReader rd( &in, ImageReader::guessFileFormat(filename) );

	int w = rd.surfaceWidth();
	int h = rd.surfaceHeight();
	SurfaceFormat fmt = rd.format();
	assert( rd.surfaces() == 1 );
	assert( fmt == SurfaceFormat::SURFACE_B8G8R8 );
	assert( rd.paletteFormat() == SurfaceFormat::SURFACE_UNKNOWN );
	assert( w == 16 );
	assert( h == 8 );
	
	Array<uint32_t> data;
	data.resize( w*h );
	rd.readSurface( data.begin(), w*4, w, h, SurfaceFormat::SURFACE_A8R8G8B8, 0, SurfaceFormat::SURFACE_UNKNOWN );

	Debug::printf( "Image (red->black gradient from top-left to bottom-right):\n" );
	for ( int j = 0 ; j < h ; ++j )
	{
		for ( int i = 0 ; i < w ; ++i )
			Debug::printf( "{%x}", (data[i+w*j]>>20)&0xF );
		Debug::printf( "\n" );
	}
	Debug::printf( "\n" );

	assert( isBrighterChannel(data[(0)+(0)*w],data[(w-1)+(0)*w],2) );
	assert( isBrighterChannel(data[(0)+(0)*w],data[(w-1)+(h-1)*w],2) );
	assert( isBrighterChannel(data[(0)+(0)*w],data[(0)+(h-1)*w],2) );
	assert( isBrighterChannel(data[(0)+(h-1)*w],data[(w-1)+(h-1)*w],2) );
	assert( isBrighterChannel(data[(w-1)+(0)*w],data[(w-1)+(h-1)*w],2) );
}

static void savePng( const String& datapath )
{
	savePNG8( datapath );
	savePNG24( datapath );
}

static void saveDDS( const String& datapath )
{
	int w = 64;
	int h = 32;
	SurfaceFormat fmt = SurfaceFormat::SURFACE_A8R8G8B8;

	// top-left black, top-right red, bottom-left green, bottom-right cyan
	Array<uint32_t> data( w*h );
	for ( int j = 0 ; j < h ; ++j )
	{
		for ( int i = 0 ; i < w ; ++i )
		{
			float3 p = float3( float(i), float(j), 0.f );
			float3 u = p * float3(1.f/float(w),1.f/float(h),1.f);
			float a = u.y>0.3f || u.x>0.3f ? 1.f:0.f;
			setPixel( data,w,h, p, rgb32f(u.x,u.y,0.f,a) );
		}
	}

	ImageWriter::write( 
		PathName(datapath,"images/out-test-dxt1.dds").toString(), SurfaceFormat::SURFACE_DXT1,
		data.begin(), w, h, w*fmt.bitsPerPixel()/8, fmt, 0, SurfaceFormat() );
	ImageWriter::write( 
		PathName(datapath,"images/out-test-dxt3.dds").toString(), SurfaceFormat::SURFACE_DXT3,
		data.begin(), w, h, w*fmt.bitsPerPixel()/8, fmt, 0, SurfaceFormat() );
	ImageWriter::write( 
		PathName(datapath,"images/out-test-dxt5.dds").toString(), SurfaceFormat::SURFACE_DXT5,
		data.begin(), w, h, w*fmt.bitsPerPixel()/8, fmt, 0, SurfaceFormat() );
}

static void saveRandomDist( const String& datapath )
{
	// disk
	int w = 256;
	int h = 256;
	SurfaceFormat fmt = SurfaceFormat::SURFACE_A8R8G8B8;
	Array<uint32_t> data( w*h );
	float3 center = float3((float)w/2.f,(float)h/2.f,0);

	for ( int i = 0 ; i < 1000 ; ++i )
		setPixel( data, w, h, RandomUtil::getPointOnDisk(70.f,100.f)+center, 0xFF00 );

	// triangle
	float3 p1( 0, -70, 0 );
	float3 p2( 0, 70, 0 );
	float3 p3( -70, 0, 0 );
	for ( int i = 0 ; i < 1000 ; ++i )
		setPixel( data, w, h, RandomUtil::getPointOnTriangle(p1,p2-p1,p3-p1)+center, 0xFF00 );

	ImageWriter::writePNG( PathName(datapath,"images/out-random-dist.png").toString(), 
		data.begin(), w, h, w*fmt.bitsPerPixel()/8, fmt, 0, SurfaceFormat::SURFACE_UNKNOWN );
}

static void loadCubeMap( const String& datapath )
{
	FileInputStream in( datapath+"images/cube_test.dds" );
	ImageReader reader( &in, ImageReader::FILEFORMAT_DDS );

	const char* axis[] = {"XP","XN","YP","YN","ZP","ZN"};

	Array<uint8_t> bits;
	for ( int i = 0 ; i < reader.surfaces() ; ++i )
	{
		int w = reader.surfaceWidth();
		int h = reader.surfaceHeight();
		int pitch = w * reader.format().bitsPerPixel()/8;
		bits.resize( pitch*h );
		reader.readSurface( &bits[0], pitch, w, h, reader.format(), 0, SurfaceFormat() );

		int axisindex = i / reader.mipLevels();
		assert( axisindex >= 0 && axisindex < 6 );
		char fname[256];
		sprintf( fname, "images/out_cube_test_%s.bmp", axis[axisindex] );

		ImageWriter::write( datapath+fname, reader.format(), &bits[0], w,h,pitch, 
			reader.format(), reader.paletteData(), reader.paletteFormat() );
	}
}

static void run( const String& datapath )
{
	loadCubeMap( datapath );
	saveRandomDist( datapath );
	loadBmpAndTga( datapath );
	loadJpeg( datapath );
	savePng( datapath );
	writePhongMap( PathName(datapath,"images/phong-5.png").toString(), 5.f, SurfaceFormat::SURFACE_A8R8G8B8 );
	writePhongMap( PathName(datapath,"images/phong-10.png").toString(), 10.f, SurfaceFormat::SURFACE_A8R8G8B8 );
	writePhongMap( PathName(datapath,"images/phong-20.png").toString(), 20.f, SurfaceFormat::SURFACE_A8R8G8B8 );
	writePhongMap( PathName(datapath,"images/phong-30.png").toString(), 30.f, SurfaceFormat::SURFACE_A8R8G8B8 );
	writePhongMap( PathName(datapath,"images/phong-40.png").toString(), 40.f, SurfaceFormat::SURFACE_A8R8G8B8 );
	writePhongMap( PathName(datapath,"images/phong-20-dxt1.dds").toString(), 20.f, SurfaceFormat::SURFACE_DXT1 );
	writePhongMap( PathName(datapath,"images/phong-20-dxt3.dds").toString(), 20.f, SurfaceFormat::SURFACE_DXT3 );
	writePhongMap( PathName(datapath,"images/phong-20-dxt5.dds").toString(), 20.f, SurfaceFormat::SURFACE_DXT5 );
	saveDDS( datapath );
}

void test( const String& datapath )
{
	String libname = "img";

	Debug::printf( "\n-------------------------------------------------------------------------\n" );
	Debug::printf( (libname + " library test begin").c_str() );
	Debug::printf( "\n-------------------------------------------------------------------------\n" );
	run( datapath );
	Debug::printf( (libname + " library test ok\n").c_str() );
}


END_NAMESPACE() // img

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
