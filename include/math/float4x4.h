#ifndef _MATH_MATRIX4X4_H
#define _MATH_MATRIX4X4_H


#include <lang/pp.h>
#include <math/float4.h>
#include <math/float3x3.h>


BEGIN_NAMESPACE(math) 


class float3x4;


/**
 * 4x4 column-major matrix of scalar type float.
 *
 * NOTE: The class behaves like C basic types what it comes to initialization
 * so default constructor leaves the components to unspecified value.
 * Alerting user is the reason also for that the name starts with lower-case letter.
 * 
 * @ingroup math
 */
class float4x4
{
public:
	enum
	{ 
		/** Number of rows in a matrix.*/
		ROWS	= 4, 
		/** Number of columns in a matrix.*/
		COLUMNS	= 4, 
	};

#ifdef _DEBUG
	/** Constructs undefined matrix. */
	float4x4()																		{float v = getNaN(); for ( int i = 0 ; i < ROWS ; ++i ) for ( int j = 0 ; j < COLUMNS ; ++j ) m[i][j] = v;}
#else
	float4x4()																		{}
#endif
		
	/** Matrix from 4 column vectors. */
	float4x4( const float4& x0, const float4& y0, 
		const float4& z0, const float4& w0 )										{setColumn(0,x0); setColumn(1,y0); setColumn(2,z0); setColumn(3,w0);}

	/** Matrix from 3x4 matrix with last row of this matrix set to (0,0,0,1). */
	float4x4( const float3x4& other );

	/** Identity matrix multiplied with specified scalar. */
	explicit float4x4( float diagonal );

	/** Transformation matrix from rotation and translation. */
	float4x4( const float3x3& rotation, const float3& translation );
		
	/** Component-wise addition of matrices.*/
	float4x4&		operator+=( const float4x4& other )							{for ( int i = 0 ; i < ROWS ; ++i ) for ( int j = 0 ; j < COLUMNS ; ++j ) m[i][j] += other.m[i][j]; return *this;}
	
	/** Component-wise subtraction of matrices.*/
	float4x4&		operator-=( const float4x4& other )							{for ( int i = 0 ; i < ROWS ; ++i ) for ( int j = 0 ; j < COLUMNS ; ++j ) m[i][j] -= other.m[i][j]; return *this;}
	
	/** Matrix multiplication.*/
	float4x4&		operator*=( const float4x4& other )							{return *this = *this * other;}
	
	/**	Multiplies all components of this matrix with a scalar.*/
	float4x4&		operator*=( float s )											{for ( int i = 0 ; i < ROWS ; ++i ) for ( int j = 0 ; j < COLUMNS ; ++j ) m[i][j] *= s; return *this;}

	/** Access element at (row,column) in the matrix.*/
	float&			operator()( int row, int column )								{return m[row][column];}
	
	/** Sets element at (row,column) in the matrix.*/
	void			set( int row, int column, float value )							{m[row][column]=value;}

	/** Sets ith column of the matrix. */
	void			setColumn( int i, const float4& c )							{m[0][i]=c[0]; m[1][i]=c[1]; m[2][i]=c[2]; m[3][i]=c[3];}

	/** Sets ith row of the matrix. */
	void			setRow( int i, const float4& c )								{m[i][0]=c[0]; m[i][1]=c[1]; m[i][2]=c[2]; m[i][3]=c[3];}

	/** Sets top-left 3x3 submatrix. */
	void			setRotation( const float3x3& rot );

	/** 
	 * Sets top-left 3x3 submatrix as angle/axis rotation. 
	 * @param axis Axis of rotation.
	 * @param angle Angle in radians.
	 */
	void			setRotation( const NS(math,float3)& axis, float angle );
		
	/** Sets right column (excluding the last row) of the matrix. */
	void			setTranslation( const float3& t );

	/** 
	 * Sets this matrix as inverse transform of float4x4(rot,t). 
	 * Assumes that the matrix is orthonormal.
	 */
	void			setInverseOrthonormalTransform( const float3x3& rot, const float3& t );

	/** 
	 * Left handed perspective projection matrix.
	 *
	 * @param fovHorz Horizontal field of view in radians.
	 * @param front Front plane distance.
	 * @param back Back plane distance.
	 * @param aspect Viewport width divided by height.
	 */
	void			setPerspectiveProjection( float fovHorz, float front, float back, float aspect );

	/** 
	 * Projection matrix to screen space transformation matrix.
	 *
	 * @param screenz Screen distance in Z-buffer coordinates.
	 * @param scalex X-axis relative scale [0,1].
	 * @param scaley Y-axis relative scale [0,1].
	 * @param zmin Z-buffer value on near plane.
	 * @param zmax Z-buffer value on far plane.
	 * @param cx X-axis origin in screen space.
	 * @param cy Y-axis origin in screen space.
	 */
	void			setScreenTransform( float screenz, float scalex, float scaley, 
						float zmin, float zmax, float cx, float cy );

	/** 
	 * Matrix projects points to the plane along 
	 * the line from the projection origin to the point to be projected.
	 *
	 * @param l Projection origin.
	 * @param n Normal of the projection plane.
	 * @param p Point on the projection plane.
	 */
	void			setPointPlaneProjection( const float3& l,
						const float3& n, const float3& p );

	/** 
	 * Matrix projects points to the plane along 
	 * lines parallel to specified direction.
	 *
	 * @param l Projection direction.
	 * @param n Normal of the projection plane.
	 * @param p Point on the projection plane.
	 */
	void			setDirectPlaneProjection( const float3& l,
						const float3& n, const float3& p );

	/** Returns component-wise addition of matrices.*/
	float4x4		operator+( const float4x4& other ) const						{float4x4 a; for ( int i = 0 ; i < ROWS ; ++i ) for ( int j = 0 ; j < COLUMNS ; ++j ) a.m[i][j] = m[i][j] + other.m[i][j]; return a;}
	
	/** Returns component-wise subtraction of matrices.*/
	float4x4		operator-( const float4x4& other ) const						{float4x4 a; for ( int i = 0 ; i < ROWS ; ++i ) for ( int j = 0 ; j < COLUMNS ; ++j ) a.m[i][j] = m[i][j] - other.m[i][j]; return a;}

	/** Returns component-wise negation.*/
	float4x4		operator-() const												{float4x4 a; for ( int i = 0 ; i < ROWS ; ++i ) for ( int j = 0 ; j < COLUMNS ; ++j ) a.m[i][j] = -m[i][j]; return a;}
	
	/**	Returns this matrix multiplied with a scalar.*/
	float4x4		operator*( float s ) const										{float4x4 a; for ( int i = 0 ; i < ROWS ; ++i ) for ( int j = 0 ; j < COLUMNS ; ++j ) a.m[i][j] = m[i][j] * s; return a;}
	
	/** Matrix multiplication.*/
	float4x4		operator*( const float4x4& other ) const;

	/** Matrix multiplication.*/
	float4x4		operator*( const float3x4& other ) const;

	/* Matrix multiplication with 4-vector as 1-column matrix.*/
	float4			operator*( const float4& v ) const;

	/** Component-wise equality.*/
	bool			operator==( const float4x4& other ) const						{for ( int i = 0 ; i < ROWS ; ++i ) for ( int j = 0 ; j < COLUMNS ; ++j ) if ( m[i][j] != other.m[i][j] ) return false; return true;}
	
	/** Component-wise inequality.*/
	bool			operator!=( const float4x4& other ) const						{for ( int i = 0 ; i < ROWS ; ++i ) for ( int j = 0 ; j < COLUMNS ; ++j ) if ( m[i][j] != other.m[i][j] ) return true; return false;}

	/** Access element at (row,column) in the matrix.*/
	const float&	operator()( int row, int column ) const							{return m[row][column];}

	/** Returns element at (row,column) in the matrix.*/
	float			get( int row, int column ) const								{return m[row][column];}

	/** Returns ith column of the matrix. */
	float4			getColumn( int i ) const										{float4 d; d[0]=m[0][i]; d[1]=m[1][i]; d[2]=m[2][i]; d[3]=m[3][i]; return d;}

	/** Returns ith row of the matrix. */
	const float4&	getRow( int i ) const											{return *reinterpret_cast<const float4*>(m[i]);} //{float4 d; d[0]=m[i][0]; d[1]=m[i][1]; d[2]=m[i][2]; d[3]=m[i][3]; return d;}

	/** Returns top-left 3x3 submatrix. */
	float3x3		rotation() const;

	/** Returns top-right 3x1 submatrix. */
	float3			translation() const;

	/** Returns transposed matrix. */
	float4x4		transpose() const;

	/** Returns determinant of the matrix. */
	float			determinant() const;

	/** Returns inverse of the matrix. */
	float4x4		inverse() const;

	/** Multiplies the matrix and the 3-vector as (x,y,z,1). */
	float3			transform( const float3& v ) const;

	/** Multiplies the matrix and the 4-vector as (x,y,z,w). */
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

private:
	float			m[ROWS][COLUMNS] MATH_ALIGN_128;
};


END_NAMESPACE() // math


#endif // _MATH_MATRIX4X4_H

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
