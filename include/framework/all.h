#ifndef _FRAMEWORK_H
#define _FRAMEWORK_H


/**
 * @defgroup framework Simple application framework
 *
 * Simple platform independent application framework (currently Win32/DirectX and PS2
 * ports exist). The framework support keyboard and mouse input and manages
 * window and rendering context creation.
 *
 * Usage instructions:
 *
 * <ol>
 * <li>Define configure and init functions in your code.
 * <li>Derive class, say MyApp, from App and define MyApp constructor and MyApp update().
 * <li>Link to framework-md (Release) or framework-mdd (Debug), or insert framework project into workspace.
 * <li>Ready to run!
 * <li>For simple examples, see samples/ directory.
 * </ol>
 *
 * @{
 */

#include <framework/App.h>

/** @} */


#endif // _FRAMEWORK_H

// Copyright (C) 2004-2006 Pixelgene Ltd. All rights reserved. Consult your license regarding permissions and restrictions.
