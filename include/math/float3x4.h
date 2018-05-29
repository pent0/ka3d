#ifndef _MATH_MATRIX3X4_H
#define _MATH_MATRIX3X4_H


#include <math/float4.h>
#include <math/float3x3.h>
#include <lang/assert.h>


BEGIN_NAMESPACE(math) 


class quaternion;
class float4x4;


/**
 * 3x4 column-major matrix of scalar type float.
 * NOTE: The class behaves like C basic types what it comes to initialization
 * so default constructor leaves the components to unspecified value.
 * 
 * @ingroup math
 */
class float3x4
{
public:
	enum
	{ 
		/** Number of rows in a matrix.*/
		ROWS	= 3, 
		/** Number of columns in a matrix.*/
		COLUMNS	= 4, 
	};

#ifdef _DEBUG
	/** Constructs undefined matrix. */
	float3x4()																		{float v = getNaN(); for ( int i = 0 ; i < ROWS ; ++i ) for ( int j = 0 ; j < COLUMNS ; ++j ) m[i][j] = v;}
#else
	float3x4()																		{}
#endif
		
	/** Identity matrix multiplied with specified scalar. */
	explicit float3x4( float diagonal );

	/** Affine tm from generic 4x4. */
	explicit float3x4( const float4x4& o );

	/** Transformation matrix from rotation and translation. */
	float3x4( const float3x3& rot, const float3& translation );

	/** Transformation matrix from rotation, translation and scale. */
	float3x4( const float3x3& rot, const float3& translation, const float3& scale );

	/** Transformation matrix from rotation and translation. */
	float3x4( const quaternion& q, const float3& translation );

	/** Transformation matrix from rotation, translation and scale. */
	float3x4( const quaternion& q, const float3& translation, const float3& scale );

	/** Component-wise addition of matrices.*/
	float3x4&		operator+=( const float3x4& other )							{for ( int i = 0 ; i < ROWS ; ++i ) for ( int j = 0 ; j < COLUMNS ; ++j ) m[i][j] += other.m[i][j]; return *this;}
	
	/** Component-wise subtraction of matrices.*/
	float3x4&		operator-=( const float3x4& other )							{for ( int i = 0 ; i < ROWS ; ++i ) for ( int j = 0 ; j < COLUMNS ; ++j ) m[i][j] -= other.m[i][j]; return *this;}

	/** Sets ith column of the matrix. */
	void			setColumn( int i, const float3& c )							{assert(i>=0&&i<COLUMNS); m[0][i]=c[0]; m[1][i]=c[1]; m[2][i]=c[2];}

	/** Sets ith row of the matrix. */
	void			setRow( int i, const float4& c )								{assert(i>=0&&i<ROWS); m[i][0]=c[0]; m[i][1]=c[1]; m[i][2]=c[2]; m[i][3]=c[3];}

	/** Sets top-left 3x3 submatrix. */
	void			setRotation( const float3x3& rot );

	/** 
	 * Sets top-left 3x3 submatrix from quaternion rotation. 
	 */
	void			setRotation( const quaternion& q );

	/** 
	 * Sets top-left 3x3 submatrix as angle/axis rotation. 
	 * @param axis Axis of rotation.
	 * @param angle Angle in radians.
	 */
	void			setRotation( const NS(math,float3)& axis, float angle );
		
	/** Sets right column (excluding the last row) of the matrix. */
	void			setTranslation( const float3& t );

	/** 
	 * Sets this matrix as inverse transform of float3x4(rot,t). 
	 * Assumes that the matrix is orthonormal.
	 */
	void			setInverseOrthonormalTransform( const float3x3& rot, const float3& t );

	/** Access element at (row,column) in the matrix.*/
	float&			operator()( int row, int column )								{assert(row<ROWS&&column<COLUMNS); return m[row][column];}

	/**	Returns this matrix multiplied with a scalar.*/
	float3x4		operator*( float s ) const										{float3x4 a; for ( int i = 0 ; i < ROWS ; ++i ) for ( int j = 0 ; j < COLUMNS ; ++j ) a.m[i][j] = m[i][j] * s; return a;}

	/** Component-wise equality.*/
	bool			operator==( const float3x4& other ) const						{for ( int i = 0 ; i < ROWS ; ++i ) for ( int j = 0 ; j < COLUMNS ; ++j ) if ( m[i][j] != other.m[i][j] ) return false; return true;}
	
	/** Component-wise inequality.*/
	bool			operator!=( const float3x4& other ) const						{for ( int i = 0 ; i < ROWS ; ++i ) for ( int j = 0 ; j < COLUMNS ; ++j ) if ( m[i][j] != other.m[i][j] ) return true; return false;}

	/** Matrix multiplication.*/
	float3x4		operator*( const float3x4& other ) const;

	/** Returns component-wise addition of matrices.*/
	float3x4		operator+( const float3x4& other ) const						{float3x4 a; for ( int i = 0 ; i < ROWS ; ++i ) for ( int j = 0 ; j < COLUMNS ; ++j ) a.m[i][j] = m[i][j] + other.m[i][j]; return a;}
	
	/** Returns component-wise subtraction of matrices.*/
	float3x4		operator-( const float3x4& other ) const						{float3x4 a; for ( int i = 0 ; i < ROWS ; ++i ) for ( int j = 0 ; j < COLUMNS ; ++j ) a.m[i][j] = m[i][j] - other.m[i][j]; return a;}

	/** Returns component-wise negation.*/
	float3x4		operator-() const												{float3x4 a; for ( int i = 0 ; i < ROWS ; ++i ) for ( int j = 0 ; j < COLUMNS ; ++j ) a.m[i][j] = -m[i][j]; return a;}

	/* Matrix multiplication with 4-vector as 1-column matrix.*/
	float4			operator*( const float4& v ) const;

	/** Access element at (row,column) in the matrix.*/
	const float&	operator()( int row, int column ) const							{assert(row<ROWS&&column<COLUMNS); return m[row][column];}

	/** Returns element at (row,column) in the matrix.*/
	float			get( int row, int column ) const								{return m[row][column];}

	/** Returns ith column of the matrix. */
	float3			getColumn( int i ) const										{return float3( m[0][i], m[1][i], m[2][i] );}

	/** Returns ith row of the matrix. */
	const float4&	getRow( int i ) const											{return *reinterpret_cast<const float4*>(m[i]);}

	/** Returns top-left 3x3 submatrix. */
	float3x3		rotation() const;

	/** Returns top-left 3x3 submatrix. */
	void			getRotation( float3x3* rot ) const;

	/** Returns top-right 3x1 submatrix. */
	float3			translation() const												{return float3( m[0][3], m[1][3], m[2][3] );}

	/** Returns top-right 3x1 submatrix. */
	void			getTranslation( NS(math,float3)* t ) const						{t->x=m[0][3]; t->y=m[1][3]; t->z=m[2][3];}

	/** Returns lengths of the three first column vectors. */
	float3			scale() const;

	/** Multiplies the matrix and the 3-vector as (x,y,z,1). */
	float3			transform( const float3& v ) const;

	/** Multiplies the matrix and the 3-vector as (x,y,z,1). */
	float4			transform( const float4& v ) const;

	/** Multiplies the matrix and the 3-vector as (x,y,z,1). */
	void			transform( const float3& v, float3* v1 ) const;

	/** Multiplies the matrix and the 4-vector as (x,y,z,w). */
	void			transform( const float4& v, float4* v1 ) const;

	/** Multiplies the matrix and the 3-vector as (x,y,z,0). */
	float3			rotate( const float3& v ) const;

	/** Multiplies the matrix and the 3-vector as (x,y,z,0). */
	void			rotate( const float3& v, float3* v1 ) const;

	/** Returns true if all components are in finite range. */
	bool			finite() const;

	/** Returns determinant of 3x3 top left sub-matrix. */
	float			determinant3() const;

	/** Returns inverse transform. */
	float3x4		inverse() const;

private:
	float			m[ROWS][COLUMNS] MATH_ALIGN_128;
};


END_NAMESPACE() // math


#endif // _MATH_MATRIX3X4_H

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
