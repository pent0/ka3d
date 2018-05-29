#ifndef _HGR_PIPE_H
#define _HGR_PIPE_H


#include <gr/Context.h>
#include <gr/Primitive.h>
#include <lang/Object.h>


BEGIN_NAMESPACE(gr) 
	class Texture;END_NAMESPACE()


BEGIN_NAMESPACE(hgr) 


class Scene;
class Camera;
class PipeSetup;


/**
 * Base class for different scene rendering techniques.
 *
 * Different rendering pipes (see Pipe and derived class GlowPipe and DefaultPipe) 
 * provide more abstract usage level for different rendering techniques.
 * For example application can render normal scene and then glow on 
 * top of that with one line call. Even tho convenient and time saving, 
 * rendering pipes have still been kept as very simple so that 
 * user can easily define own pipes if needed, or even go to more low level
 * rendering support and skipping pipes altogether. Only sample application
 * to use rendering pipes is currently samples/render_to_texture_test --
 * all others use Camera and Context classes directly. See DefaultPipe
 * for most simple rendering pipe implementation.
 *
 * @ingroup hgr
 */
class Pipe :
	public NS(lang,Object)
{
public:
	/**
	 * Allocates resources needed by the pipeline.
	 */
	explicit Pipe( PipeSetup* setup );

	~Pipe();

	/**
	 * Renders the scene using this pipe to the rendering context.
	 * @param target Rendering target or 0 if back buffer should be used.
	 * @param context Rendering target context.
	 * @param scene Scene to be rendered.
	 * @param camera Scene camera to use in rendering.
	 */
	virtual void	render( NS(gr,Texture)* target, NS(gr,Context)* context, Scene* scene, Camera* camera ) = 0;

protected:
	/**
	 * Returns pipeline setup object which can be used to share resources
	 * and setup costs between different pipes.
	 */
	PipeSetup*				setup() const;

	/**
	 * Creates overlay primitive which covers full viewport.
	 * @param context Rendering context.
	 * @param width Texture width to be rendered in pixels.
	 * @param height Texture height to be rendered in pixels.
	 * @exception GraphicsException
	 */
	static P(NS(gr,Primitive))	createOverlayPrimitive( NS(gr,Context)* context, int width, int height );

	/**
	 * Renders viewport size overlay screen using specified shader and 
	 * optional on-fly set texture.
	 */
	static void				renderOverlay( NS(gr,Primitive)* overlay, NS(gr,Shader)* shader, NS(gr,Texture)* basetex=0 );

	/**
	 * Resizes overlay to specified size.
	 * @param context Rendering context.
	 * @param width Texture width to be rendered in pixels.
	 * @param height Texture height to be rendered in pixels.
	 */
	static void				resizeOverlay( NS(gr,Primitive)* overlay, int width, int height );

private:
	P(PipeSetup)			m_setup;

	Pipe();
	Pipe( const Pipe& );
	Pipe& operator=( const Pipe& );
};


END_NAMESPACE() // hgr


#endif // _HGR_PIPE_H

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
