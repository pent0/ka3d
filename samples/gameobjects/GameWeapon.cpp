#include "GameWeapon.h"
#include "GameLevel.h"
#include "GameCamera.h"
#include <lang/Math.h>
#include <lang/Debug.h>
#include <lang/Exception.h>
#include <math/float.h>
#include <math/toString.h>
#include <math/InterpolationUtil.h>
#include <config.h>


USING_NAMESPACE(gr)
USING_NAMESPACE(hgr)
USING_NAMESPACE(lua)
USING_NAMESPACE(lang)
USING_NAMESPACE(math)


GameWeapon::GameWeapon( LuaState* luastate, GameLevel* level ) :
	GameObject( luastate, level ),
	m_target( 0, 0, 0 ),
	m_bodytm( 1.f ),
	m_owner( 0 ),
	m_muzzleflash( 0 ),
	m_muzzleflashVisible( 0 )
{
	setMesh( "data/weapons/lasergun/lasergun.hgr" );
	setParentBone( "Dummy_laser_barrel01" );
	setPrimaryEmissionBone( "Dummy_primary_flame" );
	setSecondaryEmissionBone( "Dummy_secondary_flame" );
	setPrimaryAmmo( "lasershot" );
	setAutoDisable( true );
}

GameWeapon::~GameWeapon()
{
}

void GameWeapon::setMesh( String filename )
{
	// set meshes
	GameObject::setMesh( filename );
	m_mesh = dynamic_cast<Mesh*>( m_root->getNodeByName("weapon_laser_gun") );
	if ( !m_mesh )
		throwError( Exception( Format("{0} needs to have mesh 'weapon_laser_gun'", filename) ) );
	m_muzzleflash = dynamic_cast<Mesh*>( m_root->getNodeByName("muzzleflash") );
	if ( !m_muzzleflash )
		throwError( Exception( Format("{0} needs to have mesh 'muzzleflash'", filename) ) );
	m_muzzleflash->setEnabled( false );

	// adjust mesh rotation by conventions used in 3dsmax
	float3x3 weaponrot = float3x3( float3(0,1,0), Math::PI*.5f );
	weaponrot = weaponrot * float3x3( float3(0,0,1), -Math::PI*.5f );
	m_mesh->setRotation( weaponrot * m_mesh->rotation() );

	// setup transformations between visual and simulation space
	float3x4 bodyworldtm, bodyvertextm;
	m_mesh->getBodyTransform( &m_bodytm, &bodyworldtm, &bodyvertextm );

	// setup character rigid body and collision object
	float3 size = bodyvertextm.rotate( m_mesh->boundBoxMax() - m_mesh->boundBoxMin() );
	float mass = 10.f;
	setRigidBodyFromBox( size, mass, TYPE_WEAPON, TYPE_LEVEL );
}

void GameWeapon::setOwner( GameObject* obj )
{
	m_owner = obj;

	if ( m_owner != 0 )
		disable();
	else
		enable();
}

void GameWeapon::setParentBone( String bonename )
{
	m_parentBone = bonename;
}

void GameWeapon::setPrimaryEmissionBone( String nodename )
{
	m_emission1 = nodename;
}

void GameWeapon::setSecondaryEmissionBone( String nodename )
{
	m_emission2 = nodename;
}

void GameWeapon::setPrimaryAmmo( String ammoname )
{
	m_ammo1 = ammoname;
}

void GameWeapon::setSecondaryAmmo( String ammoname )
{
	m_ammo2 = ammoname;
}

void GameWeapon::setLight( Light* lt )
{
	assert( lt );
	m_mesh->removeLights();
	m_mesh->addLight( lt );
}

void GameWeapon::update( float dt )
{
	// update muzzle flash
	bool muzzleflashvisible = m_muzzleflashVisible > 0.f;
	m_muzzleflash->setEnabled( muzzleflashvisible );
	if ( muzzleflashvisible )
		m_muzzleflashVisible -= dt;
}

void GameWeapon::getObjectsToRender( GameCamera* camera )
{
	assert( m_root != 0 );

	m_root->setTransform( transform() * m_bodytm.inverse() );
	camera->addObjectToRender( m_root );
}

void GameWeapon::setAim( const float3x4& handtm, const float3& target, float aimweight )
{
	m_target = target;

	float3x4 tm;
	tm.setTranslation( handtm.translation() );
	tm.setRotation( handtm.rotation().orthonormalize() );

	/*m_level->lines()->addLine( tm.translation(), tm.translation()+tm.getColumn(0), float4(1,0,0,1) );
	m_level->lines()->addLine( tm.translation(), tm.translation()+tm.getColumn(1), float4(0,1,0,1) );
	m_level->lines()->addLine( tm.translation(), tm.translation()+tm.getColumn(2), float4(0,0,1,1) );*/

	if ( aimweight > 0.f )
	{
		//Node* emission = m_root->getNodeByName( m_emission1 );
		//m_level->lines()->addLine( emission->worldTransform().translation(), target, float4(1,1,1,0), float4(1,1,1,1) );

		float3 aimz = normalize( target - tm.translation() );
		float3 weaponz = -tm.getColumn(0);
		float cosa = clamp( dot(weaponz,aimz), 0.f, 1.f );
		float angle = Math::acos( cosa ) * aimweight;
		float absangle = Math::abs( angle );
		if ( absangle > 0.001f && absangle < Math::toRadians(45.f) )
		{
			float3x3 rot( normalize(cross(weaponz,aimz)), angle );
			tm.setRotation( rot * tm.rotation() );
		}
	}

	setLinearVel( float3(0,0,0) );
	setAngularVel( float3(0,0,0) );
	setTransform( tm * m_bodytm );
}

void GameWeapon::firePrimary()
{
	Node* emission = m_root->getNodeByName( m_emission1 );

	float3 pos = emission->worldTransform().translation();
	float3 dir = normalize( m_target - pos );
	float3x3 rot;
	rot.generateOrthonormalBasisFromZ( dir );

	float3x4 tm;
	tm.setTranslation( pos );
	tm.setRotation( rot );

	m_level->addProjectile( m_ammo1, tm, this );

	m_muzzleflashVisible = 0.1f;
}
