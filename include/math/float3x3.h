#ifndef _MATH_MATRIX3X3_H
#define _MATH_MATRIX3X3_H


#include <lang/pp.h>
#include <math/float3.h>


BEGIN_NAMESPACE(math) 


class quaternion;


/**
 * 3x3 column-major matrix of scalar type float.
 * NOTE: The class behaves like C basic types what it comes to initialization
 * so default constructor leaves the components to unspecified value.
 * 
 * @ingroup math
 */
class float3x3
{
public:
	enum
	{ 
		/** Number of rows in a matrix.*/
		ROWS	= 3, 
		/** Number of columns in a matrix.*/
		COLUMNS	= 3, 
		/** Number of components in a matrix.*/
		SIZE	= 9,
	};

#ifdef _DEBUG
	/** Constructs undefined matrix.*/
	float3x3()																		{float v = getNaN(); for ( int i = 0 ; i < ROWS ; ++i ) for ( int j = 0 ; j < COLUMNS ; ++j ) m[i][j] = v;}
#else
	float3x3()																		{}
#endif

	/** Rotation matrix from axis-angle representation. */
	float3x3( const float3& axis, float angle );
	
	/** Matrix construction from elements. */
	float3x3(  float m00, float m01, float m02, 
		float m10, float m11, float m12,
		float m20, float m21, float m22 );

	/** Identity matrix multiplied with specified value. */
	explicit float3x3( float diagonal );
	
	/** Rotation matrix from the quaternion. */
	explicit float3x3( const quaternion& q );

	/** Component-wise addition of matrices.*/
	float3x3&		operator+=( const float3x3& other )							{for ( int i = 0 ; i < ROWS ; ++i ) for ( int j = 0 ; j < COLUMNS ; ++j ) m[i][j] += other.m[i][j]; return *this;}
	
	/** Component-wise subtraction of matrices.*/
	float3x3&		operator-=( const float3x3& other )							{for ( int i = 0 ; i < ROWS ; ++i ) for ( int j = 0 ; j < COLUMNS ; ++j ) m[i][j] -= other.m[i][j]; return *this;}
	
	/** Matrix multiplication.*/
	float3x3&		operator*=( const float3x3& other )							{return *this = *this * other;}
	
	/**	Multiplies all components of this matrix with a scalar.*/
	float3x3&		operator*=( float s )											{for ( int i = 0 ; i < ROWS ; ++i ) for ( int j = 0 ; j < COLUMNS ; ++j ) m[i][j] *= s; return *this;}

	/** Access element at (row,column) in the matrix.*/
	float&			operator()( int row, int column )								{return m[row][column];}
	
	/** Sets element at (row,column) in the matrix.*/
	void			set( int row, int column, float value )							{m[row][column]=value;}

	/** Sets ith column of the matrix. */
	void			setColumn( int i, const float3& c )							{m[0][i]=c[0]; m[1][i]=c[1]; m[2][i]=c[2];}

	/** Sets ith row of the matrix. */
	void			setRow( int i, const float3& c )								{m[i][0]=c[0]; m[i][1]=c[1]; m[i][2]=c[2];}

	/** 
	 * Generates orthonormal coordinate system about specified Z-axis. 
	 * @param axis Z-axis of space.
	 */
	void			generateOrthonormalBasisFromZ( const float3& axis );

	/** 
	 * Generates orthonormal coordinate system about specified Z-axis using preferred up-direction. 
	 * @param axis Z-axis of space.
	 */
	void			generateOrthonormalBasisFromZ( const float3& axis, const float3& up );

	/** Returns component-wise addition of matrices.*/
	float3x3		operator+( const float3x3& other ) const						{float3x3 a; for ( int i = 0 ; i < ROWS ; ++i ) for ( int j = 0 ; j < COLUMNS ; ++j ) a.m[i][j] = m[i][j] + other.m[i][j]; return a;}
	
	/** Returns component-wise subtraction of matrices.*/
	float3x3		operator-( const float3x3& other ) const						{float3x3 a; for ( int i = 0 ; i < ROWS ; ++i ) for ( int j = 0 ; j < COLUMNS ; ++j ) a.m[i][j] = m[i][j] - other.m[i][j]; return a;}

	/** Returns component-wise negation.*/
	float3x3		operator-() const												{float3x3 a; for ( int i = 0 ; i < ROWS ; ++i ) for ( int j = 0 ; j < COLUMNS ; ++j ) a.m[i][j] = -m[i][j]; return a;}
	
	/**	Returns this matrix multiplied with a scalar.*/
	float3x3		operator*( float s ) const										{float3x3 a; for ( int i = 0 ; i < ROWS ; ++i ) for ( int j = 0 ; j < COLUMNS ; ++j ) a.m[i][j] = m[i][j] * s; return a;}
	
	/** Matrix multiplication.*/
	float3x3		operator*( const float3x3& other ) const;

	/* Matrix multiplication with 3-vector as 1-column matrix.*/
	float3			operator*( const float3& v ) const;
	
	/** Component-wise equality.*/
	bool			operator==( const float3x3& other ) const						{for ( int i = 0 ; i < ROWS ; ++i ) for ( int j = 0 ; j < COLUMNS ; ++j ) if ( m[i][j] != other.m[i][j] ) return false; return true;}
	
	/** Component-wise inequality.*/
	bool			operator!=( const float3x3& other ) const						{for ( int i = 0 ; i < ROWS ; ++i ) for ( int j = 0 ; j < COLUMNS ; ++j ) if ( m[i][j] != other.m[i][j] ) return true; return false;}

	/** Access element at (row,column) in the matrix.*/
	const float&	operator()( int row, int column ) const							{return m[row][column];}

	/** Returns element at (row,column) in the matrix.*/
	float			get( int row, int column ) const								{return m[row][column];}

	/** Returns ith column of the matrix. */
	float3			getColumn( int i ) const										{float3 d; d[0]=m[0][i]; d[1]=m[1][i]; d[2]=m[2][i]; return d;}

	/** Returns ith row of the matrix. */
	const float3&	getRow( int i ) const											{return *reinterpret_cast<const float3*>(m[i]);} //{float3 d; d[0]=m[i][0]; d[1]=m[i][1]; d[2]=m[i][2]; return d;}

	/** Returns determinant of the matrix. */
	float			determinant() const;

	/** Returns transposed matrix. */
	float3x3		transpose() const;

	/** Returns inverse of the matrix. */
	float3x3		inverse() const;

	/** Returns the matrix with its column vectors orthonormalized. */
	float3x3		orthonormalize() const;

	/** Multiplies the matrix and the 3-vector. */
	void			rotate( const float3& v, float3* v1 ) const;

	/** Multiplies the matrix and the 3-vector. */
	float3			rotate( const float3& v ) const;

	/** Returns true if all components are in finite range. */
	bool			finite() const;

private:
	float			m[ROWS][COLUMNS];
};


END_NAMESPACE() // math


#endif // _MATH_MATRIX3X3_H

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
