#include <hgr/Light.h>
#include <lang/Math.h>
#include <config.h>


USING_NAMESPACE(lang)
USING_NAMESPACE(math)


BEGIN_NAMESPACE(hgr) 


const float	Light::MAX_RANGE		= 1e9f;
const float	Light::MAX_CONE_ANGLE	= 3.13845106f;


Light::Light() :
	m_color( 1,1,1 ),
	m_farAttenStart( MAX_RANGE ),
	m_farAttenEnd( MAX_RANGE ),
	m_inner( 0.f ),
	m_outer( MAX_CONE_ANGLE ),
	m_innerCos( 1.f ),
	m_outerCos( Math::cos(MAX_CONE_ANGLE*.5f) )
{
	setClassId( NODE_LIGHT );
	setType( TYPE_OMNI );
}

Light::~Light()
{
}

Light::Light( const Light& other ) :
	Node( other ),
	m_color( other.m_color ),
	m_farAttenStart( other.m_farAttenStart ),
	m_farAttenEnd( other.m_farAttenEnd ),
	m_inner( other.m_inner ),
	m_outer( other.m_outer ),
	m_innerCos( other.m_innerCos ),
	m_outerCos( other.m_outerCos )
{
	setClassId( NODE_LIGHT );
}

Node* Light::clone() const
{
	return new Light( *this );
}

void Light::setColor( const float3& color )
{
	m_color = color;
}

void Light::setInnerCone( float angle )
{
	assert( angle >= 0.f && angle <= MAX_CONE_ANGLE );
	m_inner = angle;
	m_innerCos = Math::cos( angle );
}
	
void Light::setOuterCone( float angle )
{
	assert( angle >= 0.f && angle <= MAX_CONE_ANGLE );
	m_outer = angle;
	m_outerCos = Math::cos( angle );
}

void Light::setFarAttenStart( float farstart )
{
	assert( farstart >= 0.f );
	m_farAttenStart = Math::min( farstart, MAX_RANGE );
}

void Light::setFarAttenEnd( float farend )
{
	assert( farend >= 0.f );
	m_farAttenEnd = Math::min( farend, MAX_RANGE );
}

void Light::setType( Type type )
{
	setFlags( (flags() & ~NODE_LIGHTTYPE) + (type<<NODE_LIGHTTYPE_SHIFT) );
	assert( this->type() == type && "Light::setType failed" );
}

void Light::illuminate( const float3& point, 
	const float3& normal,
	float3* diffuse ) const
{
	*diffuse = float3(0,0,0);
	if ( !enabled() )
		return;

	// geometric (N,L) atten
	float3x4 worldtm = worldTransform();
	float3 L = worldtm.translation() - point;
	float dist = L.length();
	if ( dist >= m_farAttenEnd || dist < 1e-9f ) // early far atten optimization
		return;
	L *= 1.f/dist;
	float ndotl = dot( L, normal );
	if ( ndotl <= 0.f )
		return;
	float atten = ndotl;

	// distance (far) atten
	atten *= applyDistanceAtten( dist );

	// shape atten
	if ( TYPE_SPOT == type() )
		applyShapeAtten( -dot( normalize0(worldtm.getColumn(2)), L ) );

	*diffuse = m_color * atten;
}

float Light::applyShapeAtten( float cosa ) const
{
	float atten = 1.f;
	
	if ( TYPE_SPOT == type() )
	{
		if ( cosa < m_outerCos )
			atten = 0.f;
		else if ( cosa < m_innerCos )
		{
			float u = (cosa-m_outerCos)/(m_innerCos-m_outerCos);
			float shapeatten = u*u*(3.0f-2.0f*u);
			atten *= shapeatten;
		}
	}

	return atten;
}

float Light::applyDistanceAtten( float dist ) const
{
	float atten = 1.f;
	if ( m_farAttenEnd < MAX_RANGE ) 
	{
		if ( dist >= m_farAttenEnd )
		{
			atten = 0.f;
		}
		else if ( dist > m_farAttenStart )	
		{
			float u = (m_farAttenEnd-dist)/(m_farAttenEnd-m_farAttenStart);
			atten *= u*u*(3-2*u);
		}
	}
	return atten;
}


END_NAMESPACE() // hgr

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
