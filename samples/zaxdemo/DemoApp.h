#ifndef _DEMOAPP_H
#define _DEMOAPP_H


#include <framework/App.h>
#include <gr/Context.h>
#include <hgr/Console.h>


class GameLevel;
class GameWeapon;
class GameCamera;
class GameCharacter;

namespace lua {
	class LuaState;}


class DemoApp : 
	public NS(framework,App)
{
public:
	DemoApp( NS(framework,OSInterface)* os, NS(gr,Context)* context );

	~DemoApp();

	void	keyDown( KeyType key );

	void	keyUp( KeyType key );

	void	mouseMove( int dx, int dy );

	void	activate( bool active );

	void	update( float dt, NS(gr,Context)* context );

	void	serialize( io::DataInputStream* in, io::DataOutputStream* out );

	hgr::Console*		console()	{return m_console;}

	static DemoApp*		get()		{return static_cast<DemoApp*>( App::get() );}

private:
	enum { FPS_COUNT=5 };

	float				m_time;
	float				m_timeScale;
	float				m_fps;
	float				m_fpsBuffer[FPS_COUNT];
	int					m_frameCount;
	P(NS(gr,Context))		m_context;
	P(hgr::Console)		m_console;
	P(lua::LuaState)	m_lua;
	int					m_debugMode;
	bool				m_recordAnim;

	// game objects
	P(GameLevel)		m_level;
	P(GameCharacter)	m_player;
	P(GameCamera)		m_camera;
	P(GameWeapon)		m_lasergun;

	// HUD
	P(gr::Texture)		m_crosshair;

	void	controlPlayer();
	void	simulate( float fixdt );
	void	render( NS(gr,Context)* context, float fps );
	void	swapBackBuffer( NS(gr,Context)* context );

	void	toggleTimeScale();
	void	cycleDebugMode();
	void	cycleRenderingPasses();
	void	drawDebugInfo( NS(gr,Context)* context, float fps );

	DemoApp( const DemoApp& );
	DemoApp& operator=( const DemoApp& );
};

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.


#endif // _DEMOAPP_H
