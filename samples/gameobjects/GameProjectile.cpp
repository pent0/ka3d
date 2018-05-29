#include "GameProjectile.h"
#include "GameLevel.h"
#include "GameCamera.h"
#include <hgr/ResourceManager.h>
#include <lang/Math.h>
#include <lang/Debug.h>
#include <lang/Exception.h>
#include <math/toString.h>
#include <math/InterpolationUtil.h>
#include <config.h>


USING_NAMESPACE(gr)
USING_NAMESPACE(hgr)
USING_NAMESPACE(lua)
USING_NAMESPACE(lang)
USING_NAMESPACE(math)


GameProjectile::GameProjectile( LuaState* luastate, GameLevel* level ) :
	GameObject( luastate, level ),
	m_time( 0 ),
	m_timeToLive( 0 ),
	m_source( 0 ),
	m_speed( 1.f )
{
	float mass = 0.5f;

	setRigidBodyFromRay( TYPE_PROJECTILE, TYPE_LEVEL+TYPE_CHARACTER+TYPE_DYNAMIC, mass );

	setSpeed( 100.f );
	setDamage( 10.f );
	setParticle( "weapon_laser_gun_beam.prs" );
	setTimeToLive( 1.f );
	setAutoDisable( true );
	disableGravity();
}

GameProjectile::~GameProjectile()
{
}

void GameProjectile::setTimeToLive( float time )
{
	m_timeToLive = time;
}

void GameProjectile::setSource( GameObject* obj )
{
	m_source = obj;
}

void GameProjectile::setSpeed( float speed )
{
	m_speed = speed;

	dGeomRaySetLength( geom(), speed * 1e-2f );
}

void GameProjectile::setDamage( float damage )
{
	m_damage = damage;
}

void GameProjectile::setParticle( String filename )
{
	m_particle = resourceManager()->getParticleSystem( filename );
}

void GameProjectile::setLight( Light* lt )
{
	if ( m_mesh != 0 )
	{
		m_mesh->removeLights();
		m_mesh->addLight( lt );
	}
}

void GameProjectile::update( float dt )
{
	//m_level->lines()->addLine( position(), position()+rotation().getColumn(0), float4(1,0,0,1) );
	//m_level->lines()->addLine( position(), position()+rotation().getColumn(1), float4(0,1,0,1) );
	//m_level->lines()->addLine( position(), position()+rotation().getColumn(2), float4(0,0,1,1) );

	m_particle->setTransform( transform() );
	m_particle->setParentVelocity( linearVel() );
	m_particle->update( dt );

	m_time += dt;

	if ( m_time > m_timeToLive )
		destroy();
}

void GameProjectile::reset()
{
	m_time = 0.f;

	setTransform( float3x4(1.f) );
	setSource( 0 );
	setLinearVel( float3(0,0,0) );
	setAngularVel( float3(0,0,0) );
}

void GameProjectile::getObjectsToRender( GameCamera* camera )
{
	if ( m_root != 0 )
	{
		m_root->setTransform( transform() );
		camera->addObjectToRender( m_root );
	}

	if ( m_particle != 0 )
		camera->addObjectToRender( m_particle );
}

int	GameProjectile::collided( GameObject* obj, dContact* contacts, int numc )
{
	return numc;
}

float GameProjectile::impulse() const		
{
	return mass()*m_speed;
}
