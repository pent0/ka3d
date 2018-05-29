#ifndef _GR_RECT_H
#define _GR_RECT_H


#include <lang/Array.h>


BEGIN_NAMESPACE(gr) 


/** 
 * Rectangle with integer 2D coordinates. 
 * @ingroup gr
 */
class Rect
{
public:
	/** 
	 * Creates empty rectangle to origin. 
	 */
	Rect();

	/** 
	 * Creates rectangle to origin with specified size.
	 * @param width Width of the rectangle.
	 * @param height Height of the rectangle.
	 */
	Rect( int width, int height );

	/** 
	 * Creates rectangle by top-left coordinates and size.
	 * @param x0 Top-left X-coordinate (inclusive).
	 * @param y0 Top-left Y-coordinate (inclusive).
	 * @param x1 Bottom-right X-coordinate (exclusive).
	 * @param y1 Bottom-right Y-coordinate (exclusive).
	 */
	Rect( int x0, int y0, int x1, int y1 );

	/**
	 * Sets left border of the rectangle (inclusive).
	 */
	void	setLeft( int x );

	/**
	 * Sets right border of the rectangle (exclusive).
	 */
	void	setRight( int x );

	/**
	 * Sets top border of the rectangle (inclusive).
	 */
	void	setTop( int y );

	/**
	 * Sets bottom border of the rectangle (exclusive).
	 */
	void	setBottom( int y );

	/** 
	 * Intersection of rectangles. 
	 */
	Rect&	operator&=( const Rect& other );

	/** 
	 * Union of rectangles. 
	 */
	Rect&	operator|=( const Rect& other );

	/** 
	 * Returns intersection of rectangles. 
	 */
	Rect	operator&( const Rect& other ) const;

	/** 
	 * Returns union of rectangles. 
	 */
	Rect	operator|( const Rect& other ) const;

	/**
	 * Returns left edge coordinate of the rectangle.
	 */
	int		left() const		{return m_x0;}

	/**
	 * Returns right edge coordinate of the rectangle.
	 */
	int		right() const		{return m_x1;}

	/**
	 * Returns top edge coordinate of the rectangle.
	 */
	int		top() const			{return m_y0;}

	/**
	 * Returns bottom edge coordinate of the rectangle.
	 */
	int		bottom() const		{return m_y1;}

	/**
	 * Returns width of the rectangle.
	 */
	int		width() const		{return m_x1-m_x0;}

	/**
	 * Returns height of the rectangle.
	 */
	int		height() const		{return m_y1-m_y0;}

	/**
	 * Returns area (width*height) of the rectangle.
	 */
	int		area() const		{return width()*height();}

	/**
	 * Return the difference of this and other rect.
	 * Works ONLY in situations where result is rectancle.
	 */
	Rect	difference( const Rect& other ) const;

private:
	int	m_x0;
	int	m_y0;
	int	m_x1;
	int	m_y1;
};


END_NAMESPACE() // gr


#endif // _GR_RECT_H

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
