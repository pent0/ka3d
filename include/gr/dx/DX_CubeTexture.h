#ifndef _GR_DX_CUBETEXTURE_H
#define _GR_DX_CUBETEXTURE_H


#include "DX_common.h"
#include "DX_ContextItem.h"
#include <gr/CubeTexture.h>
#include <gr/SurfaceFormat.h>
#include <lang/String.h>
#include <stdint.h>


BEGIN_NAMESPACE(gr) 


class DX_Context;


/**
 * DirectX cube texture.
 */
class DX_CubeTexture :
	public CubeTexture,
	public DX_ContextItem
{
public:
	/**
	 * Initializes the texture from file.
	 */
	explicit DX_CubeTexture( DX_Context* context, const NS(lang,String)& filename );

	///
	~DX_CubeTexture();

	/**
	 * Called before reseting device.
	 */
	void	deviceLost();

	/**
	 * Called after reseting device.
	 */
	void	deviceReset();

	/**
	 * Returns texture top level surface width in pixels.
	 */
	int		width() const;

	/**
	 * Returns texture top level surface height in pixels.
	 */
	int		height() const;

	/**
	 * Returns surface pixel storage format of this texture.
	 */
	SurfaceFormat 	format() const;

	/**
	 * Returns object string description.
	 */
	NS(lang,String)	toString() const;

	/**
	 * Returns IDirect3DCubeTexture
	 */
	IDirect3DCubeTexture9*	texture() const			{return m_tex;}

private:
	IDirect3DCubeTexture9*	m_tex;
	NS(lang,String)			m_filename;
	D3DLOCKED_RECT			m_lockedRect;
	uint16_t				m_width;
	uint16_t				m_height;
	uint8_t					m_usageflags;
	SurfaceFormat			m_format;
	LockType				m_locked;

	void	create( const NS(lang,String)& filename );
	void	validateDesc( const NS(lang,String)& filename, int usageflags );

	DX_CubeTexture( const DX_CubeTexture& );
	DX_CubeTexture& operator=( const DX_CubeTexture& );
};

	
END_NAMESPACE() // gr


#endif // _GR_DX_CUBETEXTURE_H

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
