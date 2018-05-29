/*
TODO: cleanup anim optimization prototype code (LOW)
*/
#include <hgr/TransformAnimation.h>
#include <gr/GraphicsException.h>
#include <img/Image.h>
#include <lang/Math.h>
#include <lang/Float.h>
#include <lang/Array.h>
#include <lang/Debug.h>
#include <lang/String.h>
#include <lang/OutOfMemoryException.h>
#include <math/float3.h>
#include <math/float3x3.h>
#include <math/float3x4.h>
#include <math/quaternion.h>
#include <math/InterpolationUtil.h>
#include <string.h>
#include <stdio.h>
#include <config.h>


//#define DUMP_KEYS


USING_NAMESPACE(gr)
USING_NAMESPACE(img)
USING_NAMESPACE(lang)
USING_NAMESPACE(math)


BEGIN_NAMESPACE(hgr) 


TransformAnimation::TransformAnimation( BehaviourType endbehaviour, 
	int poskeys, int rotkeys, int sclkeys,
	int poskeyrate, int rotkeyrate, int sclkeyrate ) :
	m_endTime( float(poskeys)*float(poskeyrate) ),
	m_pos( 0 ),
	m_rot( 0 ),
	m_scl( 0 ),
	m_poskeyrate( (uint8_t)poskeyrate ),
	m_rotkeyrate( (uint8_t)rotkeyrate ),
	m_sclkeyrate( (uint8_t)sclkeyrate ),
	m_endbehaviour( (uint8_t)endbehaviour )
{
	assert( rotkeys > 0 );
	assert( poskeys > 0 );

	m_pos = new KeyframeSequence( poskeys, VertexFormat::DF_V3_32 );
	m_rot = new KeyframeSequence( rotkeys, VertexFormat::DF_V4_32 );
	if ( sclkeys > 0 )
		m_scl = new KeyframeSequence( sclkeys, VertexFormat::DF_V3_32 );

	for ( int i = 0 ; i < poskeys ; ++i )
		setPositionKey( i, float3(0,0,0) );
	for ( int i = 0 ; i < rotkeys ; ++i )
		setRotationKey( i, quaternion(0,0,0,1) );
	for ( int i = 0 ; i < sclkeys ; ++i )
		setScaleKey( i, float3(1,1,1) );
}

TransformAnimation::TransformAnimation( BehaviourType endbehaviour,
	Float3Anim* posanim, KeyframeSequence* rotkeys, Float3Anim* sclanim,
	int poskeyrate, int rotkeyrate, int sclkeyrate, float endtime ) :
	m_endTime( endtime ),
	m_posAnim( posanim ),
	m_rot( rotkeys ),
	m_sclAnim( sclanim ),
	m_poskeyrate( (uint8_t)poskeyrate ),
	m_rotkeyrate( (uint8_t)rotkeyrate ),
	m_sclkeyrate( (uint8_t)sclkeyrate ),
	m_endbehaviour( (uint8_t)endbehaviour )
{
}

static float getMin( const Array<float>& a )
{
	if ( a.size() < 1 )
		return 0.f;
	float v = a[0];
	for ( int i = 1 ; i < a.size() ; ++i )
	{
		if ( a[i] < v )
			v = a[i];
	}
	return v;
}

static float getMax( const Array<float>& a )
{
	if ( a.size() < 1 )
		return 0.f;
	float v = a[0];
	for ( int i = 1 ; i < a.size() ; ++i )
	{
		if ( a[i] > v )
			v = a[i];
	}
	return v;
}

static float sign( float x )
{
	return x < 0.f ? -1.f : 1.f;
}

static void diff( const Array<float>& frameval, Array<float>& dval )
{
	for ( int i = 1 ; i < frameval.size() ; ++i )
	{
		int iprev = (i-1+frameval.size()) % frameval.size();
		dval[i] = frameval[i] - frameval[iprev];
	}
}

float evalBezier4( float t, const float* p )
{
	float cx = 3.f * (p[1] - p[0]);
	float bx = 3.f * (p[2] - p[1]) - cx;
	float ax = p[3] - p[0] - cx - bx;
	float t2 = t * t;
	return ax*(t2*t) + bx*t2  + cx*t + p[0];
}

static float sampleLinear( float t, const Array<float>& keytime, const Array<float>& keyval )
{
	assert( keytime.size() == keyval.size() );
	assert( keytime.size() > 0 );
	assert( keyval.size() > 0 );
	assert( t >= 0.f );

	if ( keytime.size() < 2 )
	{
		return keyval[0];
	}

	const int ilast = keytime.size()-1;
	for ( int i = 0 ; i < ilast ; ++i )
	{
		int inext = (i == ilast ? 0 : i+1);
		float val0 = keyval[i];
		float val1 = keyval[inext];
		float t0 = keytime[i];
		float t1 = keytime[inext];
		if ( i == ilast )
			t1 += keytime.last();

		if ( t >= t0 && t < t1 )
		{
			float u = (t-t0)/(t1-t0);
			float dval = val1 - val0;
			return val0 + u*dval;
		}
	}

	return keyval.last();
}

static float3 sampleLinear( float t, const Array<float4>& keys )
{
	assert( keys.size() > 0 );
	assert( t >= 0.f );

	if ( keys.size() < 2 )
		return keys[0].xyz();

	if ( t > keys.last().w )
		t = fmod( t, keys.last().w );

	const int ilast = keys.size()-1;
	for ( int i = 0 ; i < ilast ; ++i )
	{
		int inext = (i == ilast ? 0 : i+1);
		float t0 = keys[i].w;
		float t1 = keys[inext].w;
		if ( i == ilast )
			t1 += keys.last().w;

		if ( t >= t0 && t < t1 )
		{
			float3 val0 = keys[i].xyz();
			float3 val1 = keys[inext].xyz();
			float u = (t-t0)/(t1-t0);
			float3 dval = val1 - val0;
			return val0 + dval*u;
		}
	}

	return keys.last().xyz();
}

/*static float sampleCatmullRom( float t, const Array<float>& keytime, const Array<float>& keyval )
{
	assert( keytime.size() == keyval.size() );
	assert( keytime.size() > 0 );
	assert( keyval.size() > 0 );
	assert( t >= 0.f );

	const int ilast = keytime.size()-1;
	for ( int i = 0 ; i < ilast ; ++i )
	{
		int iprev = (i == 0 ? ilast : i-1);
		int inext = (i == ilast ? 0 : i+1);
		int inextnext = (inext == ilast ? 0 : inext+1);

		float v0 = keyval[iprev];
		float v1 = keyval[i];
		float v2 = keyval[inext];
		float v3 = keyval[inextnext];
		float t0 = keytime[iprev];
		float t1 = keytime[i];
		float t2 = keytime[inext];
		float t3 = keytime[inextnext];
		if ( iprev > i )
			t0 -= keytime.last();
		if ( inext < i )
			t2 += keytime.last();
		if ( inextnext < i )
			t3 += keytime.last();

		if ( t >= t1 && t < t2 )
		{
			float u = (t-t1)/(t2-t1);
			float val = 0.f;
			InterpolationUtil::interpolateVectorCatmullRom( 1, u, &v0, &v1, &v2, &v3, &val );
			return val;
		}
	}

	assert( false );
	return keyval.last();
}*/

static void insertKey( Array<float>& keytime, Array<float>& keyval, float t, float v )
{
	bool inserted = false;
	if ( keytime.size() > 0 )
	{
		for ( int i = 0 ; i < keytime.size() ; ++i )
		{
			if ( t < keytime[i] )
			{
				keytime.add( i, t );
				keyval.add( i, v );
				inserted = true;
				break;
			}
		}
	}
	if ( !inserted )
	{
		keytime.add( t );
		keyval.add( v );
	}
}

static void resample( float keyrate, const Array<float>& frameval, 
	Array<float>& keytime, Array<float>& keyval, int quality )
{
	keytime.resize( 0 );
	keyval.resize( 0 );

	// d' and d''
	Array<float> dval( frameval.size() );
	diff( frameval, dval );
	Array<float> ddval( frameval.size() );
	diff( dval, ddval );

	// find points when d' or d'' == 0
	keytime.add( 0 );
	keyval.add( frameval[0] );
	for ( int i = 1 ; i < frameval.size() ; ++i )
	{
		//int iprev = (i-1+frameval.size()) % frameval.size();
		int inext = (i+1) % frameval.size();

		float t = float(i)/keyrate;
		float dval0 = dval[i]; //float dval0 = frameval[i] - frameval[iprev];
		float dval1 = dval[inext]; //float dval1 = frameval[inext] - frameval[i];
		float d_eps = 1e-3f;

		float ddval0 = ddval[i];
		float ddval1 = ddval[inext];
		float dd_eps = 1e-4f;

		if ( sign(dval0) != sign(dval1) || 
			(Math::abs(dval0) < d_eps && Math::abs(dval1) > d_eps) ||
			(Math::abs(dval0) > d_eps && Math::abs(dval1) < d_eps) ||
			sign(ddval0) != sign(ddval1) || 
			(Math::abs(ddval0) < dd_eps && Math::abs(ddval1) > dd_eps) ||
			(Math::abs(ddval0) > dd_eps && Math::abs(ddval1) < dd_eps) )
		{
			keytime.add( t );
			keyval.add( frameval[i] );
		}
	}
	keytime.add( float(frameval.size())/keyrate );
	if ( Math::abs(frameval.last()-frameval[frameval.size()-2]) < Math::abs(frameval.last()-frameval[0]) )
		keyval.add( frameval.last() );
	else
		keyval.add( frameval[0] );

	// insert n keys to the animation positions with biggest errors
	for ( int q = 0 ; q < quality ; ++q )
	{
		float maxerr = 0.f;
		int maxerrpos = -1;

		for ( int i = 0 ; i < frameval.size() ; ++i )
		{
			float v = sampleLinear( float(i)/keyrate, keytime, keyval );
			float err = Math::abs( v - frameval[i] );
			if ( err > maxerr )
			{
				maxerr = err;
				maxerrpos = i;
			}
		}

		if ( maxerrpos < 0 ) // no error in animation, we're lucky...
			break;

		insertKey( keytime, keyval, float(maxerrpos)/keyrate, frameval[maxerrpos] );
		//Debug::printf( "Inserted key at %d\n", maxerrpos );
	}

#ifdef DUMP_KEYS
	static int totalkeys = 0;
	static int totalframes = 0;
	totalframes += frameval.size();
	totalkeys += keyval.size();
	Debug::printf( "totalframes(pos)=%d, totalkeys(pos)=%d\n", totalframes, totalkeys );
#endif
}

static void optimizeAnim( KeyframeSequence* poskeys, float keyrate, Array<float>* keytime, Array<float>* keyval, Array<float>* frameval, float quality )
{
	for ( int k = 0 ; k < 3 ; ++k )
	{
		keytime[k].clear();
		keyval[k].clear();
		frameval[k].resize( poskeys->keys() );
	}

	int keys = poskeys->keys();
	for ( int i = 0 ; i < keys ; ++i )
	{
		frameval[0][i] = poskeys->getKeyframe( i ).x;
		frameval[1][i] = poskeys->getKeyframe( i ).y;
		frameval[2][i] = poskeys->getKeyframe( i ).z;
	}

	if ( poskeys->keys() < 2 )
	{
		for ( int k = 0 ; k < 3 ; ++k )
		{
			keytime[k].add( 0.f );
			keyval[k].add( frameval[k][0] );
		}
		return;
	}

	float maxv[3];
	float minv[3];
	for ( int k = 0 ; k < 3 ; ++k )
	{
		minv[k] = getMin( frameval[k] );
		maxv[k] = getMax( frameval[k] );
	}

	int extrakeys = int( quality * (float)frameval[0].size() );
	for ( int k = 0 ; k < 3 ; ++k )
		resample( keyrate, frameval[k], keytime[k], keyval[k], extrakeys );

#ifdef DUMP_KEYS
	// dump optimized
	static int count = -1;
	++count;

	int w = keys;
	Image pic( w, 102 );

	char fmt[256];
	char* homedir = getenv("HOME");
	assert( homedir );
	strcpy( fmt, homedir );
	strcat( fmt, "/temp/anims/anim%03db.png" );
	char buf[256];
	sprintf( buf, fmt, count );

	for ( int i = 0 ; i < keys ; ++i )
	{
		for ( int k = 0 ; k < 3 ; ++k )
		{
			float v = sampleLinear( float(i)/keyrate, keytime[k], keyval[k] );
			//float v = sampleCatmullRom( float(i)/keyrate, keytime[k], keyval[k] );

			if ( maxv[k] > minv[k] )
			{
				v -= minv[k];
				v /= maxv[k] - minv[k];
				v *= 100.f;
				v += 1.f;
				int color[3] = {0xFFFF0000,0xFF00FF00,0xFF0000FF};
				pic.setPixel( i, int(v), color[k] );
			}
		}
	}
	pic.save( buf );
	Debug::printf( "Wrote %s\n", buf );

	// dump original
	if ( poskeys->keys() > 1 )
	{
		Array<float> frameval[3];
		static int count = -1;
		++count;

		for ( int k = 0 ; k < 3 ; ++k )
			frameval[k].resize( poskeys->keys() );

		int keys = poskeys->keys();
		for ( int i = 0 ; i < keys ; ++i )
		{
			frameval[0][i] = poskeys->getKeyframe( i ).x;
			frameval[1][i] = poskeys->getKeyframe( i ).y;
			frameval[2][i] = poskeys->getKeyframe( i ).z;
		}

		float maxv[3];
		float minv[3];
		for ( int k = 0 ; k < 3 ; ++k )
		{
			minv[k] = getMin( frameval[k] );
			maxv[k] = getMax( frameval[k] );
		}

		int w = keys;
		Image pic( w, 102 );

		char fmt[256];
		char* homedir = getenv("HOME");
		assert( homedir );
		strcpy( fmt, homedir );
		strcat( fmt, "/temp/anims/anim%03d.png" );
		char buf[256];
		sprintf( buf, fmt, count );

		for ( int i = 0 ; i < keys ; ++i )
		{
			for ( int k = 0 ; k < 3 ; ++k )
			{
				float v = frameval[k][i];
				if ( maxv[k] > minv[k] )
				{
					v -= minv[k];
					v /= maxv[k] - minv[k];
					v *= 100.f;
					v += 1.f;
					int color[3] = {0xFFFF0000,0xFF00FF00,0xFF0000FF};
					pic.setPixel( i, int(v), color[k] );
				}
			}
		}
		pic.save( buf );
		Debug::printf( "Wrote %s\n", buf );
	}
#endif // DUMP_KEYS
}

static void insertKey( Array<float4>& anim, float t, const float3& v )
{
	bool inserted = false;
	if ( anim.size() > 0 )
	{
		for ( int i = 0 ; i < anim.size() ; ++i )
		{
			if ( t <= anim[i].w )
			{
				if ( t < anim[i].w )
					anim.add( i, float4(v,t) );
				inserted = true;
				break;
			}
		}
	}
	if ( !inserted )
		anim.add( float4(v,t) );
}

static void merge( TransformAnimation::Float3Anim* anim, 
	const Array<float>* keytime, const Array<float>* frameval, float keyrate )
{
	assert( keytime[0].size() > 0 );
	assert( keytime[1].size() > 0 );
	assert( keytime[2].size() > 0 );
	assert( frameval[0].size() > 0 );
	assert( frameval[1].size() > 0 );
	assert( frameval[2].size() > 0 );

	anim->keys.clear();
	
	int frames = frameval[0].size();
	Array<float> frametime( frames );
	for ( int i = 0 ; i < frames ; ++i )
		frametime[i] = float(i)/keyrate;

	for ( int k = 0 ; k < 3 ; ++k )
	{
		for ( int i = 0 ; i < keytime[k].size() ; ++i )
		{
			float t = keytime[k][i];
			insertKey( anim->keys, t, 
				float3( 
					sampleLinear(t,frametime,frameval[0]),
					sampleLinear(t,frametime,frameval[1]),
					sampleLinear(t,frametime,frameval[2]) ) );
		}
	}

	assert( anim->keys.size() > 0 );
}

TransformAnimation::TransformAnimation( BehaviourType endbehaviour,
	KeyframeSequence* poskeys, KeyframeSequence* rotkeys, KeyframeSequence* sclkeys,
	int poskeyrate, int rotkeyrate, int sclkeyrate ) :
	m_pos( poskeys ),
	m_rot( rotkeys ),
	m_scl( sclkeys ),
	m_poskeyrate( (uint8_t)poskeyrate ),
	m_rotkeyrate( (uint8_t)rotkeyrate ),
	m_sclkeyrate( (uint8_t)sclkeyrate ),
	m_endbehaviour( (uint8_t)endbehaviour )
{
	assert( rotkeys != 0 );
	assert( poskeys != 0 );

#ifdef DUMP_KEYS
	float maxv[3];
	float minv[3];
	for ( int k = 0 ; k < 3 ; ++k )
	{
		maxv[k] = getMax( frameval[k] );
		minv[k] = getMin( frameval[k] );
	}

	if ( poskeys->keys() > 1 )
	{
		static int count = -1;
		++count;

		int frames = frameval[0].size();
		int w = frames;
		Image pic( w, 102 );

		char fmt[256];
		char* homedir = getenv("HOME");
		assert( homedir );
		strcpy( fmt, homedir );
		strcat( fmt, "/temp/anims/anim%03dc.png" );
		char buf[256];
		sprintf( buf, fmt, count );

		for ( int i = 0 ; i < frames ; ++i )
		{
			float t = float(i)/float(poskeyrate);
			float3 vec = sampleLinear( t, m_posAnim->keys );

			for ( int k = 0 ; k < 3 ; ++k )
			{
				float v = vec[k];
				if ( maxv[k] > minv[k] )
				{
					v -= minv[k];
					v /= maxv[k] - minv[k];
					v *= 100.f;
					v += 1.f;
					int color[3] = {0xFFFF0000,0xFF00FF00,0xFF0000FF};
					pic.setPixel( i, int(v), color[k] );
				}
			}
		}
		pic.save( buf );
		Debug::printf( "Wrote %s\n", buf );
	}
#endif
}

TransformAnimation::~TransformAnimation()
{
}

void TransformAnimation::eval( float time, float3* pos, quaternion* rot, float3* scl )
{
	assert( positionKeys() > 0 );
	assert( rotationKeys() > 0 );

	int frames[4];
	float u;

	if ( !m_pos )
	{
		*pos = sampleLinear( time, m_posAnim->keys );
	}
	else
	{
		InterpolationUtil::getFrame( time, (InterpolationUtil::BehaviourType)endBehaviour(),
			positionKeys(), m_poskeyrate, frames, &u );

		float3 posk0 = getPositionKey(frames[0]);
		float3 posk1 = getPositionKey(frames[1]);
		float3 posk2 = getPositionKey(frames[2]);
		float3 posk3 = getPositionKey(frames[3]);
		InterpolationUtil::interpolateVectorCatmullRom( 3, u,
			&posk0.x, &posk1.x, &posk2.x, &posk3.x,
			&pos->x );
	}

	// rotation
	InterpolationUtil::getFrame( time, 
		(InterpolationUtil::BehaviourType)endBehaviour(),
		rotationKeys(), m_rotkeyrate, frames, &u );

	quaternion rotq1 = getRotationKey( frames[1] );
	quaternion rotq2 = getRotationKey( frames[2] );
	if ( rotq1.dot(rotq2) < 0.f )
		rotq2 = -rotq2;
	*rot = rotq1.slerp( u, rotq2 ).normalize();

	// scale
	if ( scaleKeys() > 0 )
	{
		if ( !m_scl )
		{
			*scl = sampleLinear( time, m_sclAnim->keys );
		}
		else
		{
			InterpolationUtil::getFrame( time, (InterpolationUtil::BehaviourType)endBehaviour(),
				scaleKeys(), m_sclkeyrate, frames, &u );

			float3 scalek0 = getScaleKey(frames[0]);
			float3 scalek1 = getScaleKey(frames[1]);
			float3 scalek2 = getScaleKey(frames[2]);
			float3 scalek3 = getScaleKey(frames[3]);
			InterpolationUtil::interpolateVectorCatmullRom( 3, u, &scalek0.x, &scalek1.x, &scalek2.x, &scalek3.x, &scl->x );
		}
	}
	else
	{
		scl->x = 1.f;
		scl->y = 1.f;
		scl->z = 1.f;
	}
}

void TransformAnimation::eval( float time, float3x4* tm )
{
	float3 pos, scl;
	quaternion rotq;
	eval( time, &pos, &rotq, &scl );
	*tm = float3x4( rotq, pos, scl );
}

void TransformAnimation::setEndBehaviour( BehaviourType endbehaviour )
{
	m_endbehaviour = (uint8_t)endbehaviour;
}

void TransformAnimation::setPositionKey( int i, const float3& v )
{
	float iscale = 1.f / m_pos->scale();
	m_pos->setKeyframe( i, &v.x, sizeof(v), VertexFormat::DF_V4_32, iscale, m_pos->bias()*-iscale, 1 );
}

void TransformAnimation::setRotationKey( int i, const quaternion& v )
{
	float iscale = 1.f / m_rot->scale();
	m_rot->setKeyframe( i, &v.x, sizeof(v), VertexFormat::DF_V4_32, iscale, m_rot->bias()*-iscale, 1 );
}

void TransformAnimation::setScaleKey( int i, const float3& v )
{
	float iscale = 1.f / m_scl->scale();
	m_scl->setKeyframe( i, &v.x, sizeof(v), VertexFormat::DF_V3_32, iscale, m_scl->bias()*-iscale, 1 );
}

void TransformAnimation::removeRedundantKeys()
{
	if ( m_pos )
		m_pos->removeRedundantKeys();
	if ( m_rot )
		m_rot->removeRedundantKeys();
	if ( m_scl )
		m_scl->removeRedundantKeys();
}

void TransformAnimation::optimize( float quality )
{
	Array<float> keytime[3];
	Array<float> keyval[3];
	Array<float> frameval[3];

	removeRedundantKeys();

	if ( m_pos )
	{
		optimizeAnim( m_pos, (float)m_poskeyrate, keytime, keyval, frameval, quality );
		m_posAnim = new Float3Anim();
		merge( m_posAnim, keytime, frameval, (float)m_poskeyrate );
		m_pos = 0;
	}

	if ( m_scl )
	{
		optimizeAnim( m_scl, (float)m_sclkeyrate, keytime, keyval, frameval, quality );
		m_sclAnim = new Float3Anim();
		merge( m_sclAnim, keytime, frameval, (float)m_sclkeyrate );
		m_scl = 0;
	}
}

void TransformAnimation::rotate( const float3x3& rot )
{
	for ( int i = 0 ; i < rotationKeys() ; ++i )
	{
		quaternion q0 = getRotationKey( i );
		quaternion q1( rot * float3x3(q0) );
		setRotationKey( i, q1 );
	}

	for ( int i = 0 ; i < positionKeys() ; ++i )
	{
		float3 v = getPositionKey( i );
		v = rot.rotate(v);
		setPositionKey( i, v );
	}
}

void TransformAnimation::scale( float s )
{
	m_pos->setScale( m_pos->scale()*s );
	m_pos->setBias( m_pos->bias()*s );

	if ( m_scl )
	{
		m_scl->setScale( m_scl->scale()*s );
		m_scl->setBias( m_scl->bias()*s );
	}
}

float3 TransformAnimation::getLinearVelocity( float time ) const
{
	if ( positionKeys() < 2 )
		return float3(0,0,0);

	if ( m_pos )
	{
		int frame0 = int( time * m_poskeyrate ) % positionKeys();
		int frame1 = frame0 + 1;
		if ( frame1 >= positionKeys() )
		{
			--frame0;
			--frame1;
		}
		assert( frame0 >= 0 && frame0 < positionKeys() );
		assert( frame1 >= 0 && frame1 < positionKeys() );

		float3 p0 = getPositionKey( frame0 );
		float3 p1 = getPositionKey( frame1 );

		p1 -= p0;
		p1 *= m_poskeyrate;
		return p1;
	}
	else
	{
		float time0 = time;
		float time1 = time + (1.f/float(m_poskeyrate));

		float3 p0 = sampleLinear( time0, m_posAnim->keys );
		float3 p1 = sampleLinear( time1, m_posAnim->keys );

		p1 -= p0;
		p1 *= m_poskeyrate;
		return p1;
	}
}

TransformAnimation::BehaviourType TransformAnimation::toBehaviour( const String& str )
{
	const char* sz[] = 
	{
		"REPEAT",
		"OSCILLATE",
		"RESET",
		"STOP" 
	};
	assert( sizeof(sz)/sizeof(sz[0]) == BEHAVIOUR_COUNT );

	String upper = str.toUpperCase();
	for ( int i = 0 ; i < BEHAVIOUR_COUNT ; ++i )
		if ( upper == sz[i] )
			return (BehaviourType)i;

	throwError( GraphicsException( Format("Invalid animation end behavior: {0}", str) ) );
	return BEHAVIOUR_COUNT;
}

float3 TransformAnimation::getPositionKey( int i ) const
{
	float3 v;
	m_pos->getKeyframe( i, &v.x, sizeof(v), VertexFormat::DF_V3_32, 1 );
	return v;
}

quaternion TransformAnimation::getRotationKey( int i ) const
{
	quaternion v;
	m_rot->getKeyframe( i, &v.x, sizeof(v), VertexFormat::DF_V4_32, 1 );
	return v;
}

float3 TransformAnimation::getScaleKey( int i ) const
{
	float3 v;
	m_scl->getKeyframe( i, &v.x, sizeof(v), VertexFormat::DF_V3_32, 1 );
	return v;
}

int	TransformAnimation::positionKeys() const
{
	return m_pos != 0 ? m_pos->keys() : m_posAnim->keys.size();
}

int TransformAnimation::rotationKeys() const
{
	return m_rot != 0 ? m_rot->keys() : 0;
}

int TransformAnimation::scaleKeys() const
{
	return m_scl != 0 ? m_scl->keys() : m_sclAnim->keys.size();
}

int TransformAnimation::positionKeyRate() const
{
	return m_poskeyrate;
}

int TransformAnimation::rotationKeyRate() const
{
	return m_rotkeyrate;
}

int TransformAnimation::scaleKeyRate() const
{
	return m_sclkeyrate;
}

TransformAnimation::BehaviourType TransformAnimation::endBehaviour() const	
{
	return (BehaviourType)m_endbehaviour;
}


END_NAMESPACE() // hgr
