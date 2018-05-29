#ifndef _HGR_PARTICLESYSTEM_H
#define _HGR_PARTICLESYSTEM_H
#ifndef HGR_NOPARTICLES


#include <gr/Primitive.h>
#include <hgr/Visual.h>
#include <hgr/impl/ParticleSystem_Integral.h>
#include <lang/Array.h>


BEGIN_NAMESPACE(gr) 
	class Shader;
	class Texture;END_NAMESPACE()

BEGIN_NAMESPACE(io) 
	class PropertyParser;
	class InputStream;
	class OutputStream;END_NAMESPACE()

BEGIN_NAMESPACE(lua) 
	class LuaTable;END_NAMESPACE()


BEGIN_NAMESPACE(hgr) 


class Camera;
class ResourceManager;


/**
 * Particle effect. Can be created directly or from Lua script.
 * 
 * Key features:
 * <ul>
 * <li>Variable number of emitters / single system
 * <li>Variable number of particles / emitter
 * <li>All properties can have different random variable value distribution
 * <li>Elastic, scaling, rotating etc. particle sprites are supported.
 * <li>Per particle transparency, color and animation texture frame are supported.
 * <li>Particles can use also user defined shaders.
 * </ul>
 *
 * Particle systems are heavily based on concept of 'domains'.
 * It is a bit abstract, but understanding it is essential
 * for effective usage of the particle systems. On the benefit
 * side once you understand it, using the particle systems
 * becomes very intuitive and orthogonal, as nearly all properties
 * of particle systems are simple defined by setting
 * appropriate value domains for each property.
 *
 * 'Domains' are source ranges for various random
 * values needed by particle systems.
 * Domains can be sources for scalar values, 
 * 2D and 3D vectors used in the simulation.
 *
 * For example by saying that some particle effect has
 * particle initial position domain set as 'Sphere with
 * radius 4' means that the initial positions of new 
 * particles are randomized inside sphere which has radius 4.
 * If in turn the particle initial velocity domain
 * is set as Point {-50,0,0} then all particles get
 * initial velocity of -50 along negative X-axis.
 *
 * With various domains it is easy to make pretty much
 * any kind of particle effect. For example rain
 * could be done by setting position box above sky,
 * and then velocity Point(0,-1,0) and Gravity=-9.8.
 * In addition, ParticleSpriteElasticity could be
 * set above 0 (say, 0.1) to stretch particle bitmap
 * along movement speed - the system is very flexible.
 *
 * @ingroup hgr
 */
class ParticleSystem :
	public NS(hgr,Visual)
{
public:
	/**
	 * Kill mode describes how max count limit is maintained.
	 */
	enum KillType
	{
		/** No items are killed when max limit is reached. */
		KILL_NONE,
		/** Oldest item is killed when max limit is reached. */
		KILL_OLDEST,
		/** Random item is killed when max limit is reached. */
		KILL_RANDOM,
		/** Number of kill modes. */
		KILL_COUNT
	};

	/** 
	 * View mode of particle sprites.
	 * View mode defines how particles are rotated when camera is tilted.
	 */
	enum ViewType
	{
		/**
		 * Particles are facing to camera and bitmaps Y-axis points up in camera space.
		 */
		VIEW_CAMERAUP,
		/**
		 * Particles are facing to camera and bitmaps Y-axis points up in world space.
		 */
		VIEW_WORLDUP,
		/**
		 * Number of view modes.
		 */
		VIEW_COUNT
	};

	/** 
	 * Animation type of particle texture.
	 * Defines how texture frames change over life time of particle effect.
	 */
	enum AnimationType
	{
		/**
		 * Animation frames are used as normal animation sequence.
		 * Framerate defines how often frame is changed to next one.
		 */
		ANIM_LOOP,
		/**
		 * Animation frames are used by particle life time.
		 * Framerate does not affect any way.
		 */
		ANIM_LIFE,
		/**
		 * Animation frames are used randomly.
		 * Framerate defines how often frame is randomized.
		 */
		ANIM_RANDOM,
		/**
		 * Number of animation types.
		 */
		ANIM_COUNT
	};

	/**
	 * Description of a particle system.
	 * Description objects are shared between all instances
	 * of the same particle effect.
	 * Normally this class is not used directly by the user as
	 * particle systems are loaded from scripts.
	 */
	class Description :
		public NS(lang,Object)
	{
	public:
		enum
		{
			/** Absolute maximum limit for number of particles. */ 
			MAX_PARTICLES_LIMIT = 65535/6 
		};

		/** Maximum number of simultaneous emissions. */
		int					systemMaxEmissions;

		/** Value domain for number of new emissions / second random (scalar) variable. */
		NS(math,Domain)		systemRate;

		/** Value domain for system stop time random (scalar) variable. */
		NS(math,Domain)		systemStopTime;

		/** Value domain for system life time random (scalar) variable. */
		NS(math,Domain)		systemLifeTime;

		/** Maximum number of particles. See MAX_PARTICLES_LIMIT. */
		int					emissionMaxParticles;

		/** Value domain for emission rate random (scalar) variable. */
		NS(math,Domain)		emissionRate;

		/** Value domain for emission stop time random (scalar) variable. */
		NS(math,Domain)		emissionStopTime;

		/** Value domain for emission life time random (scalar) variable. */
		NS(math,Domain)		emissionLifeTime;

		/** Value domain for emission pivot random (3-vector) variable. */
		NS(math,Domain)		emissionPosition;

		/** Value domain for particle life time random (scalar) variable. */
		NS(math,Domain)		particleLifeTime;
		
		/** Value domain for particle start velocity random (3-vector) variable. */
		NS(math,Domain)		particleStartVelocity;
		
		/** Value domain for particle start position random (3-vector) variable. */
		NS(math,Domain)		particleStartPosition;
		
		/** Value domain for particle start size random (scalar) variable. */
		NS(math,Domain)		particleStartSize;
		
		/** Value domain for particle end size random (scalar) variable. */
		NS(math,Domain)		particleEndSize;
		
		/** Value domain for particle start transparency random (scalar) variable. 1 is opaque. */
		NS(math,Domain)		particleStartAlpha;
		
		/** Value domain for particle end transparency random (scalar) variable. 1 is opaque. */
		NS(math,Domain)		particleEndAlpha;
		
		/** Value domain for particle start color random (3-vector) variable. (1,1,1) is white. */
		NS(math,Domain)		particleStartColor;
		
		/** Value domain for particle end color random (3-vector) variable. (1,1,1) is white. */
		NS(math,Domain)		particleEndColor;
		
		/** 
		 * Value domain for particle elasticity random (scalar) variable. 
 		 * Good values 0-0.2, but out-of-range values can create interesting effects. 
		 */
		NS(math,Domain)		particleSpriteElasticity;
		
		/** Value domain for particle sprite start rotation (radians) random (scalar) variable. */
		NS(math,Domain)		particleSpriteRotation;
		
		/** Value domain for particle sprite start rotation speed (rad/s) random (scalar) variable. */
		NS(math,Domain)		particleSpriteStartRotationSpeed;
		
		/** Value domain for particle sprite end rotation speed (rad/s) random (scalar) variable. */
		NS(math,Domain)		particleSpriteEndRotationSpeed;
		
		/** Gravity in world space Y-axis direction, m/(s*s). */
		float				gravity;
		
		/** Wind in world space, m/s. */
		NS(math,float3)		wind;

		/** Texture animation frame change rate, changes/second. */
		float				textureFrameRate;
				
		/** Number of texture frames embedded to the texture bitmap in NxN grid. Must be square of integer. */
		int					textureFrames;

		/** Name of the particle texture (without path) */
		NS(lang,String)		textureName;

		/** Name of the particle shader */
		NS(lang,String)		shaderName;

		/** Texture (animation) for the particle effect. */
		P(NS(gr,Texture))		texture;
		
		/** Sprite shader used to render particles. */
		P(NS(gr,Shader))		shader;
		
		/** Defines how texture frames change over life time of particle effect. */
		AnimationType		textureAnimation;
		
		/** View mode defines how particles are rotated when camera is tilted. */
		ViewType			textureView;
		
		/** Kill mode describes how max particle limit is maintained. */
		KillType			emissionLimitKill;

		/** Kill mode describes how max emission limit is maintained. */
		KillType			systemLimitKill;

		/** If true then particles are updated even though they are not visible. */
		bool				particleUpdateAlways;

		/** True if particles are aligned at birth moment by setUserNormal field. */
		bool				particleAlignedToUserNormal;

		Description();

		/**
		 * Writes particle system description to output stream.
		 */
		void	write( NS(io,OutputStream)* s );

		/**
		 * Reads particle system description from input stream.
		 */
		void	read( NS(io,InputStream)* s );

	private:
		friend class ParticleSystem;

		void	write( NS(io,OutputStream)* s, int v, const NS(lang,String)& variablename, const NS(lang,String)& description );
		void	write( NS(io,OutputStream)* s, bool v, const NS(lang,String)& variablename, const NS(lang,String)& description );
		void	write( NS(io,OutputStream)* s, float v, const NS(lang,String)& variablename, const NS(lang,String)& description );
		void	write( NS(io,OutputStream)* s, const NS(math,float3)& v, const NS(lang,String)& variablename, const NS(lang,String)& description );
		void	write( NS(io,OutputStream)* s, const NS(math,Domain)& v, const NS(lang,String)& variablename, const NS(lang,String)& description );
		void	write( NS(io,OutputStream)* s, const char* v, const NS(lang,String)& variablename, const NS(lang,String)& description );
		void	write( NS(io,OutputStream)* s, const char* sz );
		void	writeDescription( NS(io,OutputStream)* s, const NS(lang,String)& description );

		void	read( NS(io,PropertyParser)& prop, int& v, const char* variablename );
		void	read( NS(io,PropertyParser)& prop, bool& v, const char* variablename );
		void	readOpt( NS(io,PropertyParser)& prop, bool& v, const char* variablename, bool defaultval );
		void	read( NS(io,PropertyParser)& prop, float& v, const char* variablename );
		void	read( NS(io,PropertyParser)& prop, NS(lang,String)& v, const char* variablename );
		void	read( NS(io,PropertyParser)& prop, NS(math,float3)& v, const char* variablename );
		void	read( NS(io,PropertyParser)& prop, NS(math,Domain)& v, const char* variablename );
		void	read( NS(io,PropertyParser)& prop, AnimationType& v, const char* variablename );
		void	read( NS(io,PropertyParser)& prop, ViewType& v, const char* variablename );
		void	read( NS(io,PropertyParser)& prop, KillType& v, const char* variablename );

		void	check( const NS(lang,String)& filename ) const;

		// intermediate values cached from the description
		float					textureFrameUV;
		NS(lang,Array)<float>	uvcache;
	};

	/* Single particle. Private implementation class. */
	class Particle
	{
	public:
		float										time;
		float										life;
		float										rot;
		float										elasticity;
		ParticleSystem_Integral<float>				size;
		ParticleSystem_Integral<NS(math,float3)>	color;
		ParticleSystem_Integral<float>				alpha;
		ParticleSystem_Integral<float>				rotspeed;
		int											frame;
		NS(math,float3)								velocity;
		NS(math,float3)								position;
		NS(math,float3x3)							userRot; // world space
	};

	/* Single emission from the particle system. Contains many particles. Private implementation class. */
	class Emission
	{
	public:
		float					time;
		float					stop;
		float					life;
		float					newParticles;
		NS(math,float3)			position;
		NS(lang,Array)<Particle>	particles;
	};

	/**
	 * Loads a particle system from a file.
	 * @param context Rendering context to be used while loading.
	 * @param filename Scene file name relative to current working directory.
	 * @param res Resource manager to load textures and shaders from. 0 for default.
	 * @param texturepath Texture path relative to current working directory.
	 * @param shaderpath Shader path relative to current working directory.
	 * @exception IOException
	 * @exception LuaException
	 * @exception GraphicsException
	 */
	ParticleSystem( NS(gr,Context)* context, const NS(lang,String)& filename, ResourceManager* res=0,
		const NS(lang,String)& texturepath="", const NS(lang,String)& shaderpath="" );

	/**
	 * Makes particle system using user provided description object.
	 */
	explicit ParticleSystem( Description* desc );

	/**
	 * Creates copy of particle system. Shares non-instance-specific data.
	 */
	explicit ParticleSystem( const ParticleSystem& other );

	~ParticleSystem();

	/**
	 * Returns clone of this node. Clones child nodes as well.
	 */
	virtual Node*	clone() const;

	/**
	 * Loads particle system from file.
	 * @exception IOException
	 * @exception LuaException
	 * @exception GraphicsException
	 */
	void	load( NS(gr,Context)* context, const NS(lang,String)& filename, ResourceManager* res,
				const NS(lang,String)& texturepath="", const NS(lang,String)& shaderpath="" );

	/**
	 * Updates particle animation.
	 * @param dt Time interval to update in seconds.
	 */
	void	update( float dt );

	/** 
	 * Renders this particle system to the active device.
	 * @param context Rendering context.
	 * @param camera Active camera.
	 * @param priority Shader priority level which is to be rendered.
	 */
	void	render( NS(gr,Context)* context, Camera* camera, int priority );

	/**
	 * Returns array of shaders used by this visual.
	 * NOTE: Implementation should not clear the list of returned shaders
	 * before adding new ones, since shadesr from multiple Visuals might be 
	 * collected at the same time.
	 */
	void	getShaders( NS(lang,Array)<NS(gr,Shader)*>& shaders );

	/**
	 * Sets rendering technique of the shaders used by this mesh.
	 */
	void	setTechnique( const char* name );

	/**
	 * Resets particle system to its initial state.
	 */
	void	reset();

	/**
	 * Computes bounding volume of the visual in model space.
	 */
	void	computeBound();

	/**
	 * Sets time (seconds) to delay particle system instance simulation start.
	 * Default delay is 0.
	 * @param time Delay in seconds.
	 */
	void	setDelay( float time );

	/**
	 * Sets velocity of parent node in world space to be taken into account while
	 * computing sprite elasticity effect.
	 */
	void	setParentVelocity( const NS(math,float3)& vel );

	/** 
	 * Sets user defined normal (normalized, in world space). 
	 * @see particleAlignedToUserNormal
	 */
	void	setUserNormal( const NS(math,float3)& normal );

	/**
	 * Returns description used by this particle system.
	 */
	Description*	description()					{return m_desc;}

	/**
	 * Returns total number of active particles.
	 */
	int				particles() const;

	/**
	 * Returns description used by this particle system.
	 */
	const Description*		description() const		{return m_desc;}

	/**
	 * Converts string to ViewType.
	 * @return VIEW_COUNT if not found.
	 */ 
	static ViewType			toViewType( const char* sz );

	/**
	 * Converts string to AnimationType.
	 * @return ANIMATION_COUNT if not found.
	 */ 
	static AnimationType	toAnimationType( const char* sz );

	/**
	 * Converts string to KillType.
	 * @return KILL_COUNT if not found.
	 */ 
	static KillType			toKillType( const char* sz );

private:
	P(Description)				m_desc;
	NS(lang,Array)<Emission>	m_emissions;
	float						m_time;
	float						m_timeSinceRender;
	float						m_newEmissions;
	float						m_systemStopTime;
	float						m_systemLifeTime;
	float						m_delay;
	NS(math,float3)				m_parentVelocity;
	NS(math,float3x3)			m_userRot;
	P(NS(gr,Primitive))			m_prim;

	void		renderDX( NS(gr,Context)* context, Camera* camera );
	void		renderN3D( NS(gr,Context)* context, Camera* camera );
	void		restart();
	void		refreshCachedValues();
	static void	getDomain( const NS(lang,String)& particlename, NS(lua,LuaTable)& tab, const NS(lang,String)& domainname, NS(math,Domain)* domain );
	static int	log2i( int x );

	template <class T> static void	killOld( NS(lang,Array)<T>& array );
	template <class T> static T*	getNew( NS(lang,Array)<T>& array, int limit, KillType killtype );

	ParticleSystem& operator=( const ParticleSystem& );
};


END_NAMESPACE() // hgr


#endif // HGR_NOPARTICLES
#endif // _HGR_PARTICLESYSTEM_H
