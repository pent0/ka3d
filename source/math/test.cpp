#include <lang/all.h>
#include <math/all.h>
#include <math.h>
#include <stdio.h>
#include <config.h>


USING_NAMESPACE(lang)


BEGIN_NAMESPACE(math) 


static void test_Matrix4x4()
{
	// point-plane projection test
	{
		float3 pp( 0,0,0 );
		float3 pn( 0,1,0 );
		float3 p = float3(3,7,4);
		float3 l = p + float3(0,100,0);
		float4x4 proj;
		proj.setPointPlaneProjection( l, pn, pp );
		
		float4 v = float4(p.x,p.y,p.z,1.f);
		float4 p1 = proj * v;
		p1 *= 1.f / p1.w;
		assert( (float3(p1.x,p1.y,p1.z)-float3(p.x,0,p.z)).length() < 1e-3f );
	}

	// direct-plane projection test
	{
		float3 v( 3,7,4 );
		float3 l = normalize( float3( -1,-1,0 ) );
		float3 p( 0,0,0 );
		float3 n( 0,1,0 );

		float k = -dot(l,n);
		float3 v1 = l * ( dot(v-p,n)/k ) + v;

		float4x4 m;
		m.setDirectPlaneProjection( l, n, p );

		float4 vp = m * float4(v.x,v.y,v.z,1);
		vp *= 1.f / vp.w;
		assert( (float3(vp.x,vp.y,vp.z)-v1).length() < 1e-3f );
	}

	// orthonormal basis & 
	// matrix multiply/inverse/subract/identity test
	{
		float3 axis	= normalize( float3(0.214791f, 0.973763f, -0.0751746f) );
		float3x3 rot;
		rot.generateOrthonormalBasisFromZ( axis );
		assert( fabsf(dot(rot.getColumn(0),rot.getColumn(1))) < 1e-4f );
		assert( fabsf(dot(rot.getColumn(2),rot.getColumn(1))) < 1e-4f );
		assert( fabsf(dot(rot.getColumn(0),rot.getColumn(2))) < 1e-4f );
		
		float4x4 m1(1.f);
		m1.setRotation( rot );
		
		float4x4 m2 = m1.inverse();
		float4x4 m3 = m1 * m2 - float4x4(1.f);
		float err = m3.getColumn(0).length() + m3.getColumn(1).length() + m3.getColumn(2).length() + m3.getColumn(3).length(); err=err;
		assert( err < 1e-6f );
	}
}

static void test_Matrix3x4()
{
	// orthonormal basis & 
	// matrix multiply/inverse/subract/identity test
	{
		float3 axis	= normalize( float3(0.214791f, 0.973763f, -0.0751746f) );
		float3x3 rot;
		rot.generateOrthonormalBasisFromZ( axis );
		assert( fabsf(dot(rot.getColumn(0),rot.getColumn(1))) < 1e-4f );
		assert( fabsf(dot(rot.getColumn(2),rot.getColumn(1))) < 1e-4f );
		assert( fabsf(dot(rot.getColumn(0),rot.getColumn(2))) < 1e-4f );
		
		float3x4 m1(1.f);
		m1.setRotation( rot );
		
		float3x4 m2 = m1.inverse();
		float3x4 m3 = m1 * m2 - float3x4(1.f);
		float err = m3.getColumn(0).length() + m3.getColumn(1).length() + m3.getColumn(2).length() + m3.getColumn(3).length(); err=err;
		//Debug::printf( "err=%f\n", err );
		assert( err < 1e-6f );
	}
}

static void test_InterpolationUtil()
{
	// check that oscillation works
	{
		String oscstr;
		int keyframes = 10;
		float keyrate = 5.f;
		for ( float time = 0.f ; time < 10.f ; time += 1.f/keyrate*.3f )
		{
			InterpolationUtil::BehaviourType endbehaviour = InterpolationUtil::BEHAVIOUR_OSCILLATE;
			int frames[4] = {-1,-1,-1,-1}; float u=-1.f;
			InterpolationUtil::getFrame( time+0.01f, endbehaviour, keyframes, keyrate, frames, &u );
			oscstr = oscstr + Format("{0,#.00},",frames[1]+u).format();
		}
	}

	// check others
	{
		int keyframes = 10;
		float keyrate = 5.f;

		float times[] =
		{
			3.15f,
			1.15f,
			3.35f,
			3.f,
			3.f,
		};
		InterpolationUtil::BehaviourType endbehaviours[] =
		{
			InterpolationUtil::BEHAVIOUR_REPEAT,
			InterpolationUtil::BEHAVIOUR_OSCILLATE,
			InterpolationUtil::BEHAVIOUR_OSCILLATE,
			InterpolationUtil::BEHAVIOUR_RESET,
			InterpolationUtil::BEHAVIOUR_STOP,
		};
		int resultframes[][4] =
		{
			{4,5,6,7},
			{4,5,6,7},
			{4,3,2,1},
			{0,0,0,0},
			{9,9,9,9},
		};
		float resultu[] =
		{
			0.75f,
			0.75f,
			0.75f,
			0.f,
			1.f,
		};

		for ( int i = 0 ; i < int(sizeof(times)/sizeof(times[0])) ; ++i )
		{
			float time = times[i];
			InterpolationUtil::BehaviourType endbehaviour = endbehaviours[i];
			int frames[4] = {-1,-1,-1,-1}; float u=-1.f;
			InterpolationUtil::getFrame( time, endbehaviour, keyframes, keyrate, frames, &u );
			assert( resultu[i] == u );
			if ( !( resultu[i] == u ) )
				throwError( Exception( Format("Error at {0}({1})", __FILE__, __LINE__) ) );

			for ( int k = 0 ; k < 4 ; ++k )
			{
				assert( resultframes[i][k] == frames[k] );
				if ( !( resultframes[i][k] == frames[k] ) )
					throwError( Exception( Format("Error at {0}({1})", __FILE__, __LINE__) ) );
			}
		}
	}
}

static void run()
{
	test_Matrix4x4();
	test_Matrix3x4();
	test_InterpolationUtil();
}

void test()
{
	String libname = "math";

	Debug::printf( "\n-------------------------------------------------------------------------\n" );
	Debug::printf( "%s library test begin\n", libname.c_str() );
	Debug::printf( "-------------------------------------------------------------------------\n" );
	run();
	Debug::printf( "%s library test ok\n", libname.c_str() );
}


END_NAMESPACE() // math

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
