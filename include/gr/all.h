/*
 * Collective header for quick tests and examples.
 * Do not use in actual applications to avoid dependencies.
 */
#ifndef _GR_ALL_H
#define _GR_ALL_H


/**
 * @defgroup gr Graphics rendering library
 *
 * Graphics rendering support. This library provides low level rendering related
 * classes (like textures, shaders and geometry primitives) and 
 * interfaces for platform specific rendering device implementations. 
 * For simple usage example, see samples/gr_test source code. 
 *
 * Library is used by initializing Context, which manages all rendering related
 * resources, and then creating other objects by calling createXXX methods
 * in Context. Normally you'd use this library together with hierarchical
 * graphics library hgr, which in turn provides for example scene file loading support.
 *
 * <b>Notes about initialization:</b>
 * Only thing in the library which is platform dependent is the initialization,
 * and it is so for a reason. However, a simple
 * application framework is provided with the engine that hides the details.
 * In case you need more low level access to the initialization (for
 * example to bypass WinMain like in MFC sceneviewer application), you can
 * bypass application framework totally and use raw platform dependent
 * Context constructors like DX_Context. After creating the context
 * with (one platform dependent call), you can you continue using the 
 * gr library in normal platform independent manner.
 *
 * @{
 */

#include <gr/Rect.h>
#include <gr/Context.h>
#include <gr/ContextObject.h>
#include <gr/GraphicsException.h>
#include <gr/Palette.h>
#include <gr/Primitive.h>
#include <gr/Shader.h>
#include <gr/SurfaceFormat.h>
#include <gr/Texture.h>
#include <gr/VertexFormat.h>

/** @} */


#endif // _GR_ALL_H

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
