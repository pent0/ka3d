/*
Bounce3D/N3D particle system notes:

- particleAlignedToUserNormal supported only on N3D

- N3D doesnt support depth write toggle yet, so additive particles (fire etc) look crap

- For best performance:
textureView = CAMERAUP
particleSpriteRotation = CONSTANT 0 
particleSpriteElasticity = CONSTANT 0 

- Unsupported properties:
particleStartAlpha
particleEndAlpha
particleStartColor
particleEndColor
*/
#ifndef HGR_NOPARTICLES
#include <hgr/ParticleSystem.h>
#include <gr/Shader.h>
#include <gr/Context.h>
#include <gr/Texture.h>
#include <io/FindFile.h>
#include <io/PathName.h>
#include <io/IOException.h>
#include <io/PropertyParser.h>
#include <io/FileInputStream.h>
#include <io/FileOutputStream.h>
#include <hgr/Camera.h>
#include <hgr/DefaultResourceManager.h>
#include <lang/pp.h>
#include <lang/Math.h>
#include <lang/Debug.h>
#include <lang/Float.h>
#include <lang/Profile.h>
#include <lang/TempBuffer.h>
#include <lang/algorithm/swap.h>
#include <math/float.h>
#include <math/float2.h>
#include <math/toString.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <config.h>


USING_NAMESPACE(gr)
USING_NAMESPACE(io)
USING_NAMESPACE(lua)
USING_NAMESPACE(lang)
USING_NAMESPACE(math)


BEGIN_NAMESPACE(hgr) 


static const char* const VIEW_NAMES[] =
{
	"CAMERAUP",
	"WORLDUP"
};

static const char* const ANIM_NAMES[] =
{
	"LOOP",
	"LIFE",
	"RANDOM"
};

static const char* const KILL_NAMES[] =
{
	"NONE",
	"OLDEST",
	"RANDOM"
};


static int findsz( const char* const* tab, const char* var, int count )
{
	for ( int i = 0 ; i < count ; ++i )
	{
		if ( !strcmp(tab[i],var) )
			return i;
	}
	return count;
}


ParticleSystem::Description::Description() :
	systemMaxEmissions(0),
	emissionMaxParticles(0),
	gravity(0),
	wind(0,0,0),
	textureFrameRate(0),
	textureFrames(0),
	texture(0),
	shader(0),
	textureAnimation(ANIM_COUNT),
	textureView(VIEW_COUNT),
	emissionLimitKill(KILL_NONE),
	particleUpdateAlways(false),
	particleAlignedToUserNormal(false),
	textureFrameUV(0),
	uvcache()
{
}

void ParticleSystem::Description::write( OutputStream* s, int v, const String& variablename, const String& description )
{
	writeDescription( s, description );
	char buf[2048];
	sprintf( buf, "%s = %d\n", variablename.c_str(), v );
	s->write( buf, strlen(buf) );
}

void ParticleSystem::Description::write( OutputStream* s, bool v, const String& variablename, const String& description )
{
	writeDescription( s, description );
	char buf[2048];
	sprintf( buf, "%s = %s\n", variablename.c_str(), (v?"true":"false") );
	s->write( buf, strlen(buf) );
}

void ParticleSystem::Description::write( OutputStream* s, float v, const String& variablename, const String& description )
{
	writeDescription( s, description );
	char buf[2048];
	sprintf( buf, "%s = %g\n", variablename.c_str(), v );
	s->write( buf, strlen(buf) );
}

void ParticleSystem::Description::write( OutputStream* s, const char* v, const String& variablename, const String& description )
{
	writeDescription( s, description );
	char buf[2048];
	sprintf( buf, "%s = %s\n", variablename.c_str(), v );
	s->write( buf, strlen(buf) );
}

void ParticleSystem::Description::write( OutputStream* s, const float3& v, const String& variablename, const String& description )
{
	writeDescription( s, description );
	char buf[2048];
	sprintf( buf, "%s = %g %g %g\n", variablename.c_str(), v.x, v.y, v.z );
	s->write( buf, strlen(buf) );
}

void ParticleSystem::Description::write( OutputStream* s, const Domain& v, const String& variablename, const String& description )
{
	writeDescription( s, description );
	char buf[2048];
	sprintf( buf, "%s = %s ", variablename.c_str(), v.toString() );
	s->write( buf, strlen(buf) );

	for ( int i = 0 ; i < v.getParameterCount(v.type()) ; ++i )
	{
		sprintf( buf, "%g ", v.getParameter(i) );
		s->write( buf, strlen(buf) );
	}

	sprintf( buf, "\n" );
	s->write( buf, strlen(buf) );
}

void ParticleSystem::Description::writeDescription( OutputStream* s, const String& description )
{
	s->write( "\n-- ", 4 );
	s->write( description.c_str(), description.length() );
	s->write( "\n", 1 );
}

void ParticleSystem::Description::write( OutputStream* s, const char* sz )
{
	s->write( sz, strlen(sz) );
}

void ParticleSystem::Description::write( OutputStream* s )
{
	write( s, 
"-- \n\
-- Particle systems are heavily based on concept of 'domains'. \n\
-- 'Domains' are source ranges for various random \n\
-- values needed by particle systems. \n\
-- Domains can be sources for scalar values,  \n\
-- 2D and 3D vectors used in the simulation. \n\
-- \n\
-- For example by saying that some particle effect has \n\
-- particle initial position domain set as 'Sphere with \n\
-- radius 4' means that the initial positions of new  \n\
-- particles are randomized inside sphere which has radius 4. \n\
-- If in turn the particle initial velocity domain \n\
-- is set as Point {-50,0,0} then all particles get \n\
-- initial velocity of -50 along negative X-axis. \n\
-- \n\
-- With various domains it is easy to make pretty much \n\
-- any kind of particle effect. For example rain \n\
-- could be done by setting position box above sky, \n\
-- and then velocity Point(0,-1,0) and Gravity=-9.8. \n\
-- In addition, ParticleSpriteElasticity could be \n\
-- set above 0 (say, 0.1) to stretch particle bitmap \n\
-- along movement speed - the system is very flexible. \n\
-- \n\
-- CONSTANT x              -- CONSTANT -1 means infinity if only positive values are expected \n\
-- RANGE begin end \n\
-- POINT x y z \n\
-- SPHERE x y z inradius outradius \n\
-- LINE x1 y1 z1 x2 y2 z2 \n\
-- BOX minx miny minz maxx maxy maxz \n\
-- CYLINDER x1 y1 z1 x2 y2 z2 inradius outradius \n\
-- DISK ox oy oz nx ny nz inradius outradius \n\
-- RECTANGLE ox oy oz ux uy uz vx vy vz \n\
-- TRIANGLE vx0 vy0 vz0 vx1 vy1 vz1 vx2 vy2 vz2 \n\
-- \n\
-- .. where: \n\
-- inradius    = min radius in which particles are born \n\
-- outradius   = max radius in which particles are born \n\
-- x1 y1 z1    = start Point coordinates \n\
-- x2 y2 z2    = end Point coordinates \n\
-- ox oy oz    = origin coordinates \n\
-- nx ny nz    = plane normal coordinates \n\
-- ux uy uz    = vector from origin to 'horizontal' edge \n\
-- vx vy vz    = vector from origin to 'vertical' edge \n\
-- vx0 vy0 vz0 = first vertex of Triangle \n\
-- vx1 vy1 vz1 = second vertex of Triangle \n\
-- vx2 vy2 vz2 = third vertex of Triangle \n\
--\n\n" );

	write( s, "-----------------------\n" );
	write( s, "-- System properties --\n" );
	write( s, "-----------------------\n" );
	write( s, systemMaxEmissions, "systemMaxEmissions", "Maximum number of simultaneous emissions" );
	write( s, KILL_NAMES[systemLimitKill], "systemLimitKill", "How emission limit is maintained, kill: NONE, OLDEST, RANDOM" );
	write( s, systemRate, "systemRate", "Number of new emissions / second" );
	write( s, systemStopTime, "systemStopTime", "Time after no more new emissions are launched" );
	write( s, systemLifeTime, "systemLifeTime", "Lifetime of the whole system" );

	write( s, "\n----------------------------------\n" );
	write( s, "-- Particle emission properties --\n" );
	write( s, "----------------------------------\n" );
	write( s, emissionMaxParticles, "emissionMaxParticles", "Maximum number of particles simultanously" );
	write( s, KILL_NAMES[emissionLimitKill], "emissionLimitKill", "How max particle limit is maintained: kill NONE, OLDEST, RANDOM" );
	write( s, emissionRate, "emissionRate", "How max particle limit is maintained: kill NONE, OLDEST, RANDOM" );
	write( s, emissionStopTime, "emissionStopTime", "Time after no more new particles are emitted" );
	write( s, emissionLifeTime, "emissionLifeTime", "Lifetime of particle emitter" );
	write( s, emissionPosition, "emissionPosition", "Pivot point for the emissions" );

	write( s, "\n-------------------------\n" );
	write( s, "-- Particle properties --\n" );
	write( s, "-------------------------\n" );
	write( s, particleLifeTime, "particleLifeTime", "Particle life time in seconds" );
	write( s, particleAlignedToUserNormal, "particleAlignedToUserNormal", "If true then particles are aligned by user normal specified by the application" );
	write( s, particleUpdateAlways, "particleUpdateAlways", "If true then particles are updated even though they are not visible. Note: Normally this should be set to false" );
	write( s, particleStartPosition, "particleStartPosition", "Volume in which the particles are born" );
	write( s, particleStartVelocity, "particleStartVelocity", "Domain for velocity vector values. This works the same way as with position, but instead of randomizing initial positions for the particles this time velocity vectors are randomized so that the length and direction of the velocity is randomized from the specified domain when the velocity vector is considered to start at origin." );
	write( s, particleStartSize, "particleStartSize", "Start size of particle" );
	write( s, particleEndSize, "particleEndSize", "Life time end size of particle" );
	write( s, particleStartAlpha, "particleStartAlpha", "Start opacity of particle" );
	write( s, particleEndAlpha, "particleEndAlpha", "End opacity of particle" );
	write( s, particleStartColor, "particleStartColor", "Start color (R,G,B in range 0-1) of particle" );
	write( s, particleEndColor, "particleEndColor", "End color (R,G,B in range 0-1) of particle" );
	write( s, particleSpriteElasticity, "particleSpriteElasticity", "Particle sprite elasticity wrt particle velocity, set Constant(0) to disable (note that elasticity and rotation are mutually exclusive)" );
	write( s, particleSpriteRotation, "particleSpriteRotation", "Particle sprite initial rotation (degrees) (note that elasticity and rotation are mutually exclusive)" );
	write( s, particleSpriteStartRotationSpeed, "particleSpriteStartRotationSpeed", "Particle sprite initial rotation speed (degrees/sec)" );
	write( s, particleSpriteEndRotationSpeed, "particleSpriteEndRotationSpeed", "Particle sprite end rotation speed (degrees/sec)" );

	write( s, "\n-------------------------------\n" );
	write( s, "-- Particle bitmap animation --\n" );
	write( s, "-------------------------------\n" );
	write( s, VIEW_NAMES[textureView], "textureView", "View mode: CAMERAUP, WORLDUP. View mode defines are particles rotated when camera rotates along Z-axis. If view mode is WORLDUP, then particle 'up' direction is the same as world Y-axis. If view mode is CAMERAUP, then particle 'up' direction is the same as camera Y-axis. Note: View mode does not affect if ParticleSpriteElasticity is set as it particle sprite elasticity defines the 'up' direction based on particle velocity" );
	write( s, textureName.c_str(), "textureName", "Texture bitmap file name" );
	write( s, ANIM_NAMES[textureAnimation], "textureAnimation", "Method of selecting which frame to display: LOOP plays back animation in Constant frame rate (TextureFramerate). LIFE starts from first frame and gradually animates until end-of-life. RANDOM selects random frame in interval defined by TextureFramerate" );
	write( s, shaderName.c_str(), "shaderName", "Shader name used to render particles" );
	write( s, textureFrameRate, "textureFrameRate", "Playback rate if TextureAnimation is LOOP" );
	write( s, textureFrames, "textureFrames", "Number of frames embedded to texture (nxn grid)" );

	write( s, "\n--------------------------------\n" );
	write( s, "-- Forces affecting particles --\n" );
	write( s, "--------------------------------\n" );
	write( s, gravity, "gravity", "Gravity affecting particles (m/s^2)" );
	write( s, wind, "wind", "Wind affecting particles (m/s)" );
}

void ParticleSystem::Description::read( PropertyParser& prop, int& v, const char* variablename )
{
	v = prop.getInt( variablename );
}

void ParticleSystem::Description::read( PropertyParser& prop, bool& v, const char* variablename )
{
	v = prop.getBoolean( variablename );
}

void ParticleSystem::Description::readOpt( PropertyParser& prop, bool& v, const char* variablename, bool defaultval )
{
	if ( prop.hasKey(variablename) )
		v = prop.getBoolean( variablename );
	else
		v = defaultval;
}

void ParticleSystem::Description::read( PropertyParser& prop, float& v, const char* variablename )
{
	v = prop.getFloat( variablename );
}

void ParticleSystem::Description::read( PropertyParser& prop, String& v, const char* variablename )
{
	v = prop.getString( variablename );
}

void ParticleSystem::Description::read( PropertyParser& prop, float3& v, const char* variablename )
{
	const char* buf = prop.getString( variablename );
	if ( sscanf(buf,"%g %g %g",&v.x,&v.y,&v.z) != 3 )
		throwError( IOException(Format("Failed to parse 3-vector {0} in file {1}", variablename, prop.name())) );
}

void ParticleSystem::Description::read( PropertyParser& prop, ParticleSystem::AnimationType& v, const char* variablename )
{
	String str;
	read( prop, str, variablename );
	const int count = ANIM_COUNT;
	v = (AnimationType)findsz( ANIM_NAMES, str.c_str(), count );
	if ( v == count )
		throwError( IOException( Format("Failed to read AnimationType {0} from {1}", variablename, prop.name()) ) );
}

void ParticleSystem::Description::read( PropertyParser& prop, ParticleSystem::ViewType& v, const char* variablename )
{
	String str;
	read( prop, str, variablename );
	const int count = VIEW_COUNT;
	v = (ViewType)findsz( VIEW_NAMES, str.c_str(), count );
	if ( v == count )
		throwError( IOException( Format("Failed to read ViewType {0} from {1}", variablename, prop.name()) ) );
}

void ParticleSystem::Description::read( PropertyParser& prop, ParticleSystem::KillType& v, const char* variablename )
{
	String str;
	read( prop, str, variablename );
	const int count = KILL_COUNT;
	v = (KillType)findsz( KILL_NAMES, str.c_str(), count );
	if ( v == count )
		throwError( IOException( Format("Failed to read KillType {0} from {1}", variablename, prop.name()) ) );
}

void ParticleSystem::Description::read( PropertyParser& prop, Domain& v, const char* variablename )
{
	TempBuffer<char> buf( 256 );
	const char* val = prop.getString( variablename );
	if ( sscanf(val,"%s",buf.begin()) != 1 )
		throwError( IOException(Format("Failed to value domain type string from variable {0} in file {1}", variablename, prop.name())) );

	String typestr = String(buf.begin()).toUpperCase();
	Domain::DomainType type = Domain::toDomainType( typestr.c_str() );
	if ( type == Domain::DOMAIN_COUNT )
		throwError( IOException(Format("Invalid value domain type string in variable {0} in file {1}", variablename, prop.name())) );

	int paramcount = Domain::getParameterCount( type );
	assert( paramcount <= 16 );
	char valfmt[256];
	strcpy( valfmt, "%s" );
	for ( int i = 0 ; i < paramcount ; ++i )
		strcat( valfmt+strlen(valfmt), " %g" );
	float params[16];
	if ( sscanf(val, valfmt, buf.begin(), params+0, params+1, params+2, params+3, params+4, params+5, params+6, params+7, params+8, params+9, params+10, params+11, params+12, params+13, params+14, params+15) != paramcount+1 )
		throwError( IOException(Format("Failed to parse value domain {0} in file {1}", variablename, prop.name())) );

	v.setType( type );
	for ( int i = 0 ; i < paramcount ; ++i )
		v.setParameter( i, params[i] );
}

void ParticleSystem::Description::read( InputStream* s )
{
	int bytesavailable = s->available();
	TempBuffer<char> buf( bytesavailable+1 );
	int bytesread = s->read( buf.begin(), bytesavailable );
	if ( bytesread != bytesavailable )
		throwError( IOException( Format("Failed to read particle system {0}", s->toString()) ) );
	buf[bytesavailable] = 0;
	PropertyParser prop( buf.begin(), s->toString() );

	read( prop, systemMaxEmissions, "systemmaxemissions" );
	read( prop, systemRate, "systemrate" );
	read( prop, systemStopTime, "systemstoptime" );
	read( prop, systemLifeTime, "systemlifetime" );
	read( prop, emissionMaxParticles, "emissionmaxparticles" );
	read( prop, emissionRate, "emissionrate" );
	read( prop, emissionStopTime, "emissionstoptime" );
	read( prop, emissionLifeTime, "emissionlifetime" );
	read( prop, emissionPosition, "emissionposition" );
	readOpt( prop, particleAlignedToUserNormal, "particlealignedtousernormal", false );
	read( prop, particleLifeTime, "particlelifetime" );
	read( prop, particleStartVelocity, "particlestartvelocity" );
	read( prop, particleStartPosition, "particlestartposition" );
	read( prop, particleStartSize, "particlestartsize" );
	read( prop, particleEndSize, "particleendsize" );
	read( prop, particleStartAlpha, "particlestartalpha" );
	read( prop, particleEndAlpha, "particleendalpha" );
	read( prop, particleStartColor, "particlestartcolor" );
	read( prop, particleEndColor, "particleendcolor" );
	read( prop, particleSpriteElasticity, "particlespriteelasticity" );
	read( prop, particleSpriteRotation, "particlespriterotation" );
	read( prop, particleSpriteStartRotationSpeed, "particlespritestartrotationspeed" );
	read( prop, particleSpriteEndRotationSpeed, "particlespriteendrotationspeed" );
	read( prop, gravity, "gravity" );
	read( prop, wind, "wind" );
	read( prop, textureFrameRate, "textureframerate" );
	read( prop, textureFrames, "textureframes" );
	read( prop, textureAnimation, "textureanimation" );
	read( prop, textureView, "textureview" );
	read( prop, emissionLimitKill, "emissionlimitkill" );
	read( prop, systemLimitKill, "systemlimitkill" );
	read( prop, particleUpdateAlways, "particleupdatealways" );
	read( prop, textureName, "texturename" );
	read( prop, shaderName, "shadername" );
}

void ParticleSystem::Description::check( const String& filename ) const
{
	bool spriteRotationEnabled = !particleSpriteRotation.isZero() || !particleSpriteStartRotationSpeed.isZero();

	if ( systemMaxEmissions < 1 || systemMaxEmissions > Description::MAX_PARTICLES_LIMIT )
		throwError( IOException( Format("Invalid value in \"{0}\": SystemMaxEmissions={1} (out-of-range, max is {2})", filename, systemMaxEmissions, Description::MAX_PARTICLES_LIMIT) ) );
	if ( systemLimitKill == KILL_COUNT )
		throwError( IOException( Format("Invalid value  in \"{0}\": systemLimitKill invalid", filename) ) );
	if ( emissionMaxParticles < 1 || emissionMaxParticles >= Description::MAX_PARTICLES_LIMIT )
		throwError( IOException( Format("Invalid value in \"{0}\": MaxParticles={1} (out-of-range, max is {2})", filename, emissionMaxParticles, Description::MAX_PARTICLES_LIMIT) ) );
	if ( emissionLimitKill == KILL_COUNT )
		throwError( IOException( Format("Invalid value  in \"{0}\": emissionLimitKill invalid", filename) ) );
	if ( !particleSpriteElasticity.isZero() && spriteRotationEnabled )
		throwError( IOException( Format("Invalid combination in \"{0}\": Sprite rotation and elasticity are mutually exclusive", filename) ) );
	if ( !particleSpriteElasticity.isZero() && particleAlignedToUserNormal )
		throwError( IOException( Format("Invalid combination in \"{0}\": particleAlignedToUserNormal and elasticity are mutually exclusive", filename) ) );
	if ( !(textureFrameRate >= 0.f && textureFrameRate <= 100.f) )
		throwError( IOException( Format("Invalid value in \"{0}\": textureFrameRate={1} (out-of-range 0-100)", filename, textureFrameRate) ) );
	if ( textureFrames < 1 || textureFrames != (1<<log2i(textureFrames)) )
		throwError( IOException( Format("Invalid value in \"{0}\": TextureFrames={1} (not square of integer)", filename, textureFrames) ) );
	if ( textureAnimation == ANIM_COUNT )
		throwError( IOException( Format("Invalid value  in \"{0}\": textureAnimation invalid", filename) ) );
	if ( textureView == VIEW_COUNT )
		throwError( IOException( Format("Invalid value  in \"{0}\": particleView invalid", filename) ) );
}

ParticleSystem::ParticleSystem( Context* context, const String& filename, ResourceManager* res,
	const String& texturepath, const String& shaderpath ) :
	m_desc( new Description ),
	m_emissions(),
	m_time( 0.f ),
	m_timeSinceRender( 0.f ),
	m_newEmissions( 0.f ),
	m_systemStopTime( 0.f ),
	m_systemLifeTime( 0.f ),
	m_delay( 0.f ),
	m_parentVelocity( 0, 0, 0 )
{
	setClassId( NODE_PARTICLESYSTEM );

	load( context, filename, res, texturepath, shaderpath );
}

ParticleSystem::ParticleSystem( const ParticleSystem& other ) :
	Visual( other ),
	m_desc( other.m_desc ),
	m_emissions( other.m_emissions ),
	m_time( other.m_time ),
	m_timeSinceRender( other.m_timeSinceRender ),
	m_newEmissions( other.m_newEmissions ),
	m_systemStopTime( other.m_systemStopTime ),
	m_systemLifeTime( other.m_systemLifeTime ),
	m_delay( other.m_delay ),
	m_parentVelocity( other.m_parentVelocity ),
	m_userRot( other.m_userRot )
{
	reset();
}

ParticleSystem::ParticleSystem( Description* desc ) :
	m_desc( desc ),
	m_emissions(),
	m_time( 0.f ),
	m_timeSinceRender( 0.f ),
	m_newEmissions( 0.f ),
	m_systemStopTime( 0.f ),
	m_systemLifeTime( 0.f ),
	m_delay( 0.f ),
	m_parentVelocity( 0, 0, 0 ),
	m_userRot( 1.f )
{
	setClassId( NODE_PARTICLESYSTEM );

	reset();
}

ParticleSystem::~ParticleSystem()
{
}

Node* ParticleSystem::clone() const
{
	return new ParticleSystem( *this );
}

void ParticleSystem::load( Context* context, const String& filename, ResourceManager* res,
	const String& texturepath, const String& shaderpath )
{
	setName( filename );

	// directories to read data from
	// (char arrays used as memory usage optimization)
	PathName parentpath = PathName(filename).parent();

	char shaderpathsz[PathName::MAXLEN];
	if ( shaderpath != "" )
		shaderpath.get( shaderpathsz, sizeof(shaderpathsz) );
	else
		String::cpy( shaderpathsz, sizeof(shaderpathsz), parentpath.toString() );
	
	char texturepathsz[PathName::MAXLEN];
	if ( texturepath != "" )
		texturepath.get( texturepathsz, sizeof(texturepathsz) );
	else
		String::cpy( texturepathsz, sizeof(texturepathsz), parentpath.toString() );

	FileInputStream in( filename );
	m_desc->read( &in );
	m_desc->check( filename );

	// load used resources (texture and shader)
	if ( !res )
		res = DefaultResourceManager::get( context );
	m_desc->texture = res->getTexture( PathName(texturepathsz,m_desc->textureName).toString() );
	m_desc->shader = res->getShader( PathName(shaderpathsz,m_desc->shaderName).toString(), Shader::SHADER_DEFAULT );
	m_desc->shader->setTexture( "BASEMAP", m_desc->texture );

	reset();
}

void ParticleSystem::update( float dt )
{
	// update time
	m_time += dt;

	// delay simulation (start)
	if ( m_time < m_delay )
		return;

	// skip the update if the effect has not been visible for some time,
	// unless updates are forced, e.g. explosion or something like that.
	// note: we cannot skip update if there is no particles, since
	// effects without particles are not rendered in any case
	if ( !m_desc->particleUpdateAlways && 
		m_emissions.size() > 0 &&
		m_timeSinceRender > 1.f &&
		particles() > 0 )
	{
		return;
	}
	
	// update time since particle effect was rendered
	m_timeSinceRender += dt;

	// kill old emissions and particles
	killOld( m_emissions );
	for ( int i = 0 ; i < m_emissions.size() ; ++i )
		killOld( m_emissions[i].particles );

	// create new emissions
	if ( m_systemStopTime < 0.f || m_time-m_delay < m_systemStopTime )
	{
		m_newEmissions += m_desc->systemRate.getRandomFloat() * dt;
		m_newEmissions = clamp( m_newEmissions, 0.f, (float)m_desc->systemMaxEmissions );

		for ( ; m_newEmissions >= 1.f ; m_newEmissions -= 1.f )
		{
			// delete emissions by KillType if max limit reached
			Emission* newitem = getNew( m_emissions, m_desc->systemMaxEmissions, m_desc->systemLimitKill );

			// a new emission can be created?
			if ( newitem != 0 )
			{
				Emission& emission = *newitem;

				emission.time = 0.f;
				emission.stop = m_desc->emissionStopTime.getRandomFloat();
				emission.life = m_desc->emissionLifeTime.getRandomFloat();
				emission.newParticles = 0.f;
				emission.position = m_desc->emissionPosition.getRandomFloat3();
				emission.particles.clear();
			}
		}
	}

	// integrate forces shared between all emissions and particles
	float3 sharedforceintegral( 0.f, 0.f, 0.f );
	sharedforceintegral.y += m_desc->gravity * dt;

	// prepare values shared between all emissions and particles
	const float		frametime		= (m_desc->textureFrameRate > Float::MIN_VALUE ? 1.f / m_desc->textureFrameRate : Float::MAX_VALUE);
	const int		textureframes	= m_desc->textureFrames;
	const float		textureframesf	= (float)textureframes;

	// prepare bounding volume computation
	float3 boundminv( MAX_BOUND, MAX_BOUND, MAX_BOUND );
	float3 boundmaxv( -MAX_BOUND, -MAX_BOUND, -MAX_BOUND );

	// update emissions
	const float3x4 worldtm = worldTransform();
	for ( int k = 0 ; k < m_emissions.size() ; ++k )
	{
		Emission& emission = m_emissions[k];

		// update emission
		emission.time += dt;

		// create new particles
		if ( emission.stop < 0.f || emission.time < emission.stop )
		{
			emission.newParticles += m_desc->emissionRate.getRandomFloat() * dt;
			emission.newParticles = clamp( emission.newParticles, 0.f, (float)m_desc->emissionMaxParticles );

			for ( ; emission.newParticles >= 1.f ; emission.newParticles -= 1.f )
			{
				// delete particles by KillType if max limit reached
				Particle* newitem = getNew( emission.particles, m_desc->emissionMaxParticles, m_desc->emissionLimitKill );

				// a new emission can be created?
				if ( newitem != 0 )
				{
					Particle& particle = *newitem;

					float life = m_desc->particleLifeTime.getRandomFloat();
					assert( life >= Float::MIN_VALUE );
					float invlife = 1.f / life;

					particle.time = 0.f;
					particle.life = life;
					particle.elasticity = m_desc->particleSpriteElasticity.getRandomFloat();
					particle.rot = m_desc->particleSpriteRotation.getRandomFloat();
					particle.rot = Math::toRadians( particle.rot );

					particle.size.set( m_desc->particleStartSize, m_desc->particleEndSize, invlife );
#ifdef PLATFORM_WIN32
					particle.color.set( m_desc->particleStartColor, m_desc->particleEndColor, invlife );
					particle.alpha.set( m_desc->particleStartAlpha, m_desc->particleEndAlpha, invlife );
#endif
					particle.rotspeed.set( m_desc->particleSpriteStartRotationSpeed, m_desc->particleSpriteEndRotationSpeed, invlife );
					particle.rotspeed.v = Math::toRadians( particle.rotspeed.v );
					particle.rotspeed.dv = Math::toRadians( particle.rotspeed.dv );

					particle.frame = 0;
					particle.velocity = m_parentVelocity + worldtm.rotate( m_desc->wind + m_desc->particleStartVelocity.getRandomFloat3() );
					particle.position = worldtm.transform( m_desc->particleStartPosition.getRandomFloat3() + emission.position );

					particle.userRot = m_userRot;
				}
			}
		}

		// update particles
		for ( int i = 0 ; i < emission.particles.size() ; ++i )
		{
			Particle& particle = emission.particles[i];

			// update texture animation
			if ( fmodf(particle.time,frametime)+dt >= frametime )
			{
				switch ( m_desc->textureAnimation )
				{
				case ANIM_LOOP:
					particle.frame = (particle.frame+1) % m_desc->textureFrames;
					break;
				case ANIM_LIFE:
					particle.frame = (int)lerp( 0.f, textureframesf, particle.time/particle.life );
					break;
				case ANIM_RANDOM:
					particle.frame = rand() % textureframes;
					break;
				case ANIM_COUNT:
					assert( false );
					break;
				}
			}

			// integrate values
			particle.time += dt;
			particle.rot += particle.rotspeed.v * dt;
			particle.size.integrate( dt );
#ifdef PLATFORM_WIN32
			particle.color.integrate( dt );
			particle.alpha.integrate( dt );
#endif
			particle.rotspeed.integrate( dt );

			// integrate forces
			particle.velocity += sharedforceintegral;

			// integrate velocity
			particle.position += particle.velocity * dt;

			// update bound volume
			float psize = particle.size.v;
			boundmaxv.x = Math::max( particle.position.x+psize, boundmaxv.x );
			boundmaxv.y = Math::max( particle.position.y+psize, boundmaxv.y );
			boundmaxv.z = Math::max( particle.position.z+psize, boundmaxv.z );
			boundminv.x = Math::min( particle.position.x-psize, boundminv.x );
			boundminv.y = Math::min( particle.position.y-psize, boundminv.y );
			boundminv.z = Math::min( particle.position.z-psize, boundminv.z );
		}
	}

	// finalize bounding volume computation
	setBoundBoxWorld( boundminv, boundmaxv );
}

void ParticleSystem::render( Context* context, Camera* camera, int priority )
{
	if ( priority == m_desc->shader->priority() && m_emissions.size() > 0 )
	{
		// mark effect rendered
		m_timeSinceRender = 0.f;

		switch ( context->platform() )
		{
#ifdef PLATFORM_WIN32
		case Context::PLATFORM_DX:
			renderDX( context, camera );
			break;
#endif

		case Context::PLATFORM_N3D:
			renderN3D( context, camera );
			break;

		default:
			Debug::printf( "Particle systems used but ParticleSystem::render() not implemented on this platform!\n" );
			break;
		}
	}
}

void ParticleSystem::renderN3D( Context* context, Camera* camera )
{
	// get temporary dynamic primitive from rendering context
	const int maxparticles = m_desc->systemMaxEmissions * m_desc->emissionMaxParticles;
	const int maxvertices = maxparticles * 4;
	const int maxfaces = maxparticles * 2;
	const int maxindices = maxfaces * 3;
	VertexFormat vf;
	vf.addPosition(VertexFormat::DF_V3_16).addTextureCoordinate(VertexFormat::DF_V2_16);
	//Primitive* prim = context->getDynamicPrimitive( Primitive::PRIM_TRI, vf, maxvertices, 0 );
	if ( !m_prim ) 
	{
		m_prim = context->createPrimitive( Primitive::PRIM_TRI, vf, maxvertices, maxindices );
		m_prim->setVertexTextureCoordinateScaleBias( float4(1.f/4096.f,0,0,0) );
	}
	Primitive* prim = m_prim;

	// transforms
	const float3x4& viewtm = camera->cachedViewTransform();
	const float3x4& cameratm = camera->cachedWorldTransform();
	const float frontclip = camera->front() + 1e-5f;
	const float frameuv = m_desc->textureFrameUV;

	// particle up direction is camera or world?
	float worldangle = 0.f;
	if ( m_desc->textureView == VIEW_WORLDUP )
	{
		float3 camerax( cameratm(0,0), cameratm(1,0), cameratm(2,0) );
		float3 cameray( cameratm(0,1), cameratm(1,1), cameratm(2,1) );
		float3 cameraz( cameratm(0,2), cameratm(1,2), cameratm(2,2) );
		float3 worldy( 0,1,0 );
		worldy = normalize0( worldy - cameraz*dot(worldy,cameraz) );
		if ( worldy.lengthSquared() <= Float::MIN_VALUE )
			worldy = cameray;
		float cosangle = dot(worldy,cameray);
		cosangle = clamp( cosangle, -1.f, 1.f );
		worldangle = Math::acos( cosangle );
		if ( dot(worldy,camerax) < 0.f )
			worldangle = -worldangle;
	}

	// set vertex data
	{
		Primitive::Lock lk( prim, Primitive::LOCK_WRITE );

		// get data pointers
		assert( VertexFormat::DF_V3_16 == prim->vertexFormat().getDataFormat(VertexFormat::DT_POSITION) );
		assert( VertexFormat::DF_V2_16 == prim->vertexFormat().getDataFormat(VertexFormat::DT_TEX0) );

		int16_t* vpos;
		int vpospitch;
		prim->getVertexDataPtr( VertexFormat::DT_POSITION, reinterpret_cast<uint8_t**>(&vpos), &vpospitch );
		assert( (vpospitch&1) == 0 );
		vpospitch >>= 1;

		int16_t* vtexcoord;
		int vtexcoordpitch;
		prim->getVertexDataPtr( VertexFormat::DT_TEX0, reinterpret_cast<uint8_t**>(&vtexcoord), &vtexcoordpitch );
		assert( (vtexcoordpitch&1) == 0 );
		vtexcoordpitch >>= 1;
		const float* const uvcache = m_desc->uvcache.begin();

		uint16_t* indexdata;
		int indexsize;
		prim->getIndexDataPtr( &indexdata, &indexsize );
		assert( indexsize == 2 );

		// prepare sprite quads
		int vi = 0;
		int ii = 0;
		for ( int k = 0 ; k < m_emissions.size() ; ++k )
		{
			Emission& emission = m_emissions[k];
			Array<Particle>& particles = emission.particles;

			for ( int i = 0 ; i < particles.size() ; ++i )
			{
				Particle& particle = particles[i];

				// particle position in view space
				float3 viewpos;
				viewtm.transform( particle.position, &viewpos );
				if ( viewpos.z < frontclip )
					continue;

				// particle size in view space
				float particleradius = particle.size.v*.5f;
				float3 dx( particleradius, 0.f, 0.f );
				float3 dy( 0.f, particleradius, 0.f );

				// particle rotation / elasticity are mutually exclusive
				float3 viewp[4];
				if ( particle.elasticity != 0.f )
				{
					assert( !m_desc->particleAlignedToUserNormal );

					float3 vel = viewtm.rotate( particle.velocity );
					float3 deltax = vel * particle.elasticity;
					float3 deltay( -deltax.y, deltax.x, 0.f );
					deltay = normalize0(deltay) * particleradius;
					viewp[0] = viewpos - deltax + deltay;
					viewp[1] = viewpos + deltax + deltay;
					viewp[2] = viewpos + deltax - deltay;
					viewp[3] = viewpos - deltax - deltay;
				}
				else
				{
					float spriteangle = worldangle + particle.rot;
					if ( m_desc->particleAlignedToUserNormal )
					{
						float3x3 rot = particle.userRot;
						if ( spriteangle != 0.f )
							rot = float3x3(rot.getColumn(2), spriteangle) * rot;
						dx = viewtm.rotate( rot.getColumn(0) ) * particleradius;
						dy = viewtm.rotate( rot.getColumn(1) ) * particleradius;
					}
					else
					{
						if ( spriteangle != 0.f )
						{
							float c = Math::cos( spriteangle );
							float s = Math::sin( spriteangle );
							dx = float3( dx.x*c - dx.y*s, dx.x*s + dx.y*c, 0.f );
							dy = float3( dy.x*c - dy.y*s, dy.x*s + dy.y*c, 0.f );
						}
					}

					viewp[0] = viewpos + float3(dy-dx);
					viewp[1] = viewpos + float3(dy+dx);
					viewp[2] = viewpos + float3(dx-dy);
					viewp[3] = viewpos - float3(dx+dy);
				}

				// view space vertex positions (ints)
				const int16_t p0x = (int16_t)viewp[0].x;
				const int16_t p0y = (int16_t)viewp[0].y;
				const int16_t p0z = (int16_t)viewp[0].z;
				const int16_t p1x = (int16_t)viewp[1].x;
				const int16_t p1y = (int16_t)viewp[1].y;
				const int16_t p1z = (int16_t)viewp[1].z;
				const int16_t p2x = (int16_t)viewp[2].x;
				const int16_t p2y = (int16_t)viewp[2].y;
				const int16_t p2z = (int16_t)viewp[2].z;
				const int16_t p3x = (int16_t)viewp[3].x;
				const int16_t p3y = (int16_t)viewp[3].y;
				const int16_t p3z = (int16_t)viewp[3].z;

				// set vertex positions (as two-triangle quad)
				vpos[0] = p0x;
				vpos[1] = p0y;
				vpos[2] = p0z;
				vpos += vpospitch;
				vpos[0] = p1x;
				vpos[1] = p1y;
				vpos[2] = p1z;
				vpos += vpospitch;
				vpos[0] = p2x;
				vpos[1] = p2y;
				vpos[2] = p2z;
				vpos += vpospitch;
				vpos[0] = p3x;
				vpos[1] = p3y;
				vpos[2] = p3z;
				vpos += vpospitch;

				// set texcoords
				const int frame = particle.frame;
				const int frame2 = frame + frame;
				assert( frame2+1 < m_desc->uvcache.size() );
				const int16_t u0 = (int16_t)((uvcache[frame2+0]) * 4096.f);
				const int16_t v0 = (int16_t)((uvcache[frame2+1]) * 4096.f);
				const int16_t u1 = (int16_t)((uvcache[frame2+0] + frameuv) * 4096.f);
				const int16_t v1 = (int16_t)((uvcache[frame2+1] + frameuv) * 4096.f);
				vtexcoord[0] = u0;
				vtexcoord[1] = v0;
				vtexcoord += vtexcoordpitch;
				vtexcoord[0] = u1;
				vtexcoord[1] = v0;
				vtexcoord += vtexcoordpitch;
				vtexcoord[0] = u1;
				vtexcoord[1] = v1;
				vtexcoord += vtexcoordpitch;
				vtexcoord[0] = u0;
				vtexcoord[1] = v1;
				vtexcoord += vtexcoordpitch;

				// set indices
				indexdata[0] = (uint16_t)(vi);
				indexdata[1] = (uint16_t)(vi+2);
				indexdata[2] = (uint16_t)(vi+1);
				indexdata[3] = (uint16_t)(vi);
				indexdata[4] = (uint16_t)(vi+3);
				indexdata[5] = (uint16_t)(vi+2);
				indexdata += 6;

				vi += 4;
				ii += 6;
			}
		}
		prim->setIndexRangeEnd( ii );
		prim->setVertexRangeEnd( vi );
	}

	// render primitive with shader
	if ( prim->vertexRangeEnd() > 0 )
	{
		Shader* shader = m_desc->shader;
		Shader::Begin use( shader );

		float4x4 id4x4( 1.f );
		shader->setMatrix( Shader::PARAM_WORLDTM, id4x4 );
		shader->setMatrix( Shader::PARAM_VIEWTM, id4x4 );

		for ( int k = 0 ; k < use.passes() ; ++k )
		{
			Shader::Pass pass( m_desc->shader, k );
			prim->render();
		}
	}
}

#ifdef PLATFORM_WIN32
void ParticleSystem::renderDX( Context* context, Camera* camera )
{
	// get temporary dynamic primitive from rendering context
	const int maxparticles = m_desc->systemMaxEmissions * m_desc->emissionMaxParticles;
	const int maxvertices = maxparticles * 6;
	VertexFormat vf;
	vf.addTransformedPosition().addDiffuse().addTextureCoordinate(VertexFormat::DF_V2_32);
	//Primitive* prim = context->getDynamicPrimitive( Primitive::PRIM_TRI, vf, maxvertices, 0 );
	if ( !m_prim ) 
		m_prim = context->createPrimitive( Primitive::PRIM_TRI, vf, maxvertices, 0 );
	Primitive* prim = m_prim;

	// viewport half-width, half-height and center coords
	const float viewdx = float( (context->viewport().right() - context->viewport().left()) >> 1 );
	const float viewdy = float( (context->viewport().bottom()- context->viewport().top()) >> 1 );
	const float viewcx = float( (context->viewport().right() + context->viewport().left()) >> 1 );
	const float viewcy = float( (context->viewport().bottom()+ context->viewport().top()) >> 1 );
	const float frameuv = m_desc->textureFrameUV;

	// prepare transforms
	const float4x4& projtm = context->projectionTransform();
	const float3x4& viewtm = camera->cachedViewTransform();
	const float3x4& cameratm = camera->cachedWorldTransform();
	const float frontclip = camera->front() + 1e-5f;

	// particle up direction is camera or world?
	float worldangle = 0.f;
	if ( m_desc->textureView == VIEW_WORLDUP )
	{
		float3 camerax( cameratm(0,0), cameratm(1,0), cameratm(2,0) );
		float3 cameray( cameratm(0,1), cameratm(1,1), cameratm(2,1) );
		float3 cameraz( cameratm(0,2), cameratm(1,2), cameratm(2,2) );
		float3 worldy( 0,1,0 );
		worldy = normalize0( worldy - cameraz*dot(worldy,cameraz) );
		if ( worldy.lengthSquared() <= Float::MIN_VALUE )
			worldy = cameray;
		float cosangle = dot(worldy,cameray);
		cosangle = clamp( cosangle, -1.f, 1.f );
		worldangle = Math::acos( cosangle );
		if ( dot(worldy,camerax) < 0.f )
			worldangle = -worldangle;
	}

	// set vertex data
	{
		Primitive::Lock lk( prim, Primitive::LOCK_WRITE );

		// get data pointers
		assert( VertexFormat::DF_V4_32 == prim->vertexFormat().getDataFormat(VertexFormat::DT_POSITIONT) );
		assert( VertexFormat::DF_V4_8 == prim->vertexFormat().getDataFormat(VertexFormat::DT_DIFFUSE) );
		assert( VertexFormat::DF_V2_32 == prim->vertexFormat().getDataFormat(VertexFormat::DT_TEX0) );

		float* vpos;
		int vpospitch;
		prim->getVertexDataPtr( VertexFormat::DT_POSITIONT, reinterpret_cast<uint8_t**>(&vpos), &vpospitch );
		assert( (vpospitch&3) == 0 );
		vpospitch >>= 2;

		uint32_t* vcolor;
		int vcolorpitch;
		prim->getVertexDataPtr( VertexFormat::DT_DIFFUSE, reinterpret_cast<uint8_t**>(&vcolor), &vcolorpitch );
		assert( (vcolorpitch&3) == 0 );
		vcolorpitch >>= 2;

		float* vtexcoord;
		int vtexcoordpitch;
		prim->getVertexDataPtr( VertexFormat::DT_TEX0, reinterpret_cast<uint8_t**>(&vtexcoord), &vtexcoordpitch );
		assert( (vtexcoordpitch&3) == 0 );
		vtexcoordpitch >>= 2;
		const float* const uvcache = m_desc->uvcache.begin();

		// prepare sprite quads
		int vi = 0;
		for ( int k = 0 ; k < m_emissions.size() ; ++k )
		{
			Emission& emission = m_emissions[k];
			Array<Particle>& particles = emission.particles;

			for ( int i = 0 ; i < particles.size() ; ++i )
			{
				Particle& particle = particles[i];

				// particle position in view space
				float3 viewpos;
				viewtm.transform( particle.position, &viewpos );
				if ( viewpos.z < frontclip )
					continue;

				// particle size in screen space
				float particleradius = particle.size.v*.5f;
				float4 projsize = projtm * float4( particleradius, 0.f, viewpos.z, 1.f );
				if ( projsize.w <= 1e-9f ) // ignore particles behind screen
					continue;
				float rhw = 1.f / projsize.w;
				float screenradius = projsize.x * rhw * viewdx;
				float2 dx( screenradius, 0.f );
				float2 dy( 0, screenradius );

				// particle position in screen space
				float4 projpoint = projtm * float4( viewpos.x, viewpos.y, viewpos.z, 1.f );
				projpoint *= rhw;
				projpoint.x = viewcx + projpoint.x * viewdx;
				projpoint.y = viewcy - projpoint.y * viewdy;
				float2 screenpoint( projpoint.x, projpoint.y );

				// particle rotation / elasticity are mutually exclusive
				float spriteangle = worldangle + particle.rot;
				if ( particle.elasticity == 0.f && spriteangle != 0.f )
				{
					float c = Math::cos( spriteangle );
					float s = Math::sin( spriteangle );
					dx = float2( dx.x*c - dx.y*s, dx.x*s + dx.y*c );
					dy = float2( dy.x*c - dy.y*s, dy.x*s + dy.y*c );
				}
				else if ( particle.elasticity != 0.f )
				{
					// camera space points
					float3 viewp[4];
					float3 vel = viewtm.rotate( particle.velocity );

					float3 deltax = vel * particle.elasticity;
					float3 deltay( -deltax.y, deltax.x, 0.f );
					deltay = normalize0(deltay) * particleradius;
					viewp[0] = viewpos - deltax + deltay;
					viewp[1] = viewpos + deltax + deltay;
					viewp[2] = viewpos + deltax - deltay;
					viewp[3] = viewpos - deltax - deltay;

					// screen space points
					bool skip = false;
					float4 screenp[4];
					for ( int n = 0 ; n < 4 ; ++n )
					{
						if ( viewp[n].z < frontclip )
						{
							skip = true;
							break;
						}

						screenp[n] = projtm * float4(viewp[n].x,viewp[n].y,viewp[n].z,1.f);
						
						float hw = screenp[n].w;
						if ( hw <= Float::MIN_VALUE )
						{
							skip = true;
							break;
						}
						float rhw = 1.f / hw;

						screenp[n] *= rhw;
						screenp[n].x = viewcx + screenp[n].x*viewdx;
						screenp[n].y = viewcy - screenp[n].y*viewdy;
					}
					if ( skip )
						continue;

					// adjust projected point and screen space axes
					dx = float2( screenp[2].x-screenp[1].x, screenp[2].y-screenp[1].y )*.5f;
					dy = float2( screenp[1].x-screenp[0].x, screenp[1].y-screenp[0].y )*.5f;
					screenpoint = float2( (screenp[0].x+screenp[2].x)*.5f, (screenp[0].y+screenp[2].y)*.5f );
				}

				// screen rectangle corner positions
				float2 d0 = dx + dy;
				float2 d1 = dx - dy;
				float2 d2 = dx + dy;
				float2 d3 = dy - dx;
				float2 p0 = screenpoint - d0;
				float2 p1 = screenpoint + d1;
				float2 p2 = screenpoint + d2;
				float2 p3 = screenpoint + d3;

				// set vertex positions (as two-triangle quad)
				vpos[0] = p0.x;
				vpos[1] = p0.y;
				vpos[2] = projpoint.z;
				vpos[3] = rhw;
				vpos += vpospitch;
				vpos[0] = p1.x;
				vpos[1] = p1.y;
				vpos[2] = projpoint.z;
				vpos[3] = rhw;
				vpos += vpospitch;
				vpos[0] = p2.x;
				vpos[1] = p2.y;
				vpos[2] = projpoint.z;
				vpos[3] = rhw;
				vpos += vpospitch;
				// tri 0,2,3
				vpos[0] = p0.x;
				vpos[1] = p0.y;
				vpos[2] = projpoint.z;
				vpos[3] = rhw;
				vpos += vpospitch;
				vpos[0] = p2.x;
				vpos[1] = p2.y;
				vpos[2] = projpoint.z;
				vpos[3] = rhw;
				vpos += vpospitch;
				vpos[0] = p3.x;
				vpos[1] = p3.y;
				vpos[2] = projpoint.z;
				vpos[3] = rhw;
				vpos += vpospitch;

				// set color0
				uint32_t color = 
					uint32_t(clamp( particle.color.v.z*255.f, 0.f, 255.f )) +
					(uint32_t(clamp( particle.color.v.y*255.f, 0.f, 255.f )) << 8) +
					(uint32_t(clamp( particle.color.v.x*255.f, 0.f, 255.f )) << 16) +
					(uint32_t(clamp( particle.alpha.v*255.f, 0.f, 255.f )) << 24);
				*vcolor = color;
				vcolor += vcolorpitch;
				*vcolor = color;
				vcolor += vcolorpitch;
				*vcolor = color;
				vcolor += vcolorpitch;
				*vcolor = color;
				vcolor += vcolorpitch;
				*vcolor = color;
				vcolor += vcolorpitch;
				*vcolor = color;
				vcolor += vcolorpitch;

				// set texcoords
				const int frame = particle.frame;
				const int frame2 = frame + frame;
				assert( frame2+1 < m_desc->uvcache.size() );
				const float u0 = uvcache[frame2+0];
				const float v0 = uvcache[frame2+1];
				const float u1 = u0 + frameuv;
				const float v1 = v0 + frameuv;
				vtexcoord[0] = u0;
				vtexcoord[1] = v0;
				vtexcoord += vtexcoordpitch;
				vtexcoord[0] = u1;
				vtexcoord[1] = v0;
				vtexcoord += vtexcoordpitch;
				vtexcoord[0] = u1;
				vtexcoord[1] = v1;
				vtexcoord += vtexcoordpitch;
				vtexcoord[0] = u0;
				vtexcoord[1] = v0;
				vtexcoord += vtexcoordpitch;
				vtexcoord[0] = u1;
				vtexcoord[1] = v1;
				vtexcoord += vtexcoordpitch;
				vtexcoord[0] = u0;
				vtexcoord[1] = v1;
				vtexcoord += vtexcoordpitch;

				vi += 6;
			}
		}
		prim->setVertexRangeEnd( vi );
	}

	// render primitive with shader
	if ( prim->vertexRangeEnd() > 0 )
	{
		Shader::Begin use( m_desc->shader );
		for ( int k = 0 ; k < use.passes() ; ++k )
		{
			Shader::Pass pass( m_desc->shader, k );
			prim->render();
		}
	}
}
#endif

void ParticleSystem::getShaders( Array<Shader*>& shaders )
{
	shaders.add( m_desc->shader );
}

int ParticleSystem::particles() const
{
	int count = 0;
	const Emission* end = m_emissions.end();
	for ( const Emission* it = m_emissions.begin() ; it != end ; ++it )
		count += it->particles.size();
	return count;
}

void ParticleSystem::restart()
{
	m_time = 0.f;
	m_timeSinceRender = -Float::MAX_VALUE;
	m_newEmissions = 0.f;
	m_systemStopTime = m_desc->systemStopTime.getRandomFloat();
	m_systemLifeTime = m_desc->systemLifeTime.getRandomFloat();
	m_emissions.clear();
}

ParticleSystem::ViewType ParticleSystem::toViewType( const char* sz )
{
	assert( int(sizeof(VIEW_NAMES)/sizeof(VIEW_NAMES[0])) == VIEW_COUNT );
	for ( int i = 0; i < VIEW_COUNT ; ++i )
		if ( !strcmp(VIEW_NAMES[i],sz) )
			return ViewType(i);
	return VIEW_COUNT;
}

ParticleSystem::AnimationType ParticleSystem::toAnimationType( const char* sz )
{
	assert( int(sizeof(ANIM_NAMES)/sizeof(ANIM_NAMES[0])) == ANIM_COUNT );
	for ( int i = 0; i < ANIM_COUNT ; ++i )
		if ( !strcmp(ANIM_NAMES[i],sz) )
			return AnimationType(i);
	return ANIM_COUNT;
}

ParticleSystem::KillType ParticleSystem::toKillType( const char* sz )
{
	assert( int(sizeof(KILL_NAMES)/sizeof(KILL_NAMES[0])) == KILL_COUNT );
	for ( int i = 0; i < KILL_COUNT ; ++i )
		if ( !strcmp(KILL_NAMES[i],sz) )
			return KillType(i);
	return KILL_COUNT;
}

void ParticleSystem::refreshCachedValues()
{
	assert( m_desc != 0 );

	// allocate space for emissions and particles
	m_emissions.resize( m_desc->systemMaxEmissions );
	for ( int i = 0 ; i < m_emissions.size() ; ++i )
		m_emissions[i].particles.resize( m_desc->emissionMaxParticles );
	m_emissions.clear();

	// setup texture uv cache
	const int frames = m_desc->textureFrames;
	const int framesx = frames > 1 ? log2i( frames ) : 1;
	m_desc->textureFrameUV = 1.f/float(framesx);
	m_desc->uvcache.resize( frames*2 );

	int xframe = 0;
	int yframe = 0;
	for ( int i = 0 ; i < m_desc->uvcache.size() ; i += 2 )
	{
		m_desc->uvcache[i+0] = m_desc->textureFrameUV * (float)xframe;
		m_desc->uvcache[i+1] = m_desc->textureFrameUV * (float)yframe;

		if ( ++xframe >= framesx )
		{
			xframe = 0;
			++yframe;
		}
	}
}

int ParticleSystem::log2i( int x )
{
	assert( x > 0 );

	int logx = 0;
	for ( ; x > 1 ; x >>= 1 )
		++logx;
	return logx;
}

void ParticleSystem::reset()
{
	refreshCachedValues();
	restart();
}

template <class T> void ParticleSystem::killOld( Array<T>& array )
{
	for ( int i = 0 ; i < array.size() ; ++i )
	{
		if ( array[i].life > 0.f && array[i].time >= array[i].life )
		{
			array[i] = array.last();
			array.resize( array.size()-1 );
			--i;
		}
	}
}

template <class T> T* ParticleSystem::getNew( Array<T>& array, int limit, KillType killtype )
{
	if ( array.size() < limit )
	{
		array.resize( array.size()+1 );
		return &array.last();
	}

	switch ( killtype )
	{
	case KILL_NONE:
		return 0;

	case KILL_OLDEST:{
		float oldage = 0.f;
		int oldix = -1;
		for ( int i = 0 ; i < array.size() ; ++i )
			if ( array[i].time > oldage )
			{
				oldage = array[i].time;
				oldix = i;
			}
		if ( oldix >= 0 )
			return &array[oldix];
		return 0;}

	case KILL_RANDOM:
		return &array[ rand() % array.size() ];

	default:
		return 0;
	}
}

void ParticleSystem::computeBound()
{
	// nothing to compute, ParticleSystem::update keeps up-to-date!
}

void ParticleSystem::setDelay( float time )
{
	assert( time >= 0.f );
	m_delay = time;
}

void ParticleSystem::setParentVelocity( const float3& vel )
{
	m_parentVelocity = vel;
}

void ParticleSystem::setUserNormal( const NS(math,float3)& normal )
{
	m_userRot.generateOrthonormalBasisFromZ( normal );
}


END_NAMESPACE() // hgr
#endif // HGR_NOPARTICLES
