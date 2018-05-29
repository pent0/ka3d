#ifndef _GR_PALETTE_H 
#define _GR_PALETTE_H


#include <gr/ContextObject.h>


BEGIN_NAMESPACE(gr) 

	
class SurfaceFormat;
	
	
/**
 * Base class for platform dependent palettes.
 * @ingroup gr
 */
class Palette :
	public ContextObject
{
public:
	Palette();

	/**
	 * Sets palette entries.
	 * Palette must be locked for writing before this method can be called.
	 * @param index First palette entry to set.
	 * @param data Pointer to palette source data.
	 * @param fmt Source pixel format.
	 * @param count Number of entries to set.
	 */
	virtual void	setEntries( int index, const void* data, const SurfaceFormat& fmt, int count ) = 0;

	/**
	 * Gets palette entries.
	 * Palette must be locked for reading before this method can be called.
	 * @param index First palette entry to get.
	 * @param data Pointer to palette data.
	 * @param fmt Palette pixel format.
	 * @param count Number of entries to get.
	 */
	virtual void	getEntries( int index, void* data, const SurfaceFormat& fmt, int count ) = 0;

	/**
	 * Returns number of entries in the palette.
	 */
	virtual int		entries() const = 0;
};


END_NAMESPACE() // gr


#endif // _GR_PALETTE_H

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
