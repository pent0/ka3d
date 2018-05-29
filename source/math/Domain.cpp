#include <math/Domain.h>
#include <math/RandomUtil.h>
#include <string.h>
#include <config.h>


BEGIN_NAMESPACE(math) 


static const char* const DOMAIN_NAMES[] =
{
	"NONE",
	"CONSTANT",
	"RANGE",
	"POINT",
	"SPHERE",
	"LINE",
	"BOX",
	"CYLINDER",
	"DISK",
	"RECTANGLE",
	"TRIANGLE"
};


Domain::Domain() :
	m_type( DOMAIN_NONE )
{
	m_data.triangle = Triangle();
}

void Domain::setConstant( float c )
{
	setType( DOMAIN_CONSTANT );
	m_data.constant.c = c;
}

void Domain::setRange( float x0, float x1 )
{
	setType( DOMAIN_CONSTANT );
	m_data.range.x0 = x0;
	m_data.range.x1 = x1;
}

void Domain::setPoint( const float3& point )
{
	setType( DOMAIN_POINT );
	m_data.point.x = point.x;
	m_data.point.y = point.y;
	m_data.point.z = point.z;
}

void Domain::setSphere( const float3& center, float inradius, float outradius )
{
	setType( DOMAIN_SPHERE );
	m_data.sphere.x = center.x;
	m_data.sphere.y = center.y;
	m_data.sphere.z = center.z;
	m_data.sphere.r1 = inradius;
	m_data.sphere.r2 = outradius;
}

void Domain::setLine( const float3& start, const float3& end )
{
	setType( DOMAIN_LINE );
	m_data.line.x1 = start.x;
	m_data.line.y1 = start.y;
	m_data.line.z1 = start.z;
	m_data.line.x2 = end.x;
	m_data.line.y2 = end.y;
	m_data.line.z2 = end.z;
}

void Domain::setBox( const float3& mindim, const float3& maxdim )
{
	setType( DOMAIN_BOX );
	m_data.box.x1 = mindim.x;
	m_data.box.y1 = mindim.y;
	m_data.box.z1 = mindim.z;
	m_data.box.x2 = maxdim.x;
	m_data.box.y2 = maxdim.y;
	m_data.box.z2 = maxdim.z;
}

void Domain::setCylinder( const float3& start, const float3& end, float inradius, float outradius )
{
	setType( DOMAIN_CYLINDER );
	m_data.cylinder.r1 = inradius;
	m_data.cylinder.r2 = outradius;
	m_data.cylinder.x1 = start.x;
	m_data.cylinder.y1 = start.y;
	m_data.cylinder.z1 = start.z;
	m_data.cylinder.x2 = end.x;
	m_data.cylinder.y2 = end.y;
	m_data.cylinder.z2 = end.z;
}

void Domain::setDisk( const float3& origin, const float3& normal, float inradius, float outradius )
{
	setType( DOMAIN_DISK );
	m_data.disk.nx = normal.x;
	m_data.disk.ny = normal.y;
	m_data.disk.nz = normal.z;
	m_data.disk.ox = origin.x;
	m_data.disk.oy = origin.y;
	m_data.disk.oz = origin.z;
	m_data.disk.r1 = inradius;
	m_data.disk.r2 = outradius;
}

void Domain::setRectangle( const float3& origin, const float3& u, const float3& v )
{
	setType( DOMAIN_RECTANGLE );
	m_data.rectangle.ox = origin.x;
	m_data.rectangle.oy = origin.y;
	m_data.rectangle.oz = origin.z;
	m_data.rectangle.ux = u.x;
	m_data.rectangle.uy = u.y;
	m_data.rectangle.uz = u.z;
	m_data.rectangle.vx = v.x;
	m_data.rectangle.vy = v.y;
	m_data.rectangle.vz = v.z;
}

void Domain::setTriangle( const float3& v0, const float3& v1, const float3& v2 )
{
	setType( DOMAIN_TRIANGLE );
	m_data.triangle.ox = v0.x;
	m_data.triangle.oy = v0.y;
	m_data.triangle.oz = v0.z;
	m_data.triangle.ux = v1.x-v0.x;
	m_data.triangle.uy = v1.y-v0.y;
	m_data.triangle.uz = v1.z-v0.z;
	m_data.triangle.vx = v2.x-v0.x;
	m_data.triangle.vy = v2.y-v0.y;
	m_data.triangle.vz = v2.z-v0.z;
}

float Domain::getRandomFloat() const
{
	return getRandomFloat3().x;
}

float3 Domain::getRandomFloat3() const
{
	assert( m_type < DOMAIN_COUNT );
	assert( m_type != DOMAIN_NONE ); // undefined domain? sounds programming error
	assert( DOMAIN_COUNT == 11 ); // make sure all cases are defined below

	switch ( m_type )
	{
	case DOMAIN_NONE:		return float3(0,0,0);
	case DOMAIN_CONSTANT:	return float3(m_data.constant.c, 0, 0);
	case DOMAIN_RANGE:		return float3(RandomUtil::getRandom(m_data.range.x0, m_data.range.x1), 0, 0);
	case DOMAIN_POINT:		return float3(m_data.point.x, m_data.point.y, m_data.point.z);
	case DOMAIN_SPHERE:		return RandomUtil::getPointInSphere( m_data.sphere.r1, m_data.sphere.r2 ) + float3(m_data.sphere.x, m_data.sphere.y, m_data.sphere.z);
	case DOMAIN_LINE:		return RandomUtil::getPointOnLine( float3(m_data.line.x1, m_data.line.y1, m_data.line.z1), float3(m_data.line.x2, m_data.line.y2, m_data.line.z2) );
	case DOMAIN_BOX:		return RandomUtil::getPointInBox( float3(m_data.box.x1, m_data.box.y1, m_data.box.z1), float3(m_data.box.x2, m_data.box.y2, m_data.box.z2) );
	case DOMAIN_CYLINDER:	return RandomUtil::getPointInCylinder( float3(m_data.cylinder.x1, m_data.cylinder.y1, m_data.cylinder.z1), float3(m_data.cylinder.x2, m_data.cylinder.y2, m_data.cylinder.z2), m_data.cylinder.r1, m_data.cylinder.r2 );
	case DOMAIN_DISK:		return RandomUtil::getPointOnDisk( float3(m_data.disk.ox, m_data.disk.oy, m_data.disk.oz), float3(m_data.disk.nx, m_data.disk.ny, m_data.disk.nz), m_data.disk.r1, m_data.disk.r2 );
	case DOMAIN_RECTANGLE:	return RandomUtil::getPointOnRectangle( float3(m_data.rectangle.ox, m_data.rectangle.oy, m_data.rectangle.oz), float3(m_data.rectangle.ux, m_data.rectangle.uy, m_data.rectangle.uz), float3(m_data.rectangle.vx, m_data.rectangle.vy, m_data.rectangle.vz) );
	case DOMAIN_TRIANGLE:	return RandomUtil::getPointOnTriangle( float3(m_data.rectangle.ox, m_data.rectangle.oy, m_data.rectangle.oz), float3(m_data.rectangle.ux, m_data.rectangle.uy, m_data.rectangle.uz), float3(m_data.rectangle.vx, m_data.rectangle.vy, m_data.rectangle.vz) );
	default:				assert( false ); // switch case doesnt cover all
	}

	return float3(0,0,0);
}

const char* Domain::toString() const
{
	return DOMAIN_NAMES[m_type];
}

Domain::DomainType Domain::toDomainType( const char* str )
{
	assert( DOMAIN_COUNT == sizeof(DOMAIN_NAMES)/sizeof(DOMAIN_NAMES[0]) );

	for ( int i = 0 ; i < DOMAIN_COUNT ; ++i )
		if ( !strcmp(str,DOMAIN_NAMES[i]) )
			return DomainType(i);

	return DOMAIN_COUNT;
}

int Domain::getParameterCount( DomainType type )
{
	assert( type < DOMAIN_COUNT ); // invalid domain
	assert( DOMAIN_COUNT == 11 ); // make sure all cases are defined below

	switch ( type )
	{
	case DOMAIN_NONE:		return 0;
	case DOMAIN_CONSTANT:	return sizeof(Constant)/sizeof(float);
	case DOMAIN_RANGE:		return sizeof(Range)/sizeof(float);
	case DOMAIN_POINT:		return sizeof(Point)/sizeof(float);
	case DOMAIN_SPHERE:		return sizeof(Sphere)/sizeof(float);
	case DOMAIN_LINE:		return sizeof(Line)/sizeof(float);
	case DOMAIN_BOX:		return sizeof(Box)/sizeof(float);
	case DOMAIN_CYLINDER:	return sizeof(Cylinder)/sizeof(float);
	case DOMAIN_DISK:		return sizeof(Disk)/sizeof(float);
	case DOMAIN_RECTANGLE:	return sizeof(Rectangle)/sizeof(float);
	case DOMAIN_TRIANGLE:	return sizeof(Triangle)/sizeof(float);
	default:				assert( false ); // switch case doesnt cover all
	}

	return 0;
}

void Domain::setType( DomainType type )
{
	assert( type < DOMAIN_COUNT );
	m_type = type;
}

void Domain::setParameter( int i, float value )
{
	assert( i >= 0 && i < getParameterCount(m_type) );
	float* param = reinterpret_cast<float*>( &m_data );
	param[i] = value;
}

float Domain::getParameter( int i ) const
{
	assert( i >= 0 && i < getParameterCount(m_type) );
	const float* param = reinterpret_cast<const float*>( &m_data );
	return param[i];
}


END_NAMESPACE() // math

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
