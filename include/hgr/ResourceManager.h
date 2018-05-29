#ifndef _HGR_RESOURCEMANAGER_H
#define _HGR_RESOURCEMANAGER_H


#include <lang/Object.h>
#include <lang/String.h>


BEGIN_NAMESPACE(gr) 
	class Shader;
	class Context;
	class Texture;
	class CubeTexture;END_NAMESPACE()

BEGIN_NAMESPACE(lang) 
	class String;END_NAMESPACE()


BEGIN_NAMESPACE(hgr) 


class ParticleSystem;


/**
 * Abstract base class for managing and sharing rendering resources.
 *
 * @ingroup hgr
 */
class ResourceManager :
	public NS(lang,Object)
{
public:
	/**
	 * Gets shader by name.
	 * @param name (Base) name of the shader.
	 * @param flags Shader compilation flags. See NS(Shader,Flags).
	 * @exception Exception
	 */
	virtual NS(gr,Shader)*			getShader( const NS(lang,String)& name, int flags ) = 0;

	/** 
	 * Gets texture by filename. 
	 * @param name Name of the texture.
	 * @exception Exception
	 */
	virtual NS(gr,Texture)*		getTexture( const NS(lang,String)& filename ) = 0;

	/** 
	 * Gets cube texture by filename. 
	 * @param name Name of the texture.
	 * @exception Exception
	 */
	virtual NS(gr,CubeTexture)*	getCubeTexture( const NS(lang,String)& filename ) = 0;

#ifndef HGR_NOPARTICLES
	/** 
	 * Gets particle system by filename. 
	 * @param name Name of the particle system.
	 * @exception Exception
	 */
	virtual ParticleSystem*		getParticleSystem( const NS(lang,String)& filename,
									const NS(lang,String)& texturepath="",
									const NS(lang,String)& shaderpath="" ) = 0;
#endif
};


END_NAMESPACE() // hgr


#endif // _HGR_RESOURCEMANAGER_H

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
