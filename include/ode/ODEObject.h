#ifndef _ODE_ODEOBJECT_H
#define _ODE_ODEOBJECT_H


#include <ode/common.h>
#include <lang/Array.h>
#include <lang/String.h>
#include <lang/Object.h>
#include <math/float3x4.h>


BEGIN_NAMESPACE(hgr) 
	class Mesh;END_NAMESPACE()


BEGIN_NAMESPACE(ode) 


class ODEWorld;


/**
 * Simple wrapper for ODE collision geometry and simulated rigid body.
 * The class uses meshes bounding volume and triangle information 
 * to calculate requested rigid body objects for the simulation. 
 * The class also takes care of mapping transformations between visual 
 * meshes and simulated rigid bodies.
 *
 * @ingroup ode
 */
class ODEObject :
	public NS(lang,Object)
{
public:
	/** Geometry type used in simulation. */
	enum GeomType
	{
		/** Sphere. */
		GEOM_SPHERE		= 0,
		/** Box. */
		GEOM_BOX		= 1,
		/** Triangle mesh. */
		GEOM_TRIMESH	= 7,
		/** Defaults to triangle mesh. */
		GEOM_DEFAULT	= -1,
	};

	/** Describes how mass of the object is defined. */
	enum MassType
	{
		/** Object with infinite mass has no simulated body at all. */
		MASS_INFINITE,
		/** Mass is specified relative to body size (e.g. water 1 kg/L). */
		MASS_DENSITY,
		/** Mass is specified as absolute value. */
		MASS_TOTAL
	};

	/**
	 * Initializes object without rigid body.
	 */
	ODEObject();

	/**
	 * Creates a rigid body from specified mesh.
	 * @param world World where to create the rigid body.
	 * @param space Collision space.
	 * @param mesh Source mesh.
	 * @param geomclass Geometry type used in simulation.
	 * @param masstype Describes how mass of the object is defined.
	 * @param mass Mass value of the object. See MassType.
	 */
	ODEObject( dWorldID world, dSpaceID space, NS(hgr,Mesh)* mesh, GeomType geomtype, MassType masstype, float mass );

	/** 
	 * Creates object as plain collision geometry.
	 */
	explicit ODEObject( dGeomID geom );

	/** Destroys collision geometry and rigid body. */
	~ODEObject();

	/**
	 * Creates a rigid body from specified mesh.
	 * @param world World where to create the rigid body.
	 * @param space Collision space.
	 * @param mesh Source mesh.
	 * @param geomclass Geometry type used in simulation.
	 * @param masstype Describes how mass of the object is defined.
	 * @param mass Mass value of the object. See MassType.
	 */
	void			create( dWorldID world, dSpaceID space, NS(hgr,Mesh)* mesh, GeomType geomtype, MassType masstype, float mass );

	/** 
	 * Creates object as plain collision geometry.
	 */
	void			create( dGeomID geom );

	/**
	 * Destroys collision geometry and ridig body associated with it.
	 */
	void			destroy();

	/** Disables rigid body simulations but keeps collisions enabled. */
	void			disableRigidBody();

	/** 
	 * Sets visual object's transform from simulated object transform. 
	 * Used after simulation to keep visual and simulated objects synchronized.
	 */
	void			updateVisualTransform();

	/** Adds impulse to the body. */
	void			addImpulse( dWorldID world, const NS(math,float3)& imp, float dt );

	/** Adds impulse to the body at specified (world space) point. */
	void			addImpulseAt( dWorldID world, const NS(math,float3)& point,
						const NS(math,float3)& imp, float dt );

	/** Sets rigid body angular velocity. */
	void			setAngularVel( const NS(math,float3)& v );

	/** Sets rigid body linear velocity. */
	void			setLinearVel( const NS(math,float3)& v );

	/** Sets rigid body position. */
	void			setPosition( const NS(math,float3)& v );

	/** Sets rigid body rotation. */
	void			setRotation( const NS(math,float3x3)& v );

	/** Sets rigid body transform. */
	void			setTransform( const NS(math,float3x4)& v );

	/** Returns rigid body transform. */
	void			getTransform( NS(math,float3x4)* v ) const;

	/** Returns rigid body position. */
	void			getPosition( NS(math,float3)* v ) const;

	/** Returns rigid body rotation. */
	void			getRotation( NS(math,float3x3)* v ) const;

	/** Returns rigid body linear velocity. */
	void			getLinearVel( NS(math,float3)* v ) const;

	/** Returns rigid body angular velocity. */
	void			getAngularVel( NS(math,float3)* v ) const;

	/** Returns rigid body transform. */
	NS(math,float3x4)	transform() const;

	/** Returns rigid body position. */
	NS(math,float3)	position() const;

	/** Returns rigid body rotation. */
	NS(math,float3x3)	rotation() const;

	/** Returns rigid body linear velocity. */
	NS(math,float3)	linearVel() const;

	/** Returns rigid body angular velocity. */
	NS(math,float3)	angularVel() const;

	/** Returns body associated with this object if any. */
	dBodyID			body() const;

	/** Returns geometry associated with this object if any. */
	dGeomID			geom() const;

	/** Returns visual mesh associated with this object if any. */
	NS(hgr,Mesh)*		mesh() const;

	/**
	 * Parses ODEObject object properties from user property string.
	 *
	 * User property can contains set of assignments separated with newline,
	 * for example:
	 * body=box <br>
	 * mass=10 <br>
	 *
	 * Whitespace is ignored, so for example valid string is "body=box\n mass=10".
	 * 
	 * Supported property strings:
	 * <ul>
	 * <li>body=x -- Where x is {box, sphere, trimesh}. If body=x is missing, then
	 * level geometry is created, which is solid triangle mesh with infinite mass.
	 * <li>density=x -- Mass of body as density value. Default density is 1. Mass and Density are exclusive values.
	 * <li>mass=x -- Mass of body as absolute (total) value. Mass and Density are exclusive values.
	 * </ul>
	 *
	 * @exception Exception
	 */
	static void		parseProperties( NS(hgr,Mesh)* mesh, const NS(lang,String)& props,	
						GeomType* geomtype, MassType* masstype, float* mass );

private:
	P(NS(hgr,Mesh))				m_mesh;
	dGeomID						m_geom;
	NS(math,float3x4)				m_ibodytm;
	NS(lang,Array)<NS(math,float3)>	m_vertices;
	NS(lang,Array)<int>			m_indices;

	static void		setMassBox( dBodyID body, MassType masstype, float mass, const NS(math,float3)& size );
	static void		setMassSphere( dBodyID body, MassType masstype, float mass, float r );

	ODEObject( const ODEObject& );
	ODEObject& operator=( const ODEObject& );
};


END_NAMESPACE() // ode


#endif // _ODE_ODEOBJECT_H

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
