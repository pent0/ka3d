#include <ode/odex.h>
#include <math/float3.h>
#include <math/float3x3.h>
#include <math/float3x4.h>
#include <config.h>


USING_NAMESPACE(math)


void dBodyGetAngularVel( dBodyID body, float3* v )
{
	const float* p = dBodyGetAngularVel( body );
	v->x = p[0];
	v->y = p[1];
	v->z = p[2];
}

void dBodyGetLinearVel( dBodyID body, float3* v )
{
	const float* p = dBodyGetLinearVel( body );
	v->x = p[0];
	v->y = p[1];
	v->z = p[2];
}

void dBodyGetPosition( dBodyID body, float3* v )
{
	const float* p = dBodyGetPosition( body );
	v->x = p[0];
	v->y = p[1];
	v->z = p[2];
}

void dBodyGetRotation( dBodyID body, float3x3* v )
{
	float3x3& rot = *v;
	
	const float* r = dBodyGetRotation( body );
	rot(0,0) = r[0];
	rot(0,1) = r[1];
	rot(0,2) = r[2];
	rot(1,0) = r[4+0];
	rot(1,1) = r[4+1];
	rot(1,2) = r[4+2];
	rot(2,0) = r[8+0];
	rot(2,1) = r[8+1];
	rot(2,2) = r[8+2];
}

void dBodyGetTransform( dBodyID body, float3x4* v )
{
	float3x4& tm = *v;

	const float* p = dBodyGetPosition( body );
	tm(0,3) = p[0];
	tm(1,3) = p[1];
	tm(2,3) = p[2];
	
	const float* r = dBodyGetRotation( body );
	tm(0,0) = r[0];
	tm(0,1) = r[1];
	tm(0,2) = r[2];
	tm(1,0) = r[4+0];
	tm(1,1) = r[4+1];
	tm(1,2) = r[4+2];
	tm(2,0) = r[8+0];
	tm(2,1) = r[8+1];
	tm(2,2) = r[8+2];
}

void dBodySetAngularVel( dBodyID body, const float3& v )
{
	dBodySetAngularVel( body, v.x, v.y, v.z );
}

void dBodySetLinearVel( dBodyID body, const float3& v )
{
	dBodySetLinearVel( body, v.x, v.y, v.z );
}

void dBodySetPosition( dBodyID body, const float3& v )
{
	dBodySetPosition( body, v.x, v.y, v.z );
}

void dBodySetRotation( dBodyID body, const float3x3& v )
{
	dMatrix3 r;
	r[0*4+0] = v(0,0);
	r[0*4+1] = v(0,1);
	r[0*4+2] = v(0,2);
	r[1*4+0] = v(1,0);
	r[1*4+1] = v(1,1);
	r[1*4+2] = v(1,2);
	r[2*4+0] = v(2,0);
	r[2*4+1] = v(2,1);
	r[2*4+2] = v(2,2);
	dBodySetRotation( body, r );
}

void dBodySetTransform( dBodyID body, const float3x4& v )
{
	dBodySetPosition( body, v(0,3), v(1,3), v(2,3) );
	
	dMatrix3 r;
	r[0*4+0] = v(0,0);
	r[0*4+1] = v(0,1);
	r[0*4+2] = v(0,2);
	r[1*4+0] = v(1,0);
	r[1*4+1] = v(1,1);
	r[1*4+2] = v(1,2);
	r[2*4+0] = v(2,0);
	r[2*4+1] = v(2,1);
	r[2*4+2] = v(2,2);
	dBodySetRotation( body, r );
}

void dGeomSetPosition( dGeomID geom, const float3& v )
{
	dGeomSetPosition( geom, v.x, v.y, v.z );
}

void dGeomSetRotation( dGeomID geom, const float3x3& v )
{
	dMatrix3 r;
	r[0*4+0] = v(0,0);
	r[0*4+1] = v(0,1);
	r[0*4+2] = v(0,2);
	r[1*4+0] = v(1,0);
	r[1*4+1] = v(1,1);
	r[1*4+2] = v(1,2);
	r[2*4+0] = v(2,0);
	r[2*4+1] = v(2,1);
	r[2*4+2] = v(2,2);
	dGeomSetRotation( geom, r );
}

void dGeomSetTransform( dGeomID geom, const float3x4& v )
{
	dGeomSetPosition( geom, v(0,3), v(1,3), v(2,3) );
	
	dMatrix3 r;
	r[0*4+0] = v(0,0);
	r[0*4+1] = v(0,1);
	r[0*4+2] = v(0,2);
	r[1*4+0] = v(1,0);
	r[1*4+1] = v(1,1);
	r[1*4+2] = v(1,2);
	r[2*4+0] = v(2,0);
	r[2*4+1] = v(2,1);
	r[2*4+2] = v(2,2);
	dGeomSetRotation( geom, r );
}

void dGeomGetPosition( dGeomID geom, float3* v )
{
	const float* p = dGeomGetPosition( geom );
	v->x = p[0];
	v->y = p[1];
	v->z = p[2];
}

void dGeomGetRotation( dGeomID geom, float3x3* v )
{
	float3x3& rot = *v;
	
	const float* r = dGeomGetRotation( geom );
	rot(0,0) = r[0];
	rot(0,1) = r[1];
	rot(0,2) = r[2];
	rot(1,0) = r[4+0];
	rot(1,1) = r[4+1];
	rot(1,2) = r[4+2];
	rot(2,0) = r[8+0];
	rot(2,1) = r[8+1];
	rot(2,2) = r[8+2];
}

void dGeomGetTransform( dGeomID geom, float3x4* v )
{
	float3x4& tm = *v;

	const float* p = dGeomGetPosition( geom );
	tm(0,3) = p[0];
	tm(1,3) = p[1];
	tm(2,3) = p[2];
	
	const float* r = dGeomGetRotation( geom );
	tm(0,0) = r[0];
	tm(0,1) = r[1];
	tm(0,2) = r[2];
	tm(1,0) = r[4+0];
	tm(1,1) = r[4+1];
	tm(1,2) = r[4+2];
	tm(2,0) = r[8+0];
	tm(2,1) = r[8+1];
	tm(2,2) = r[8+2];
}

void dWorldImpulseToForce( dWorldID world, float dt, const float3& imp, float3* f )
{
	dVector3 df;
	dWorldImpulseToForce( world, dt, imp.x, imp.y, imp.z, df );
	f->x = df[0];
	f->y = df[1];
	f->z = df[2];
}

void dBodyAddForce( dBodyID body, const float3& f )
{
	dBodyAddForce( body, f.x, f.y, f.z );
}

void dBodyAddTorque( dBodyID body, const float3& f )
{
	dBodyAddTorque( body, f.x, f.y, f.z );
}

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
