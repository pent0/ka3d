#ifndef _ODE_ODEX_H
#define _ODE_ODEX_H


#define _ODE_ODECPP_H_
#include <ode/ode.h>


BEGIN_NAMESPACE(math) 
	class float3;
	class float3x3;
	class float3x4;END_NAMESPACE()


void dWorldImpulseToForce( dWorldID world, float dt, const NS(math,float3)& imp, NS(math,float3)* f );

void dBodyGetAngularVel( dBodyID body, NS(math,float3)* v );
void dBodyGetLinearVel( dBodyID body, NS(math,float3)* v );
void dBodyGetPosition( dBodyID body, NS(math,float3)* v );
void dBodyGetRotation( dBodyID body, NS(math,float3x3)* v );
void dBodyGetTransform( dBodyID body, NS(math,float3x4)* v );
void dBodySetAngularVel( dBodyID body, const NS(math,float3)& v );
void dBodySetLinearVel( dBodyID body, const NS(math,float3)& v );
void dBodySetPosition( dBodyID body, const NS(math,float3x4)& v );
void dBodySetRotation( dBodyID body, const NS(math,float3x3)& v );
void dBodySetTransform( dBodyID body, const NS(math,float3x4)& v );
void dBodyAddForce( dBodyID body, const NS(math,float3)& f );
void dBodyAddTorque( dBodyID body, const NS(math,float3)& f );

void dGeomGetPosition( dGeomID geom, NS(math,float3)* v );
void dGeomGetRotation( dGeomID geom, NS(math,float3x3)* v );
void dGeomGetTransform( dGeomID geom, NS(math,float3x4)* v );
void dGeomSetPosition( dGeomID geom, const NS(math,float3x4)& v );
void dGeomSetRotation( dGeomID geom, const NS(math,float3x3)& v );
void dGeomSetTransform( dGeomID geom, const NS(math,float3x4)& f );


#endif // _ODE_ODEX_H

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
