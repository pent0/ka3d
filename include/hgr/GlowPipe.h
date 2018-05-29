#ifndef _HGR_GLOWPIPE_H
#define _HGR_GLOWPIPE_H


#include <hgr/Pipe.h>


BEGIN_NAMESPACE(hgr) 


class Scene;
class Camera;


/**
 * Post-processing glow effect.
 * @ingroup hgr
 */
class GlowPipe :
	public Pipe
{
public:
	/**
	 * Inits resources needed by glow effect.
	 * @param setup Rendering setup which is shared between all pipes.
	 * @exception GraphicsException
	 */
	explicit GlowPipe( PipeSetup* setup );

	/**
	 * Renders (only) glow to specified rendering target.
	 * @param target Rendering target or 0 if back buffer should be used.
	 * @param context Rendering target context.
	 * @param scene Scene to be rendered.
	 * @param camera Scene camera to use in rendering.
	 */
	void	render( NS(gr,Texture)* target, NS(gr,Context)* context, Scene* scene, Camera* camera );

private:
	P(NS(gr,Texture))				m_rtt;
	P(NS(gr,Texture))				m_rttBlurH;
	P(NS(gr,Texture))				m_rttBlur;
	P(NS(gr,Texture))				m_rttNewBlur;
	P(NS(gr,Texture))				m_rttOldBlur;
	P(NS(gr,Shader))				m_add;
	P(NS(gr,Shader))				m_trail;
	P(NS(gr,Shader))				m_mulA;
	P(NS(gr,Shader))				m_blurH;
	P(NS(gr,Shader))				m_blurV;
	P(NS(gr,Primitive))			m_overlay;
};


END_NAMESPACE() // hgr


#endif // _HGR_GLOWPIPE_H

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
