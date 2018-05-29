#include <gr/all.h>
#include <lang/all.h>
#include <string.h>
#include <config.h>


USING_NAMESPACE(lang)


BEGIN_NAMESPACE(gr) 

	
static void run()
{
	// VertexFormat test
	{
		VertexFormat vf;
		vf.addPosition( VertexFormat::DF_V3_32 );
		vf.addNormal( VertexFormat::DF_V3_32 );
		vf.addDiffuse( VertexFormat::DF_V4_8 );
		vf.addTextureCoordinate( VertexFormat::DF_V2_32 );
		vf.addTextureCoordinate( VertexFormat::DF_V3_32 );
		vf.addTextureCoordinate( VertexFormat::DF_V3_32 );
		assert( vf.getDataFormat(VertexFormat::DT_POSITION) == VertexFormat::DF_V3_32 );
		assert( vf.getDataFormat(VertexFormat::DT_NORMAL) == VertexFormat::DF_V3_32 );
		assert( vf.getDataFormat(VertexFormat::DT_DIFFUSE) == VertexFormat::DF_V4_8 );
		assert( vf.textureCoordinates() == 3 );
		assert( vf.getTextureCoordinateFormat(0) == VertexFormat::DF_V2_32 );
		assert( vf.getTextureCoordinateFormat(1) == VertexFormat::DF_V3_32 );
		assert( vf.getTextureCoordinateFormat(2) == VertexFormat::DF_V3_32 );
	}
	
	// SurfaceFormat test
	{
		// 16 <-> 32
		{
			SurfaceFormat	srcfmt				= SurfaceFormat::SURFACE_A8R8G8B8;
			const int		pixels				= 8;
			uint32_t		source[ pixels ] 	= 
			{ 
				0xF0806040,0xF0806040, 0xF1816141,0xF1816141, 
				0xF2816141,0xF2816141, 0xF3816141,0xF3816141 
			};
		
			// two 32->16
			SurfaceFormat targetfmt = SurfaceFormat::SURFACE_R5G5B5;
			uint16_t target[ pixels ];
			targetfmt.copyPixels( target, SurfaceFormat::SURFACE_UNKNOWN, 0, srcfmt, source, SurfaceFormat::SURFACE_UNKNOWN, 0,  pixels );
		
			SurfaceFormat targetfmt2 = SurfaceFormat::SURFACE_A4R4G4B4;
			uint16_t target2[ pixels ];
			targetfmt2.copyPixels( target2, SurfaceFormat::SURFACE_UNKNOWN, 0, srcfmt, source, SurfaceFormat::SURFACE_UNKNOWN, 0,  pixels );
		
			int i;
			for ( i = 0 ; i < pixels ; ++i )
			{
				assert( target[i] == 0xC188 );
				assert( target2[i] == 0xF864 );
			}
		
			// 16->32, alpha channel default
			srcfmt.copyPixels( source, SurfaceFormat::SURFACE_UNKNOWN, 0, targetfmt, target, SurfaceFormat::SURFACE_UNKNOWN, 0,  pixels );
			for ( i = 0 ; i < pixels ; ++i )
			{
				assert( source[i] == 0xFF806040 );
			}
		}

		// palettized
		{
			// pal16
			uint32_t pal16[16];
			SurfaceFormat palfmt16( SurfaceFormat::SURFACE_A8R8G8B8 );
			for ( int i = 0 ; i < 16 ; ++i )
			{
				uint32_t v = (i*255)/15;
				pal16[i] = v+(v<<8)+(v<<16)+(0xFF<<24);
			}
	
			// pal256
			uint32_t pal256[256];
			SurfaceFormat palfmt256( SurfaceFormat::SURFACE_A8R8G8B8 );
			for ( int i = 0 ; i < 256 ; ++i )
			{
				uint32_t v = (i*255)/255;
				pal256[i] = v+(v<<8)+(v<<16)+(0xFF<<24);
			}
			
			// pal16 <-> 32
			{
				uint8_t src[2] = {0x27, 0x1A};
				SurfaceFormat srcfmt( SurfaceFormat::SURFACE_P4 );
				
				uint32_t dst[4];
				SurfaceFormat dstfmt( SurfaceFormat::SURFACE_A8R8G8B8 );
				
				dstfmt.copyPixels( dst, SurfaceFormat::SURFACE_UNKNOWN, 0, srcfmt, src, palfmt16, pal16, 4 );
				
				/*assert( dst[0] == 0xFF777777 );
				assert( dst[1] == 0xFF222222 );
				assert( dst[2] == 0xFFAAAAAA );
				assert( dst[3] == 0xFF111111 );*/
				//for ( int i = 0 ; i < 4 ; ++i )
				//	Debug::printfln( "dst[%i] = 0x%X", i, dst[i] );

				uint8_t src2[2];
				srcfmt.copyPixels( src2, palfmt16, pal16, dstfmt, dst, SurfaceFormat::SURFACE_UNKNOWN, 0, 4 );
				//assert( !memcmp(src2,src,2) );
			}

			// pal256 <-> 32
			{
				uint8_t src[2] = {0x27, 0x1A};
				SurfaceFormat srcfmt( SurfaceFormat::SURFACE_P8 );
				
				uint32_t dst[2];
				SurfaceFormat dstfmt( SurfaceFormat::SURFACE_A8R8G8B8 );
				
				dstfmt.copyPixels( dst, SurfaceFormat::SURFACE_UNKNOWN, 0, srcfmt, src, palfmt256, pal256,  2 );
				
				assert( dst[0] == 0xFF272727 );
				assert( dst[1] == 0xFF1A1A1A );
				//for ( int i = 0 ; i < 2 ; ++i )
				//	Debug::printfln( "dst[%i] = 0x%X", i, dst[i] );

				uint8_t src2[2];
				srcfmt.copyPixels( src2, palfmt256, pal256, dstfmt, dst, SurfaceFormat::SURFACE_UNKNOWN, 0, 4 );
				assert( !memcmp(src2,src,2) );
			}
		}
	}
}

void test()
{
	String libname = "gr";

	Debug::printf( "\n-------------------------------------------------------------------------\n" );
	Debug::printf( "%s library test begin\n", libname.c_str() );
	Debug::printf( "-------------------------------------------------------------------------\n" );
	run();
	Debug::printf( "%s library test ok\n", libname.c_str() );
}


END_NAMESPACE() // gr

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
