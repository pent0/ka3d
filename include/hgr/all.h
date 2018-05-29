#ifndef _HGR_ALL_H
#define _HGR_ALL_H


/**
 * @defgroup hgr Hierarchical graphics library
 *
 * Hierarchical graphics library provides support for hierarchical
 * transformations, key-framed animations and scene file input/output.
 * 
 * Scene files can be loaded by creating Scene class instance
 * with scene file name as parameter (and additional paths for
 * textures and shaders if needed).
 *
 * Key-framed animation playback is handled by TransformAnimation
 * and TransformAnimationSet classes. For simple usage example
 * see samples/hgr_file_test. Scene class also provides shortcut for
 * scene-wide animation playback, applyAnimations(time).
 *
 * Different rendering pipes (see Pipe and derived class GlowPipe and DefaultPipe) 
 * provide more abstract usage level for different rendering techniques.
 * For example application can render normal scene and then glow on 
 * top of that with one line call. Even tho convenient and time saving, 
 * rendering pipes have still been kept as very simple so that 
 * user can easily define own pipes if needed, or even go to more low level
 * rendering support and skipping pipes altogether. Only sample application
 * to use rendering pipes is currently samples/render_to_texture_test --
 * all others use Camera and Context classes directly. See DefaultPipe
 * for most simple rendering pipe implementation.
 *
 * @{
 */

#include <hgr/Camera.h>
#include <hgr/Console.h>
#include <hgr/DefaultPipe.h>
#include <hgr/Dummy.h>
#include <hgr/Globals.h>
#include <hgr/GlowPipe.h>
#include <hgr/Light.h>
#include <hgr/LightSorter.h>
#include <hgr/Lines.h>
#include <hgr/Mesh.h>
#include <hgr/Node.h>
#include <hgr/ParticleSystem.h>
#include <hgr/Pipe.h>
#include <hgr/PipeSetup.h>
#include <hgr/Scene.h>
#include <hgr/SceneInputStream.h>
#include <hgr/SceneOutputStream.h>
#include <hgr/TransformAnimation.h>
#include <hgr/TransformAnimationSet.h>
#include <hgr/UserPropertySet.h>
#include <hgr/ViewFrustum.h>
#include <hgr/Visual.h>

/** @} */


#endif // _HGR_ALL_H

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
