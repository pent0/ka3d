#ifndef _ODE_ODEWORLD_H
#define _ODE_ODEWORLD_H


#include <ode/ODECollisionInterface.h>
#include <lang/Object.h>


BEGIN_NAMESPACE(math) 
	class float3;END_NAMESPACE()


BEGIN_NAMESPACE(ode) 


/**
 * Simple wrapper which combines ODE simulation world and collision space.
 *
 * @ingroup ode
 */
class ODEWorld :
	public NS(lang,Object)
{
public:
	/**
	 * Creates simulation world, collision space and contact group,
	 * using default settings (gravity 9.8 m/s2, etc.)
	 * @param groundlevel Absolute minimum Y-coordinate which limits free falls.
	 */
	explicit ODEWorld( float groundlevel=-100.f );

	~ODEWorld();

	/**
	 * Sets gravity in simulation world. Default is -9.8 along Y-axis.
	 */
	void			setGravity( const NS(math,float3)& f );

	/** 
	 * Simulates time step. 
	 * Calls user specified collision checker to find out contacts between two objects.
	 * @param dt Time step to simulate.
	 * @param checker Collision checker to be used to find contacts. Pass 0 to use default.
	 */
	void			step( float dt, ODECollisionInterface* checker );

	/** Returns simulation world. */
	dWorldID		world() const;

	/** Returns collision space. */
	dSpaceID		space() const;

	/** Returns contact group using in collision detection. */
	dJointGroupID	contacts() const;

	/**
	 * Returns number of enabled bodies in the simulation.
	 * Good for performance tuning, as number of simultaneous enabled
	 * bodies should be low for best performance.
	 */
	int				enabledBodies() const;

private:
	enum { MAX_CONTACTS = 16 };

	struct CollisionCallbackProxyData
	{
		ODEWorld*				world;
		ODECollisionInterface*	checker;
		dContact				contacts[MAX_CONTACTS];
	};

	dWorldID		m_world;
	dSpaceID		m_space;
	dJointGroupID	m_contacts;
	dGeomID			m_ground;

	static void		collisionCallbackProxy( void* data, dGeomID o1, dGeomID o2 );

	ODEWorld( const ODEWorld& );
	ODEWorld& operator=( const ODEWorld& );
};


END_NAMESPACE() // ode


#endif // _ODE_ODEWORLD_H

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
