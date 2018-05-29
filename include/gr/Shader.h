#ifndef _GR_SHADER_H
#define _GR_SHADER_H


#include <gr/ContextObject.h>


BEGIN_NAMESPACE(lang) 
	class String;END_NAMESPACE()

BEGIN_NAMESPACE(math) 
	class float4;
	class float4x4;END_NAMESPACE()


BEGIN_NAMESPACE(gr) 

	
class BaseTexture;
	
	
/**
 * Surface shader.
 * @ingroup gr
 */
class Shader :
	public ContextObject
{
public:
	enum 
	{
		/** Maximum number of lights supported by any shader. */
		MAX_LIGHTS = 8 
	};

	/**
	 * Dynamic shader parameter types. Dynamic shader parameters
	 * must be set after NS(Shader,begin)() call.
	 * Static parameters (=parameters specified by strings)
	 * can be set anytime.
	 */
	enum ParamType
	{
		/** Invalid parameter */
		PARAM_NONE,

		/** World space position of camera */
		PARAM_CAMERAP,
		/** Local->world transform */
		PARAM_WORLDTM,
		/** World->camera transform */
		PARAM_VIEWTM,
		/** Camera->world transform */
		PARAM_VIEWTMINV,
		/** World->camera->projection transform */
		PARAM_VIEWPROJTM,
		/** View->projection transform */
		PARAM_PROJTM,
		/** Local->screen transform */
		PARAM_TOTALTM,
		/** Local->world transform array for bones */
		PARAM_BONEWORLDTM,

		/** World position of light 0 */
		PARAM_LIGHTP0,
		/** World position of light 1 */
		PARAM_LIGHTP1,
		/** World position of light 2 */
		PARAM_LIGHTP2,
		/** World position of light 3 */
		PARAM_LIGHTP3,
		/** World position of light 4 */
		PARAM_LIGHTP4,
		/** World position of light 5 */
		PARAM_LIGHTP5,
		/** World position of light 6 */
		PARAM_LIGHTP6,
		/** World position of light 7 */
		PARAM_LIGHTP7,
		/** Color of light 0 */
		PARAM_LIGHTC0,
		/** Color of light 1 */
		PARAM_LIGHTC1,
		/** Color of light 2 */
		PARAM_LIGHTC2,
		/** Color of light 3 */
		PARAM_LIGHTC3,
		/** Color of light 4 */
		PARAM_LIGHTC4,
		/** Color of light 5 */
		PARAM_LIGHTC5,
		/** Color of light 6 */
		PARAM_LIGHTC6,
		/** Color of light 7 */
		PARAM_LIGHTC7,

		/** Number of different dynamic parameters. */
		PARAM_COUNT
	};

	/**
	 * Sorting mode which should be used with this shader.
	 */
	enum SortType
	{
		/** No sort needed. */
		SORT_NONE,
		/** Polygons of this material should be drawn back-to-front order. */
		SORT_BACKTOFRONT,
		/** Polygons of this material should be drawn front-to-back order. */
		SORT_FRONTTOBACK,
		/** Polygons of this material should be drawn inside-out order. */
		SORT_INSIDEOUT,
		/** Polygons of this material should be drawn outside-in order. */
		SORT_OUTSIDEIN,
	};

	/**
	 * Compilation flags for the material.
	 */
	enum Flags
	{
		/** Default material. */
		SHADER_DEFAULT		= 0,
		/** Shader should enable lightmapping if supported. */
		SHADER_LIGHTMAPPING	= 1,
		/** Two sided material. */
		SHADER_TWOSIDED		= 2,
		/** Shader flags bits. */
		SHADER_MASK			= 0xFFFF,
	};

	/** 
	 * Begins rendering using the shader.
	 * Calls NS(Shader,begin)() in the constructor and NS(Shader,end)()
	 * in the destructor.
	 * Use this class instead of the methods directly for exception-safety.
	 * @see NS(Shader,begin)
	 */
	class Begin
	{
	public:
		/**
		 * Begins rendering using the shader.
		 * Calls NS(Shader,begin)().
		 */
		explicit Begin( Shader* shader )			: m_shader(shader) {m_passes=shader->begin();}

		/**
		 * Ends rendering using the shader.
		 * Calls NS(Shader,end)().
		 */
		~Begin()									{m_shader->end();}

		/** 
		 * Returns number of passes needed by the shader.
		 */
		int		passes() const						{return m_passes;}

	private:
		Shader*	m_shader;
		int		m_passes;
	};

	/** 
	 * Begins rendering specified pass using the shader.
	 * Calls NS(Shader,beginPass)() in the constructor and NS(Shader,endPass)()
	 * in the destructor.
	 * Use this class instead of the methods directly for exception-safety.
	 * @see NS(Shader,begin)
	 */
	class Pass
	{
	public:
		/**
		 * Starts rendering specified pass.
		 * Calls NS(Shader,beginPass)().
		 */
		Pass( Shader* shader, int pass )			: m_shader(shader) {shader->beginPass(pass);}

		/**
		 * Ends rendering specified pass.
		 * Calls NS(Shader,endPass)().
		 */
		~Pass()										{m_shader->endPass();}

	private:
		Shader*	m_shader;
	};

	Shader();

	/**
	 * Returns clone of this shader.
	 * All parameters are unique to each clone, but code
	 * and other constants are shared.
	 */
	virtual Shader*		clone() const = 0;

	/**
	 * Sets rendering technique to be used when rendering objects using this shader.
	 * If the requested technique is not supported, the shader is disabled
	 * until valid technique is set.
	 * @param name Name of the rendering technique to be set. Pass 0 or empty string to restore default technique.
	 * @see enabled
	 * @exception GraphicsException If technique is found but corrupted.
	 */
	virtual void		setTechnique( const char* name ) = 0;

	/**
	 * Sets shader texture parameter.
	 */
	virtual void		setTexture( ParamType param, BaseTexture* value ) = 0;

	/**
	 * Sets custom shader texture parameter which is not defined in ParamType.
	 */
	virtual void		setTexture( const char* name, BaseTexture* value ) = 0;

	/**
	 * Sets shader 4x4 matrix parameter.
	 */
	virtual void		setMatrix( ParamType param, const NS(math,float4x4)& value ) = 0;

	/**
	 * Sets custom shader 4x4 matrix parameter which is not defined in ParamType.
	 */
	virtual void		setMatrix( const char* name, const NS(math,float4x4)& value ) = 0;

	/**
	 * Sets shader 4x4 matrix array parameter.
	 */
	virtual void		setMatrixArray( ParamType param, NS(math,float4x4)** values, int count ) = 0;

	/**
	 * Sets shader 4-vector parameter.
	 */
	virtual void		setVector( ParamType param, const NS(math,float4)& value ) = 0;

	/**
	 * Sets custom shader 4-vector parameter which is not defined in ParamType.
	 */
	virtual void		setVector( const char* name, const NS(math,float4)& value ) = 0;

	/**
	 * Sets shader float parameter.
	 */
	virtual void		setFloat( ParamType param, float value ) = 0;

	/**
	 * Sets custom float parameter which is not defined in ParamType.
	 */
	virtual void		setFloat( const char* name, float value ) = 0;

	/**
	 * Gets shader texture parameter or 0 if not set.
	 */
	virtual BaseTexture*	getTexture( const char* name ) = 0;

	/**
	 * Gets shader matrix parameter or 0 if not set.
	 */
	virtual NS(math,float4x4)	getMatrix( const char* name ) = 0;

	/**
	 * Gets shader vector parameter or 0 if texture not set.
	 */
	virtual NS(math,float4)	getVector( const char* name ) = 0;

	/**
	 * Gets shader float parameter or 0 if not set.
	 */
	virtual float			getFloat( const char* name ) = 0;

	/**
	 * Begins rendering geometry using the shader.
	 * Prefer not using this method directly, but use Begin helper class instead.
	 * Rendering sequence:
	 * <ol>
	 * <li>First call NS(Shader,begin)() for the shader before rendering any geometry
	 * <li>Set shader parameters which need to be updated every frame, like transforms.
	 * <li>Call NS(Shader,beginPass)(0) to start rendering geometry using the first pass of the shader.
	 * <li>Call render() for each primitive to be rendered.
	 * <li>Call NS(Shader,endPass)() to stop rendering using this pass of the shader.
	 * <li>Loop to NS(Shader,beginPass)(i) for other passes, as many times as return value of begin() specified.
	 * <li>End rendering by calling NS(Shader,end)() for the shader.
	 * @return Number of passes to be rendered. 0 if shader is disabled.
	 */
	virtual int			begin() = 0;
	
	/**
	 * Begins rendering using specified pass. See begin() for description
	 * of the rendering sequence.
	 */
	virtual void		beginPass( int pass ) = 0;

	/**
	 * Ends rendering using specified pass. See begin() for description
	 * of the rendering sequence.
	 */
	virtual void		endPass() = 0;

	/**
	 * Ends rendering using this shader. See begin() for description
	 * of the rendering sequence.
	 */
	virtual void		end() = 0;

	/**
	 * Sets name of the shader.
	 */
	virtual void		setName( const NS(lang,String)& name ) = 0;

	/**
	 * Sets shader sorting mode.
	 */
	virtual void		setSort( SortType sort ) = 0;

	/**
	 * Returns priority of the shader.
	 * Higher priority shaders need to be rendered before lower priority ones.
	 * Default priority is 0. Priorities can be negative.
	 * Every even priority shader objects are drawn from front to back
	 * and odd priority shader objects from back to front.
	 */
	virtual int			priority() const = 0;

	/**
	 * Returns preferred sort mode of the shader.
	 * Default sort mode is SORT_NONE.
	 */
	virtual SortType	sort() const = 0;

	/**
	 * Returns true if the shader is enabled.
	 * @see setTechnique
	 */
	virtual bool		enabled() const = 0;

	/**
	 * Returns name of the shader.
	 */
	virtual const NS(lang,String)&		name() const = 0;

	/**
	 * Converts shader parameter type to string.
	 */
	static const char*	toString( ParamType param );

	/**
	 * Converts string to shader parameter type.
	 * @return PARAM_NONE if string not valid parameter name.
	 */
	static ParamType	toParamType( const char* sz );
};

	
END_NAMESPACE() // gr


#endif // _GR_SHADER_H

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
