#ifndef _ODE_ALL_H
#define _ODE_ALL_H


/**
 * @defgroup ode Physics simulation support library (ODE C++ wrapper)
 *
 * This small wrapper library is currently <em>open layer</em> above 
 * Open Dynamics Engine (ODE). This means that you will need to use 
 * ODE itself too when using the wrapper, but on the other hand you 
 * will have all the features ODE provides, plus some of them are just much 
 * more convenient to use with the wrapper.
 *
 * So why wrap ODE functionality at all?
 * <ol>
 * <li>Better resource management (RAII-idiom)
 * <li>Easy interface to create rigid bodies from 3D engine meshes
 * <li>Maps transformations between simulated rigid bodies and 3D engine meshes (its not always straightforward, for example due to scaling, hierarchies and different pivot point transformations)
 * <li>Hides repeating setup tasks and avoid need to copypaste code.
 * <li>Provide ODE function wrappers which take in 3D engine vector (float3, etc.) and matrix classes (odex.h) for convenience.
 * </ol>
 *
 * Event though the wrapper has currently limited functionality, 
 * it makes various tasks easier to get you started using ODE.
 * For example you don't need any callbacks, lengthy triangle
 * mesh and transformation setups, etc., to get a simulation 
 * up-and-running, even when the geometry comes from 3dsmax scene. 
 *
 * For usage example see tutorials/physics_simulation.
 *
 * @{
 */

#include <ode/odex.h>
#include <ode/ODEWorld.h>
#include <ode/ODEObject.h>

/** @} */


#endif // _ODE_ALL_H

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
