#include "GameCamera.h"
#include "GameLevel.h"
#include "GameCharacter.h"
#include <gr/Context.h>
#include <hgr/GlowPipe.h>
#include <hgr/PipeSetup.h>
#include <hgr/DefaultPipe.h>
#include <hgr/Camera.h>
#include <lang/Math.h>
#include <lang/Profile.h>
#include <config.h>


USING_NAMESPACE(gr)
USING_NAMESPACE(io)
USING_NAMESPACE(lua)
USING_NAMESPACE(hgr)
USING_NAMESPACE(lang)
USING_NAMESPACE(math)


GameCamera::GameCamera( LuaState* lua, 
	GameCharacter* target, GameLevel* level ) :
	GameObject( lua, level ),
	m_target( target ),
	m_time( 0 ),
	m_dt( 0 ),
	m_passes( -1 )
{
	// setup aim helper
	m_aim = new GameRay( lua, level, TYPE_LEVEL+TYPE_CHARACTER+TYPE_DYNAMIC );
	m_aim->setIgnoreObject( target );
	level->addObject( m_aim );

	// setup rendering camera
	m_camera = new Camera;
	m_camera->setFront( 0.1f );
	m_camera->setBack( 1000.f );

	// setup rendering pipes
	m_pipeSetup = new PipeSetup( context() );
	m_defaultPipe = new DefaultPipe( m_pipeSetup, "Default", -2 );
	m_glowPipe = new GlowPipe( m_pipeSetup );
	m_spritePipe = new DefaultPipe( m_pipeSetup, "Default", -3, -3 );

	// setup physics and collision objects
	setRigidBodyFromSphere( 0.5f, 1.f, TYPE_CAMERA, TYPE_LEVEL );
	disableGravity();
	setAutoDisable( false );
}

GameCamera::~GameCamera()
{
}

void GameCamera::update( float dt )
{
	assert( m_level );
	assert( m_target );

	m_dt += dt;

	// setup aim update
	// (note: this must precede following camera position
	// re-computation, since it resets camera position explicitly)
	m_aim->setRay( position(), rotation().getColumn(2), 100.f );

	// prepare re-compute camera position
	float3x4 meshtm( m_target->transform() );
	meshtm.setRotation( meshtm.rotation() * float3x3(float3(1,0,0),-m_target->pitch()) );
	float3 cameradistv( 0.f, 1.f, 3.f );
	setPosition( meshtm.translation() + float3(0,1.8f,0) );
	setLinearVel( meshtm.rotate(cameradistv) * (1.f/dt) );
}

void GameCamera::render( Context* context )
{
	assert( m_level );
	assert( m_target );

	m_time += m_dt;

	// update camera rotation
	m_camera->setPosition( position() );
	m_camera->lookAt( m_target->position() + float3(0,2.3f,0) );
	setTransform( m_camera->transform() );

	// collect object node hierarchies to render
	Scene* scene = m_level->getSceneToRender( this, m_time, m_dt );
	m_nodes.clear();
	for ( int i = 0 ; i < m_level->objects() ; ++i )
		m_level->getObject(i)->getObjectsToRender( this );
	for ( int i = 0 ; i < m_nodes.size() ; ++i )
		m_nodes[i]->linkTo( scene );

	// render scene
	bool defaultenabled = (m_passes&(1<<0)) != 0;
	bool spriteenabled	= (m_passes&(1<<1)) != 0;
	bool glowenabled	= (m_passes&(1<<2)) != 0;
	{PROFILE(pipeSetup);	m_pipeSetup->setup( m_camera );}
	{PROFILE(defaultPipe);	if (defaultenabled)	m_defaultPipe->render( 0, context, scene, m_camera );}
	{PROFILE(spritePipe);	if (spriteenabled)	m_spritePipe->render( 0, context, scene, m_camera );}
	{PROFILE(glowPipe);		if (glowenabled)	m_glowPipe->render( 0, context, scene, m_camera );}

	// remove rendered objects
	for ( int i = 0 ; i < m_nodes.size() ; ++i )
		m_nodes[i]->unlink();

	// reset dt to be collected in next updates
	m_dt = 0.f;
}

void GameCamera::getObjectsToRender( GameCamera* camera )
{
	camera->addObjectToRender( m_camera );
}

void GameCamera::togglePass( int index )
{
	int f = 1<<index;
	if ( m_passes & f ) 
		m_passes &= ~f; 
	else 
		m_passes |= f;
}

const float3& GameCamera::aimPoint() const
{
	return m_aim->endPoint();
}

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
