#ifndef _IMG_SHAPEUTIL_H
#define _IMG_SHAPEUTIL_H


#include <lang/pp.h>


BEGIN_NAMESPACE(img) 


/**
 * Utilities for drawing debug lines and shapes to A8R8G8B8 images.
 *
 * @ingroup lang
 */
class ShapeUtil
{
public:
	/**
	 * Draws a solid line to the image.
	 * @param img Image buffer
	 * @param w Image width
	 * @param h Image height
	 * @param vx0 Line start x-coordinate
	 * @param vy0 Line start y-coordinate
	 * @param vx1 Line end x-coordinate (excl)
	 * @param vy1 Line end y-coordinate (excl)
	 * @param color Line color
	 */
	static void drawLine( int* img, int w, int h,
		int vx0, int vy0, int vx1, int vy1, int color );

	/**
	 * Draws a solid polygon to the image.
	 * @param img Image buffer
	 * @param w Image width
	 * @param h Image height
	 * @param vx Polygon vertices x-coordinates
	 * @param vy Polygon vertices y-coordinates
	 * @param vcount Polygon vertex count
	 * @param color Polygon color
	 */
	static void drawPolygon( int* img, int w, int h, 
		const int* vx, const int* vy, int vcount, int color );

private:
	/**
	 * Adds edge X-coordinates to two edge buffers.
	 * @param edge1 Left edge buffer.
	 * @param edge2 Right edge buffer.
	 * @param h Height of each edge buffer.
	 * @param vx0 Edge start X.
	 * @param vy0 Edge start Y.
	 * @param vx1 Edge end X.
	 * @param vy1 Edge end Y.
	 */
	static void calcEdges( int* edge1, int* edge2, int h, 
		int vx0, int vy0, int vx1, int vy1 );
};


END_NAMESPACE() // img


#endif // _IMG_SHAPEUTIL_H
