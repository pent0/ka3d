#ifndef _FRAMEWORK_OSINTERFACE_H
#define _FRAMEWORK_OSINTERFACE_H


#include <io/PathName.h>
#include <lang/Array.h>
#include <lang/Object.h>
#include <lang/String.h>


BEGIN_NAMESPACE(io) 
	class DataInputStream;
	class DataOutputStream;END_NAMESPACE()

BEGIN_NAMESPACE(gr) 
	class Context;END_NAMESPACE()


BEGIN_NAMESPACE(framework) 


/**
 * Interface to operating system specific functionality.
 */
class OSInterface
{
public:
	/**
	 * Loads all shaders from specific directory.
	 * After this the application can refer to the shader with
	 * its basename. For example after loading "high-end-shaders/bumpspecular.fx"
	 * application can simply request "bumpspecular" shader and
	 * get an instance of the previously loaded shader.
	 */
	virtual void			loadShaders( const NS(lang,String)& path, bool recursesubdirs ) = 0;
	
	/**
	 * Returns path to default data directory.
	 */
	virtual NS(lang,String)	getDefaultDataPath() = 0;
};


END_NAMESPACE() // framework


#endif // _FRAMEWORK_OSINTERFACE_H
