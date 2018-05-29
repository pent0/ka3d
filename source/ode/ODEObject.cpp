#include <ode/ODEObject.h>
#include <gr/Primitive.h>
#include <io/PropertyParser.h>
#include <hgr/Mesh.h>
#include <ode/ode.h>
#include <lang/Exception.h>
#include <string.h>
#include <config.h>


USING_NAMESPACE(io)
USING_NAMESPACE(gr)
USING_NAMESPACE(hgr)
USING_NAMESPACE(lang)
USING_NAMESPACE(math)


BEGIN_NAMESPACE(ode) 


ODEObject::ODEObject() :
	m_mesh( 0 ),
	m_geom( 0 ),
	m_ibodytm( 1.f )
{
}

ODEObject::ODEObject( dGeomID geom ) :
	m_mesh( 0 ),
	m_geom( 0 ),
	m_ibodytm( 1.f )
{
	create( geom );
}

ODEObject::ODEObject( dWorldID world, dSpaceID space, hgr::Mesh* mesh, GeomType geomtype, MassType masstype, float mass ) :
	m_mesh( 0 ),
	m_geom( 0 ),
	m_ibodytm( 1.f )
{
	create( world, space, mesh, geomtype, masstype, mass );
}

void ODEObject::create( dGeomID geom )
{
	destroy();

	m_geom = geom;
}

void ODEObject::create( dWorldID world, dSpaceID space, hgr::Mesh* mesh, GeomType geomtype, MassType masstype, float mass )
{
	assert( world );
	assert( space );
	assert( mesh );

	destroy();

	m_mesh = mesh;

	// default to triangle mesh
	if ( geomtype == GEOM_DEFAULT )
		geomtype = GEOM_TRIMESH;

	// get rigid body world transform
	float3x4 bodyworldtm, bodytm, bodyvertextm;
	mesh->getBodyTransform( &bodyworldtm, &bodytm, &bodyvertextm );
	m_ibodytm = bodytm.inverse();

	// create body if not infinite mass
	dBodyID body = 0;
	if ( masstype != MASS_INFINITE )
        body = dBodyCreate( world );

	// init geometry
	if ( geomtype == GEOM_TRIMESH )
	{
		if ( body )
			setMassBox( body, masstype, mass, bodyvertextm.rotate(mesh->boundBoxMax() - mesh->boundBoxMin()) );

		// collect level geometry
		for ( int i = 0 ; i < mesh->primitives() ; ++i )
		{
			const int oldverts = m_vertices.size();
			Primitive* prim = mesh->getPrimitive(i);
			Primitive::Lock lk( prim, Primitive::LOCK_READ );

			for ( int k = 0 ; k < prim->vertices() ; ++k )
			{
				float4 v;
				prim->getVertexPositions( k, &v, 1 );
				m_vertices.add( bodyvertextm.transform(float3(v.x,v.y,v.z)) );
			}
			for ( int k = 0 ; k < prim->indices() ; ++k )
			{
				int ix;
				prim->getIndices( k, &ix, 1 );
				m_indices.add( ix+oldverts );
			}
		}

		// build level geometry from collected data
		// (no rigid body, 'infinite mass')
		dTriMeshDataID data = dGeomTriMeshDataCreate();

		dGeomTriMeshDataBuildSingle( data, 
			&m_vertices[0], sizeof(float3), m_vertices.size(),
			&m_indices[0], m_indices.size(), 3*sizeof(int) );

		m_geom = dCreateTriMesh( space, data, 0, 0, 0 );
	}
	else if ( geomtype == GEOM_BOX )
	{
		float3 size = bodyvertextm.rotate(mesh->boundBoxMax() - mesh->boundBoxMin());
		if ( body )
			setMassBox( body, masstype, mass, size );

		m_geom = dCreateBox( space, size.x, size.y, size.z );
	}
	else if ( geomtype == GEOM_SPHERE )
	{
		float r = bodyvertextm.rotate(mesh->boundBoxMax() - mesh->boundBoxMin()).x*.5f;
		if ( body )
			setMassSphere( body, masstype, mass, r );

		m_geom = dCreateSphere( space, r );
	}
	else
	{
		if ( body )
			dBodyDestroy( body );
		throwError( Exception( Format("Geometry type {0} creation from mesh (\"{0}\") is unsupported", geomtype, mesh->name()) ) );
	}

	if ( body )
		dGeomSetBody( m_geom, body );
	setTransform( bodyworldtm );
}

ODEObject::~ODEObject()
{
	destroy();
}

void ODEObject::destroy()
{
	if ( m_geom )
	{
		dBodyID body = dGeomGetBody( m_geom );
		if ( body )
			dBodyDestroy( body );

		if ( dTriMeshClass == dGeomGetClass(m_geom) )
		{
			dTriMeshDataID data = dGeomTriMeshGetData( m_geom );
			dGeomTriMeshDataDestroy( data );
		}

		dGeomDestroy( m_geom );
		m_geom = 0;
	}
}

void ODEObject::updateVisualTransform()
{
	assert( m_geom );

	dBodyID body = dGeomGetBody( m_geom );
	if ( body != 0 )
	{
		float3x4 tm;
		getTransform( &tm );
		m_mesh->setTransform( tm * m_ibodytm );
	}
}

void ODEObject::addImpulse( dWorldID world, const float3& imp, float dt )
{
	assert( body() ); // body is required for simulations

	dVector3 f;
	dWorldImpulseToForce( world, dt, imp.x, imp.y, imp.z, f );
	dBodyAddForce( body(), f[0], f[1], f[2] );
}

void ODEObject::addImpulseAt( dWorldID world, const float3& point, 
	const float3& imp, float dt )
{
	assert( body() ); // body is required for simulations

	dVector3 f;
	dWorldImpulseToForce( world, dt, imp.x, imp.y, imp.z, f );
	dBodyAddForceAtPos( body(), f[0], f[1], f[2], point.x, point.y, point.z );
}

Mesh* ODEObject::mesh() const	
{
	return m_mesh;
}

void ODEObject::setRotation( const float3x3& v )
{
	assert( m_geom );

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
	dGeomSetRotation( m_geom, r );
}

void ODEObject::setTransform( const float3x4& v )
{
	assert( m_geom );

	dGeomSetPosition( m_geom, v(0,3), v(1,3), v(2,3) );
	
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
	dGeomSetRotation( m_geom, r );
}

void ODEObject::setAngularVel( const float3& v )
{
	assert( m_geom );
	dBodySetAngularVel( body(), v.x, v.y, v.z );
}

void ODEObject::setLinearVel( const float3& v )
{
	assert( m_geom );
	dBodySetLinearVel( body(), v.x, v.y, v.z );
}

void ODEObject::setPosition( const float3& v )
{
	assert( m_geom );
	dGeomSetPosition( m_geom, v.x, v.y, v.z );
}

void ODEObject::getAngularVel( float3* v ) const
{
	assert( m_geom );
	const float* p = dBodyGetAngularVel( body() );
	v->x = p[0];
	v->y = p[1];
	v->z = p[2];
}

void ODEObject::getLinearVel( float3* v ) const
{
	assert( m_geom );
	const float* p = dBodyGetLinearVel( body() );
	v->x = p[0];
	v->y = p[1];
	v->z = p[2];
}

void ODEObject::getTransform( float3x4* v ) const
{
	assert( m_geom );

	float3x4& tm = *v;

	const float* p = dGeomGetPosition( m_geom );
	tm(0,3) = p[0];
	tm(1,3) = p[1];
	tm(2,3) = p[2];
	
	const float* r = dGeomGetRotation( m_geom );
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

void ODEObject::getRotation( float3x3* v ) const
{
	assert( m_geom );

	float3x3& rot = *v;
	
	const float* r = dGeomGetRotation( m_geom );
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

void ODEObject::getPosition( float3* v ) const
{
	assert( m_geom );
	const float* p = dGeomGetPosition( m_geom );
	v->x = p[0];
	v->y = p[1];
	v->z = p[2];
}

float3x4 ODEObject::transform() const
{
	assert( m_geom );

	float3x4 tm;

	const float* p = dGeomGetPosition( m_geom );
	tm(0,3) = p[0];
	tm(1,3) = p[1];
	tm(2,3) = p[2];
	
	const float* r = dGeomGetRotation( m_geom );
	tm(0,0) = r[0];
	tm(0,1) = r[1];
	tm(0,2) = r[2];
	tm(1,0) = r[4+0];
	tm(1,1) = r[4+1];
	tm(1,2) = r[4+2];
	tm(2,0) = r[8+0];
	tm(2,1) = r[8+1];
	tm(2,2) = r[8+2];

	return tm;
}

float3x3 ODEObject::rotation() const
{
	assert( m_geom );

	float3x3 rot;

	const float* r = dGeomGetRotation( m_geom );
	rot(0,0) = r[0];
	rot(0,1) = r[1];
	rot(0,2) = r[2];
	rot(1,0) = r[4+0];
	rot(1,1) = r[4+1];
	rot(1,2) = r[4+2];
	rot(2,0) = r[8+0];
	rot(2,1) = r[8+1];
	rot(2,2) = r[8+2];

	return rot;
}

float3 ODEObject::position() const
{
	assert( m_geom );

	float3 v;
	const float* p = dGeomGetPosition( m_geom );
	v.x = p[0];
	v.y = p[1];
	v.z = p[2];
	return v;
}

float3 ODEObject::linearVel() const
{
	assert( m_geom );

	float3 v;
	const float* p = dBodyGetLinearVel( body() );
	v.x = p[0];
	v.y = p[1];
	v.z = p[2];
	return v;
}

float3 ODEObject::angularVel() const
{
	assert( m_geom );

	float3 v;
	const float* p = dBodyGetAngularVel( body() );
	v.x = p[0];
	v.y = p[1];
	v.z = p[2];
	return v;
}

dBodyID ODEObject::body() const	
{
	assert( m_geom );
	return dGeomGetBody( m_geom );
}

dGeomID ODEObject::geom() const	
{
	assert( m_geom );
	return m_geom;
}

void ODEObject::parseProperties( Mesh* mesh, const String& props,
	GeomType* geomtype, MassType* masstype, float* mass )
{
	PropertyParser parser( props, mesh->name() );

	// defaults
	*geomtype = GEOM_DEFAULT;
	*masstype = MASS_INFINITE;
	*mass = 0.f;

	// parse geometry shape
	if ( parser.hasKey("physics") )
	{
		const char* sz = parser.getString( "physics" );
		if ( !strcmp(sz,"trimesh") ) // triangle mesh
			*geomtype = GEOM_TRIMESH;
		else if ( !strcmp(sz,"box") ) // bounding box
			*geomtype = GEOM_BOX;
		else if ( !strcmp(sz,"sphere") ) // bounding sphere
			*geomtype = GEOM_SPHERE;
	}

	// see if user has specified mass/density property
	if ( parser.hasKey("mass") )
	{
		const char* sz = parser.getString( "mass" );
		if ( *geomtype == GEOM_DEFAULT )
			throwError( Exception( Format("Failed to parse mesh \"{0}\" user property: 'Mass=<x>' doesnt make sense if object has no 'Physics=<x>' defined", mesh->name()) ) );
		if ( sscanf(sz,"%g",mass) != 1 )
			throwError( Exception( Format("Failed to parse mesh \"{0}\" user property 'mass': {1}", mesh->name(), sz) ) );
		*masstype = MASS_TOTAL;
	}
	else if ( parser.hasKey("density") )
	{
		const char* sz = parser.getString( "density" );
		if ( *geomtype == GEOM_DEFAULT )
			throwError( Exception( Format("Failed to parse mesh \"{0}\" user property: 'Density=<x>' doesnt make sense if object has no 'Physics=<x>' defined", mesh->name()) ) );
		if ( *masstype != MASS_INFINITE )
			throwError( Exception( Format("Failed to parse mesh \"{0}\" user property: Density and Mass are mutually exlusive properties", mesh->name()) ) );
		if ( sscanf(sz,"%g",mass) != 1 )
			throwError( Exception( Format("Failed to parse mesh \"{0}\" user property 'density': {1}", mesh->name(), sz) ) );
		*masstype = MASS_DENSITY;
	}
}

void ODEObject::setMassBox( dBodyID body, MassType masstype, float mass, const float3& size )
{
	dMass m;
	dMassSetZero( &m );
	
	if ( masstype == MASS_TOTAL )
		dMassSetBoxTotal( &m, mass, size.x, size.y, size.z );
	else
		dMassSetBox( &m, mass, size.x, size.y, size.z );

	dBodySetMass( body, &m );
}

void ODEObject::setMassSphere( dBodyID body, MassType masstype, float mass, float r )
{
	dMass m;
	dMassSetZero( &m );
	
	if ( masstype == MASS_TOTAL )
		dMassSetSphereTotal( &m, mass, r );
	else
		dMassSetSphere( &m, mass, r );

	dBodySetMass( body, &m );
}

void ODEObject::disableRigidBody()
{
	if ( m_geom != 0 )
	{
		dBodyID body = dGeomGetBody( m_geom );
		if ( body )
			dBodyDestroy( body );
	}
}


END_NAMESPACE() // ode
