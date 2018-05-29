#ifndef _HGR_DEFAULTRESOURCEMANAGER_H
#define _HGR_DEFAULTRESOURCEMANAGER_H


#include <gr/Texture.h>
#include <gr/CubeTexture.h>
#ifndef HGR_NOPARTICLES
#include <hgr/ParticleSystem.h>
#endif
#include <hgr/ResourceManager.h>
#include <lang/Hashtable.h>


BEGIN_NAMESPACE(gr) 
	class Context;
	class Texture;END_NAMESPACE()

BEGIN_NAMESPACE(lang) 
	class String;END_NAMESPACE()


BEGIN_NAMESPACE(hgr) 


class ParticleSystem;


/**
 * Default rendering resource manager.
 *
 * Manages resources in following way:
 * <ul>
 * <li>Shares texture by matching pathname (i.e. C:/mydata/tex.bmp != C:/tex.bmp)
 * <li>Shares particles by matching basename (i.e. C:/flare.prs == C:/data/flare.prs)
 * <li>Shaders are not shared (i.e. C:/my.fx != C:/my.fx)
 * </ul>
 *
 * @ingroup hgr
 */
class DefaultResourceManager :
	public ResourceManager
{
public:
	/**
	 * Creates resource manager using specified rendering context.
	 */
	explicit DefaultResourceManager( NS(gr,Context)* context );

	~DefaultResourceManager();

	/**
	 * Gets shader by name.
	 * @exception Exception
	 */
	NS(gr,Shader)*		getShader( const NS(lang,String)& name, int flags );

	/** 
	 * Gets texture by filename. Note that basenames need to be unique!
	 * @exception Exception
	 */
	NS(gr,Texture)*		getTexture( const NS(lang,String)& filename );

	/** 
	 * Gets cube texture by filename. 
	 * @exception Exception
	 */
	NS(gr,CubeTexture)*	getCubeTexture( const NS(lang,String)& filename );

	/** 
	 * Gets particle system by filename. 
	 * @exception Exception
	 */
	ParticleSystem*		getParticleSystem( const NS(lang,String)& filename,
							const NS(lang,String)& texturepath,
							const NS(lang,String)& shaderpath );

	/**
	 * Finds texture resources below specific path.
	 * @param pathfilter Path to find resource from, e.g. D:/data/*
	 */
	void				findTextureResources( const NS(lang,String)& pathfilter );

	/**
	 * Replaces texture file extension with specified string.
	 * Can be used for example to load platform-specific texture files 
	 * when scenes have been exported with generic files.
	 * @param newsuffix New file name extension for textures, dot included.
	 */
	void				replaceTextureExtension( const NS(lang,String)& newsuffix );

	/**
	 * Releases textures which have no (external) references left.
	 * @return Number of surface memory bytes released.
	 */
	int					releaseUnusedTextures();

	/**
	 * Returns active resource manager.
	 */
	static ResourceManager* get( NS(gr,Context)* context );

	/**
	 * Sets active resource manager.
	 */
	static void				set( ResourceManager* res );

private:
	class TextureResource
	{
	public:
		P(NS(gr,Texture))	texture;
		NS(lang,String)		filename;
	};

	NS(lang,String)		m_textureExtension;
	P(NS(gr,Context))																				m_context;
	NS(lang,Hashtable)< NS(lang,String),TextureResource,NS(lang,Hash)<NS(lang,String)> >			m_textures;
	NS(lang,Hashtable)< NS(lang,String),P(NS(gr,CubeTexture)),NS(lang,Hash)<NS(lang,String)> >		m_cubeTextures;
#ifndef HGR_NOPARTICLES
	NS(lang,Hashtable)< NS(lang,String),P(NS(hgr,ParticleSystem)),NS(lang,Hash)<NS(lang,String)> >	m_particles;
#endif

	NS(lang,String)		getTextureSystemFilename( const NS(lang,String)& filename );


	DefaultResourceManager( const DefaultResourceManager& );
	DefaultResourceManager& operator=( const DefaultResourceManager& );
};


END_NAMESPACE() // hgr


#endif // _HGR_DEFAULTRESOURCEMANAGER_H

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
