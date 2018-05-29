#ifndef _ODE_DEFAULTCOLLISIONCHECKER_H
#define _ODE_DEFAULTCOLLISIONCHECKER_H


#include <ode/ODECollisionInterface.h>


BEGIN_NAMESPACE(ode) 


/**
 * Called by simulator to get contacts between objects.
 * Application checks collisions and returns number of contacts.
 *
 * @ingroup ode
 */
class ODEDefaultCollisionChecker :
	public ODECollisionInterface
{
public:
	/**
	 * Called when two objects are near.
	 * @param o1 The first object geometry.
	 * @param o2 The second object geometry.
	 * @param contacts Contact buffer.
	 * @param maxcontacts Maximum number of contacts in the buffer.
	 * @return Number of contacts in the buffer to add as contact joints to the simulation solver.
	 */
	int		checkCollisions( dGeomID o1, dGeomID o2, dContact* contacts, int maxcontacts );
};


END_NAMESPACE() // ode


#endif // _ODE_DEFAULTCOLLISIONCHECKER_H

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
