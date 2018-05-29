#include <hgr/DefaultResourceManager.h>
#include <io/PathName.h>
#include <io/FindFile.h>
#include <io/IOException.h>
#include <img/ImageReader.h>
#include <gr/Context.h>
#include <hgr/Globals.h>
#include <lang/String.h>
#include <config.h>
#include <lang/pp.h>
#include <lang/Debug.h>
#include <time.h>


USING_NAMESPACE(gr)
USING_NAMESPACE(io)
USING_NAMESPACE(img)
USING_NAMESPACE(lang)


BEGIN_NAMESPACE(hgr) 


DefaultResourceManager::DefaultResourceManager( Context* context ) :
	m_context( context )
{
}

DefaultResourceManager::~DefaultResourceManager()
{
	m_textures.clear();
}

Shader* DefaultResourceManager::getShader( const String& name, int flags )
{
	return m_context->createShader( name, flags );
}

void DefaultResourceManager::replaceTextureExtension( const String& newsuffix )
{
	m_textureExtension = newsuffix;
}

String DefaultResourceManager::getTextureSystemFilename( const String& filename )
{
	if ( m_textureExtension != "" && filename.length() > 4 )
		return filename.substring(0,filename.length()-4) + m_textureExtension;
	else
		return filename;
}

Texture* DefaultResourceManager::getTexture( const String& originalfilename )
{
#ifdef PLATFORM_SUPPORTS_FINDFILE

	String basename = PathName(originalfilename).basename();
	TextureResource& obj = m_textures[basename];

	if ( obj.texture == 0 )
	{
		if ( obj.filename.length() == 0 )
		{
			obj.filename = originalfilename;
		}

		obj.texture = m_context->createTexture( getTextureSystemFilename(obj.filename) );
	}

	return obj.texture;

#else

	TextureResource& res = m_textures[originalfilename];
	if ( res.texture == 0 )
		res.texture = m_context->createTexture( getTextureSystemFilename(originalfilename) );
	return res.texture;

#endif
}

CubeTexture* DefaultResourceManager::getCubeTexture( const String& filename )
{
	CubeTexture* obj = m_cubeTextures[filename];
	if ( obj == 0 )
		return m_cubeTextures[filename] = m_context->createCubeTexture( filename );
	else
		return obj;
}

#ifndef HGR_NOPARTICLES
ParticleSystem* DefaultResourceManager::getParticleSystem( const String& filename,
	const String& texturepath, const String& shaderpath )
{
	String basename = PathName(filename).basename();

	ParticleSystem* obj = m_particles[basename];
	if ( obj == 0 )
		return m_particles[basename] = new ParticleSystem( m_context, filename, this, texturepath, shaderpath );
	else
		return new ParticleSystem( *obj );
}
#endif

ResourceManager* DefaultResourceManager::get( Context* context )
{
	ResourceManager* res = Globals::get().resourceManager;
	if ( !res )
		Globals::get().resourceManager = res = new DefaultResourceManager( context );
	assert( res != 0 );
	return res;
}

void DefaultResourceManager::set( ResourceManager* res )
{
	Globals::get().resourceManager = res;
}

void DefaultResourceManager::findTextureResources( const String& pathfilter )
{
#ifdef PLATFORM_SUPPORTS_FINDFILE
	
	for ( FindFile ff(pathfilter) ; ff.more() ; ff.next() )
	{
		const FindFile::Data& data = ff.data();

		if ( ImageReader::guessFileFormat(data.path.toString()) != ImageReader::FILEFORMAT_UNKNOWN )
		{
			String basename = data.path.basename();
			TextureResource& res = m_textures[basename];

			if ( res.filename.length() > 0 )
			{
				Debug::printf( "Warning: Ambiguous texture files? \"%s\" (cached) and \"%s\" (ignored)\n", res.filename.c_str(), data.path.toString() );
				//throwError( IOException( Format("Ambiguous texture file names: {0} and {1}", res.filename, data.path.toString()) ) );
				continue;
			}

			res.filename = data.path.toString();
		}

	}

#endif
}

int DefaultResourceManager::releaseUnusedTextures()
{
	int bytesrel = 0;

	for ( HashtableIterator< String, TextureResource > it = m_textures.begin(); it != m_textures.end() ; ++it )
	{
		Texture* tex = it.value().texture;
		if ( tex && tex->references() == 1 )
		{		
			int texsize = tex->width()*tex->height()*(tex->format().bitsPerPixel()/8);
			bytesrel += texsize;
			it.value().texture = 0;
		}
	}

	return bytesrel;
}


END_NAMESPACE() // hgr
