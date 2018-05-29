#include <img/ShapeUtil.h>
#include <stdlib.h>
#include <config.h>


BEGIN_NAMESPACE(img) 


void ShapeUtil::drawLine( int* img, int w, int h, int vx0, int vy0, int vx1, int vy1, int color )
{
	int ycount = abs(vy1-vy0);
	int xcount = abs(vx1-vx0);
	int count = ycount > xcount ? ycount : xcount;
	if ( count < 1 )
		return;

	int dx = ((vx1-vx0) << 10)/count;
	int dy = ((vy1-vy0) << 10)/count;
	int ax = vx0 << 10;
	int ay = vy0 << 10;

	for ( int i = 0 ; i < count ; ++i )
	{
		int x = ax >> 10;
		int y = ay >> 10;
		if ( unsigned(x) < unsigned(w) && unsigned(y) < unsigned(h) )
			img[w*y+x] = color;
		ax += dx;
		ay += dy;
	}
}

void ShapeUtil::drawPolygon( int* img, int w, int h, 
	const int* vx, const int* vy, int vcount, int color )
{
	assert( vcount >= 3 );
	assert( h <= 1024 );

	int edge1[1024];
	int edge2[1024];
	int miny = h;
	int maxy = -h;

	for ( int i = 0 ; i < vcount ; ++i )
	{
		int y = vy[i];
		if ( y < 0 )
			y = 0;
		else if ( y > h )
			y = h;

		if ( y < miny )
			miny = y;
		if ( y > maxy )
			maxy = y;
	}
	
	for ( int i = miny ; i < maxy ; ++i )
	{
		edge1[i] = w;
		edge2[i] = -w;
	}

	int k = vcount-1;
	for ( int i = 0 ; i < vcount ; k = i++ )
		calcEdges( edge1, edge2, h, vx[k], vy[k], vx[i], vy[i] );

	for ( int i = miny ; i < maxy ; ++i )
	{
		int x0 = edge1[i];
		int x1 = edge2[i];
		int p = i*w + x0;

		for ( int x = x0 ; x < x1 ; ++x )
		{
			if ( x >= 0 && x < w )
				img[p] += color;
			++p;
		}
	}
}

void ShapeUtil::calcEdges( int* edge1, int* edge2, int h, int vx0, int vy0, int vx1, int vy1 )
{
	int count = abs(vy1-vy0);
	if ( count < 1 )
		return;

	int dx = ((vx1-vx0) << 10)/count;
	int dy = ((vy1-vy0) << 10)/count;
	int ax = vx0 << 10;
	int ay = vy0 << 10;

	for ( int i = 0 ; i < count ; ++i )
	{
		int x = ax >> 10;
		int y = ay >> 10;
		if ( unsigned(y) < unsigned(h) )
		{
			if ( x > edge2[y] )
				edge2[y] = x;
			if ( x < edge1[y] )
				edge1[y] = x;
		}
		ax += dx;
		ay += dy;
	}
}


END_NAMESPACE() // img

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
