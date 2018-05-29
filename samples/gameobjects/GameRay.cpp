#include "GameRay.h"
#include "GameLevel.h"
#include <math/toString.h>
#include <lang/Debug.h>
#include <config.h>


USING_NAMESPACE(gr)
USING_NAMESPACE(hgr)
USING_NAMESPACE(lua)
USING_NAMESPACE(lang)
USING_NAMESPACE(math)


GameRay::GameRay( LuaState* luastate, GameLevel* level, int collides ) :
	GameObject( luastate, level ),
	m_endPoint( 0, 0, 0 ),
	m_ignore( 0 )
{
	setRigidBodyFromRay( TYPE_NONE, collides, 1.f );
	setAutoDisable( false );
}

GameRay::~GameRay()
{
}

void GameRay::setRay( const float3& origin, const float3& dir, float maxlen )
{
	float3x4 tm;
	tm.setTranslation( origin );
	float3x3 rot;
	rot.generateOrthonormalBasisFromZ( dir );
	tm.setRotation( rot );
	setTransform( tm );
	//dGeomRaySet( geom(), origin.x, origin.y, origin.z, dir.x, dir.y, dir.z );

	dGeomRaySetLength( geom(), maxlen );
	enable();

	// reset ray end point
	m_endPoint = origin + dir * maxlen;

	setAngularVel( float3(0,0,0) );
	setLinearVel( float3(0,0,0) );
}

void GameRay::setIgnoreObject( GameObject* ignore )
{
	m_ignore = ignore;
}

int	GameRay::collided( GameObject* obj, dContact* contacts, int /*numc*/ )
{
	if ( obj == m_ignore )
		return 0;

	float3 pos( contacts[0].geom.pos[0], contacts[0].geom.pos[1], contacts[0].geom.pos[2] );
	m_endPoint = pos;
	return 0;
}
