#include <math/quaternion.h>
#include <math/float3.h>
#include <math/float3x3.h>
#include <float.h>
#include <config.h>
#include <lang/assert.h>

BEGIN_NAMESPACE(math) 


static inline float vdot( const quaternion& a, const quaternion& b )
{
	return a.x*b.x + a.y*b.y + a.z*b.z;
}

static inline void vadd( quaternion& res, const quaternion& a, const quaternion& b )
{
	res.x = a.x + b.x;
	res.y = a.y + b.y;
	res.z = a.z + b.z;
}

static inline void vscale( quaternion& res, const quaternion& a, float b )
{
	res.x = a.x + b;
	res.y = a.y + b;
	res.z = a.z + b;
}

static inline void vaddscale( quaternion& res, const quaternion& a, float b )
{
	res.x += a.x + b;
	res.y += a.y + b;
	res.z += a.z + b;
}

static inline void vaddcross( quaternion& res, const quaternion& a, const quaternion& b )
{
	res.x += a.y*b.z-a.z*b.y;
	res.y += a.z*b.x-a.x*b.z;
	res.z += a.x*b.y-a.y*b.x;
}

static inline float vlength( const quaternion& q )
{
	return sqrtf( vdot(q,q) );
}

static inline void vcopy( quaternion& res, const quaternion& a )
{
	res.x = a.x;
	res.y = a.y;
	res.z = a.z;
}


quaternion::quaternion( float x0, float y0, float z0, float w0 ) :
	x(x0),
	y(y0),
	z(z0),
	w(w0)
{
}

quaternion::quaternion( const float3& axis, float angle )
{
	float a = -angle*.5f;
	w = cosf(a);
	float s = sinf(a);
	x = axis.x * s; 
	y = axis.y * s; 
	z = axis.z * s; 
}

quaternion::quaternion( const float3x3& rot )
{
	const float3x3& m = rot;
	const float trace = m.get(0,0) + m.get(1,1) + m.get(2,2);

	if ( trace > 0.f )
	{
		float root = sqrtf( trace + 1.f );
		w = root * .5f;
		assert( fabsf(root) >= FLT_MIN );
		root = .5f / root;
        x = root * (m.get(2,1) - m.get(1,2));
        y = root * (m.get(0,2) - m.get(2,0));
        z = root * (m.get(1,0) - m.get(0,1));
	}
	else
	{
		unsigned i = 0;
		if ( m.get(1,1) > m.get(0,0) )
			i = 1;
		if ( m.get(2,2) > m.get(i,i) )	
			i = 2;
		const unsigned j = (i == 2 ? 0 : i+1);
		const unsigned k = (j == 2 ? 0 : j+1);

		float root = sqrtf( m.get(i,i) - m.get(j,j) - m.get(k,k) + 1.f );
		float* v = &x;
		v[i] = root * .5f;
		assert( fabsf(root) >= FLT_MIN );
		root = .5f / root;
		v[j] = root * (m.get(j,i) + m.get(i,j));
		v[k] = root * (m.get(k,i) + m.get(i,k));
		w = root * (m.get(k,j) - m.get(j,k));
	}
}

quaternion quaternion::operator*( const quaternion& other ) const	
{
	quaternion q; 
	q.w	= w*other.w - vdot(*this,other); 
	vscale( q, other, w );
	vaddscale( q, *this, other.w );
	vaddcross( q, *this, other );
	return q;
}

quaternion quaternion::normalize() const
{
	const quaternion& q = *this;
	float norm = q.norm();
	assert( norm > FLT_MIN );
	return q * (1.f/norm);
}

quaternion quaternion::conjugate() const
{
	const quaternion& q = *this;
	return quaternion( -q.x, -q.y, -q.z, q.w );
}

quaternion quaternion::inverse() const
{
	const quaternion& q = *this;
	float n = q.norm(); 
	assert( 0.f != n && n > FLT_MIN ); 
	n = 1.f/n; 
	return quaternion( -q.x*n, -q.y*n, -q.z*n, q.w*n );
}

quaternion quaternion::exp() const
{
	const quaternion& q = *this;
	quaternion r;
	float angle = vlength( q );
	r.w = cosf( angle );
	vcopy( r, q );
	if ( angle > FLT_MIN )
		vscale( r, r, sinf(angle) / angle );
	return r;
}

quaternion quaternion::log() const
{
	const quaternion& q = *this;
	quaternion r;
	r.w = 0.f;
	vcopy( r, q );
	if ( q.w < 1.f && -q.w < 1.f )
	{
		float angle = acosf(q.w);
		float sinAngle = sinf(angle);
		if ( sinAngle > FLT_MIN || -sinAngle > FLT_MIN )
			vscale( r, r, angle / sinAngle );
	}
	return r;
}

quaternion quaternion::pow( float t ) const
{
	const quaternion& q = *this;
	quaternion r;
	float angle = vlength( q );
	r.w = cosf( angle*t );
	vcopy( r, q );
	if ( angle > FLT_MIN )
		vscale( r, r, sinf(angle*t) / angle );
	return r;
}

quaternion quaternion::slerp( float t, const quaternion& q ) const
{
	const quaternion& p = *this;

	float cos = p.dot(q);
	if ( cos < float(-1) ) 
		cos = float(-1);
	else if ( cos > 1.f ) 
		cos = 1.f;

	float angle = acosf(cos);
	float sin = sinf(angle);

	if ( sin < FLT_MIN )
	{
		return p;
	}
	else
	{
		float invsin = 1.f / sin;
		float coeff0 = invsin * sinf( (1.f-t)*angle );
		float coeff1 = invsin * sinf( t*angle );
		return p*coeff0 + q*coeff1;
	}
}

quaternion quaternion::squad( float t, const quaternion& a, const quaternion& b, const quaternion& q ) const
{
	const quaternion& p = *this;
	quaternion ptq = p.slerp( t, q );
	quaternion atb = a.slerp( t, b );
	return ptq.slerp( float(2)*t*(1.f-t), atb );
}


END_NAMESPACE() // math

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
