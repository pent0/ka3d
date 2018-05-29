#ifndef _MATH_DOMAIN_H
#define _MATH_DOMAIN_H


#include <lang/pp.h>
#include <math/float3.h>


BEGIN_NAMESPACE(math) 


/**
 * Domain specifies distribution for a random variable.
 * @ingroup math
 */
class Domain
{
public:
	/**
	 * Type of domain.
	 */
	enum DomainType
	{
		/** Undefined. Always (0,0,0). */
		DOMAIN_NONE,
		/** Scalar constant. */
		DOMAIN_CONSTANT,
		/** Scalar range. */
		DOMAIN_RANGE,
		/** Constant point. */
		DOMAIN_POINT,
		/** Sphere. */
		DOMAIN_SPHERE,
		/** Line. */
		DOMAIN_LINE,
		/** Box. */
		DOMAIN_BOX,
		/** Cylinder. */
		DOMAIN_CYLINDER,
		/** Disk. */
		DOMAIN_DISK,
		/** Rectangle. */
		DOMAIN_RECTANGLE,
		/** Triangle. */
		DOMAIN_TRIANGLE,
		/** Number of domain types. */
		DOMAIN_COUNT
	};

	/** Sets domain as undefined. */
	Domain();

	/** Sets domain as scalar constant. */
	void		setConstant( float c );

	/** Sets domain as scalar range. */
	void		setRange( float x0, float x1 );

	/** Sets domain as point. */
	void		setPoint( const float3& point );

	/** Sets domain as sphere. */
	void		setSphere( const float3& center, float inradius, float outradius );

	/** Sets domain as line. */
	void		setLine( const float3& start, const float3& end );

	/** Sets domain as box. */
	void		setBox( const float3& mindim, const float3& maxdim );

	/**	Sets domain as cylinder. */
	void		setCylinder( const float3& start, const float3& end, float inradius, float outradius );

	/** Sets domain as disk. */
	void		setDisk( const float3& origin, const float3& normal, float inradius, float outradius );

	/** Sets domain as rectangle. */
	void		setRectangle( const float3& origin, const float3& u, const float3& v );

	/** Sets domain as triangle. */
	void		setTriangle( const float3& v0, const float3& v1, const float3& v2 );

	/** Sets type of the domain. Remember to set parameters as well. */
	void		setType( DomainType type );

	/**
	 * Sets ith parameter of the domain definition.
	 */
	void		setParameter( int i, float value );

	/**
	 * Returns random 3-vector inside current domain.
	 * If the domain has less dimensions than the rest are filled with zero.
	 * When asking random value from undefined domain the result is always zero vector.
	 */
	float3		getRandomFloat3() const;

	/**
	 * Returns random scalar inside current domain.
	 * If the domain has more dimensions than one then the rest are discarded.
	 * When asking random value from undefined domain the result is always zero.
	 */
	float		getRandomFloat() const;

	/**
	 * Returns random 3-vector inside current domain.
	 * If the domain has less dimensions than the rest are filled with zero.
	 * If the domain has more dimensions than one then the rest are discarded.
	 * When asking random value from undefined domain the result is always zero.
	 */
	void		getRandom( float3* v ) const			{*v=getRandomFloat3();}

	/**
	 * Returns random scalar inside current domain.
	 * If the domain has less dimensions than the rest are filled with zero.
	 * If the domain has more dimensions than one then the rest are discarded.
	 * When asking random value from undefined domain the result is always zero.
	 */
	void		getRandom( float* v ) const				{*v=getRandomFloat();}

	/**
	 * Returns ith parameter of the domain definition.
	 */
	float		getParameter( int i ) const;

	/**
	 * Returns true if the domain is zero constant.
	 */
	bool		isZero() const							{return m_type == DOMAIN_CONSTANT && m_data.constant.c == 0.f;}

	/** 
	 * Returns current type of the domain. 
	 */
	DomainType	type() const							{return m_type;}

	/**
	 * Returns domain type as string.
	 */
	const char*	toString() const;

	/**
	 * Converts string to domain type.
	 * @return DOMAIN_COUNT if type not found.
	 */
	static DomainType	toDomainType( const char* str );

	/**
	 * Returns number of parameters used by specified domain type.
	 */
	static int			getParameterCount( DomainType type );

private:
	struct Constant
	{
		float c;
	};

	struct Range
	{
		float x0, x1;
	};

	struct Point
	{
		float x, y, z;
	};

	struct Sphere
	{
		float x, y, z;
		float r1, r2;
	};

	struct Line
	{
		float x1, y1, z1;
		float x2, y2, z2;
	};

	struct Box
	{
		float x1, y1, z1;
		float x2, y2, z2;
	};

	struct Cylinder
	{
		float x1, y1, z1;
		float x2, y2, z2;
		float r1, r2;
	};

	struct Disk
	{
		float ox, oy, oz;
		float nx, ny, nz;
		float r1, r2;
	};

	struct Rectangle
	{
		float ox, oy, oz;
		float ux, uy, uz;
		float vx, vy, vz;
	};

	struct Triangle
	{
		float ox, oy, oz;
		float ux, uy, uz;
		float vx, vy, vz;
	};

	union DomainData
	{
		Constant	constant;
		Range		range;
		Point		point;
		Sphere		sphere;
		Line		line;
		Box			box;
		Cylinder	cylinder;
		Disk		disk;
		Rectangle	rectangle;
		Triangle	triangle;
	};

	DomainData	m_data;
	DomainType	m_type;
};


END_NAMESPACE() // math


#endif // _MATH_DOMAIN_H

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
