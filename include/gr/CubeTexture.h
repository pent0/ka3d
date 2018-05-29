#ifndef _GR_CUBETEXTURE_H 
#define _GR_CUBETEXTURE_H


#include <gr/Rect.h>
#include <gr/BaseTexture.h>
#include <gr/SurfaceFormat.h>


BEGIN_NAMESPACE(gr) 

	
/**
 * Base class for platform dependent cube textures.
 * @ingroup gr
 */
class CubeTexture :
	public BaseTexture
{
public:
	CubeTexture();

	/**
	 * Returns single texture top level surface width in pixels.
	 */
	virtual int				width() const = 0;

	/**
	 * Returns single texture top level surface height in pixels.
	 */
	virtual int				height() const = 0;

	/**
	 * Returns surface pixel storage format of this texture.
	 */
	virtual SurfaceFormat 	format() const = 0;
};


END_NAMESPACE() // gr


#endif // _GR_CUBETEXTURE_H

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
