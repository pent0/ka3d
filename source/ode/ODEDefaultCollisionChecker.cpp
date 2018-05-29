#include <ode/ODEDefaultCollisionChecker.h>
#include <ode/collision.h>
#include <config.h>


BEGIN_NAMESPACE(ode) 


int ODEDefaultCollisionChecker::checkCollisions( dGeomID o1, dGeomID o2,
	dContact* contacts, int maxcontacts )
{
	// get geometric properties of contacts
	int numc = dCollide( o1, o2, maxcontacts, 
		&contacts[0].geom, sizeof(contacts[0]) );

	// set contact non-geometric parameters
	for ( int i = 0; i < numc ; ++i ) 
	{
		contacts[i].surface.mode = dContactBounce | dContactSoftCFM;
		contacts[i].surface.mu = dInfinity;
		contacts[i].surface.mu2 = 0.f;
		contacts[i].surface.bounce = 0.1f;
		contacts[i].surface.bounce_vel = 0.1f;
		contacts[i].surface.soft_cfm = 0.0001f;
	}

	return numc;
}


END_NAMESPACE() // ode

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
