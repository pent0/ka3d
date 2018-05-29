#include "DemoApp.h"
#include "GameLevel.h"
#include "GameCamera.h"
#include "GameCharacter.h"
#include "version.h"
#include "resource.h"
#include <io/all.h>
#include <gr/Rect.h>
#include <lua/LuaState.h>
#include <hgr/DefaultResourceManager.h>
#include <lang/Math.h>
#include <lang/Profile.h>
#include <config.h>


USING_NAMESPACE(gr)
USING_NAMESPACE(io)
USING_NAMESPACE(lua)
USING_NAMESPACE(hgr)
USING_NAMESPACE(lang)
USING_NAMESPACE(math)
USING_NAMESPACE(framework)


DemoApp::DemoApp( NS(framework,OSInterface)* os, Context* context ) :
	App( os ),
	m_time( 0.f ),
	m_timeScale( 1.f ),
	m_fps( 0.f ),
	m_frameCount( 0 ),
	m_context( context ),
	m_debugMode( 0 ),
	m_recordAnim( false )
{
	// framework
	P(ResourceManager) res = DefaultResourceManager::get( context );
	m_lua = new LuaState;
	m_console = new Console( context, res->getTexture(path("fonts/comic_sans_ms_20x23.dds")), 20, 23, -2 );

	// game objects
	m_level = new GameLevel( context, m_lua, res );

	m_player = new GameCharacter( m_lua, m_level, "data/zax.lua" );
	m_player->setTransform( m_level->getStartpoint("PLAYER_START_POINT") );
	m_level->addObject( m_player );

	m_lasergun = new GameWeapon( m_lua, m_level );
	m_player->addWeapon( m_lasergun );
	m_level->addObject( m_lasergun );

	m_camera = new GameCamera( m_lua, m_player, m_level );
	m_level->addObject( m_camera );

	// HUD
	m_crosshair = res->getTexture( "data/crosshairs/default.tga" );
}

DemoApp::~DemoApp()
{
}

void DemoApp::update( float dt, Context* context )
{
	Profile::beginFrame();

	// update time
	float fps = 1.f / dt;
	float fixdt = 1.f / 100.f; // run sim in 100Hz
	dt *= m_timeScale;
	fixdt *= Math::min(1.f,m_timeScale);
	m_time += dt;

	controlPlayer();
	simulate( fixdt );
	render( context, fps );
	swapBackBuffer( context );

	Profile::endFrame();
}

void DemoApp::keyDown( KeyType key )
{
	if ( KEY_ESCAPE == key )
	{
		quit();
	}
	else if ( KEY_F4 == key )
	{
		toggleTimeScale();
	}
	else if ( KEY_F5 == key && m_player != 0 )
	{
		m_player->setTransform( m_level->getStartpoint("PLAYER_START_POINT") );
		if ( !m_player->hasFireArm() )
			m_player->addWeapon( m_lasergun );
	}
	else if ( KEY_F8 == key )
	{
		cycleDebugMode();
	}
	else if ( KEY_LBUTTON == key )
	{
		m_player->setControlState( GameCharacter::CONTROL_FIRE_PRIMARY, 1.f );
	}
	else if ( KEY_BACKSPACE == key )
	{
		m_player->dropWeapon();
	}
}

void DemoApp::keyUp( KeyType key )
{
	if ( KEY_LBUTTON == key )
		m_player->setControlState( GameCharacter::CONTROL_FIRE_PRIMARY, 0.f );
}

void DemoApp::toggleTimeScale()
{
	if ( m_timeScale == 1.f )
		m_timeScale = isKeyDown(KEY_SHIFT) ? 3.f : .1f;
	else
		m_timeScale = 1.f;
}

void DemoApp::cycleDebugMode()
{
	if ( isKeyDown(KEY_SHIFT) )
		++m_debugMode;
	else
		--m_debugMode;
}

void DemoApp::activate( bool /*active*/ )
{
	if ( m_player != 0 )
		m_player->resetControlState();
}

void DemoApp::mouseMove( int dx, int dy )
{
	if ( dx < 0 )
		m_player->setControlState( GameCharacter::CONTROL_TURN_LEFT, (float)-dx );
	else if ( dx > 0 )
		m_player->setControlState( GameCharacter::CONTROL_TURN_RIGHT, (float)dx );

	if ( dy < 0 )
		m_player->setControlState( GameCharacter::CONTROL_TURN_UP, (float)-dy );
	else if ( dy > 0 )
		m_player->setControlState( GameCharacter::CONTROL_TURN_DOWN, (float)dy );
}

void DemoApp::controlPlayer()
{
	m_player->setControlState( GameCharacter::CONTROL_WALK_FORWARD, isKeyDown(KEY_UP) || isKeyDown(KEY_W) ? 1.f : 0.f );
	m_player->setControlState( GameCharacter::CONTROL_STEP_LEFT, isKeyDown(KEY_A) ? 1.f : 0.f );
	m_player->setControlState( GameCharacter::CONTROL_STEP_RIGHT, isKeyDown(KEY_D) ? 1.f : 0.f );
	m_player->setControlState( GameCharacter::CONTROL_JUMP, isKeyDown(KEY_SPACE) ? 1.f : 0.f );
}

void DemoApp::simulate( float dt )
{
	PROFILE(simulation);

	// update simulation at fixed interval
	if ( m_time > 1.f )
		m_time = 1.f;
	for ( ; m_time > dt ; m_time -= dt )
		m_level->update( dt );
}

void DemoApp::render( Context* context, float fps )
{
	PROFILE(render);

	Context::RenderScene rs( context );

	// game view
	m_camera->render( context );

	// crosshair
	if ( m_player->isAimingOrShooting() )
	{
		m_console->drawImage( (context->width()-m_crosshair->width())/2,
			(context->height()-m_crosshair->height())/2, m_crosshair );
	}

	// debug info
	if ( m_debugMode != 0 )
		drawDebugInfo( context, fps );

	// flush & reset
	m_console->render( context );
	m_console->clear();
	context->statistics.reset();
	m_level->lines()->removeLines();
}

void DemoApp::drawDebugInfo( Context* context, float fps )
{
	if ( m_debugMode == 1 )
	{
		m_console->printf( "RENDERING\n" );

		// average fps for n frames
		m_fpsBuffer[m_frameCount++ % FPS_COUNT] = fps;
		if ( (m_frameCount%5) == 0 )
		{
			// update rendered fps count only every 5 frames for better readability
			int n = m_frameCount > FPS_COUNT ? FPS_COUNT : m_frameCount;
			m_fps = m_fpsBuffer[0];
			for ( int i = 1 ; i < n ; ++i )
				m_fps += m_fpsBuffer[i];
			m_fps /= (float)n;
		}
		m_console->printf( "fps %5.1f, tri %6d, pri %6d\n", m_fps, context->statistics.renderedTriangles, context->statistics.renderedPrimitives );
	}

	if ( m_debugMode == 2 )
	{
		m_console->printf( "PHYSICS\n" );

		m_level->drawSimulationObjects();

		m_console->printf( "active bodies: %d\n", m_level->enabledBodies() );
	}

	if ( m_debugMode == 3 )
	{
		m_console->printf( "PLAYER\n" );

		// show player info
		m_console->printf( "pos %5.02f %5.02f %5.02f (ground=%d)\n", m_player->position().x, m_player->position().y, m_player->position().z, m_player->isOnGround() ? 1 : 0 );
		m_console->printf( "vel %5.02f %5.02f %5.02f\n", m_player->linearVel().x, m_player->linearVel().y, m_player->linearVel().z );
		m_console->printf( "dir %5.02f %5.02f %5.02f\n", m_player->rotation().getColumn(2).x, m_player->rotation().getColumn(2).y, m_player->rotation().getColumn(2).z );
		m_console->printf( "amb %5.02f %5.02f %5.02f\n", m_player->ambient().x, m_player->ambient().y, m_player->ambient().z );
		m_console->printf( "dir %g pitch %g\n", m_player->direction(), m_player->pitch() );
		m_console->printf( "body:%s (%5.2f), upper:%s (%5.2f)\n", m_player->bodyStateString(), m_player->bodyStateTime(), m_player->upperBodyStateString(), m_player->upperBodyStateTime() );
	}

	if ( m_debugMode == 4 )
	{
		m_console->printf( "PROFILER\n" );

		// render info about profiled blocks
		for ( int i = 0 ; i < Profile::blocks() ; ++i )
			m_console->printf( "%-16s = %5.1f %% (x%5d)\n", Profile::getName(i), Profile::getPercent(i), Profile::getCount(i) );
		Profile::reset();
	}

	// make sure debug mode is in range
	if ( m_debugMode < 0 )
		m_debugMode += 100;
	m_debugMode %= 5;
}

void DemoApp::swapBackBuffer( Context* context )
{
	PROFILE(swapBackBuffer);

	if ( m_recordAnim )
		context->capture( "C:/Documents and Settings/jani/My Documents/tmp/zax%04d.jpg" );
	context->present();
}

void DemoApp::serialize( io::DataInputStream* in, io::DataOutputStream* out )
{
	assert( m_level );
	m_player->serialize( in, out );
	m_camera->serialize( in, out );
}

void framework::configure( App::Configuration& config )
{
	config.name = "Zax Demo " ZAXDEMO_VERSION_NUMBER " Zeta";
	config.captureMouse = true;

	// load config
	LuaState lua;
	LuaTable tab( &lua );
	FileInputStream in( "data/config.lua" );
	tab.read( &in );

#ifdef _DEBUG
	config.fullscreen = false;
#else
	config.fullscreen = tab.getBoolean( "fullscreen" );
#endif
	config.width = (int)tab.getNumber( "width" );
	config.height = (int)tab.getNumber( "height" );
	config.bits = (int)tab.getNumber( "bits" );
	config.iconResourceID = IDI_ICON1;
	config.debugMemoryAllocs = false;
}

App* framework::init( NS(framework,OSInterface)* os, NS(gr,Context)* context )
{
	return new DemoApp( os, context );
}
