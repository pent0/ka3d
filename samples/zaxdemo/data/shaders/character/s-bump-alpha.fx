//
// Diffuse/specular alpha shader for skinned objects.
// Transparency can be controlled with normal map alpha channel.
// This shader is meant for normal semi-transparent polygons
// in skinned meshes, there is special shader for hair (see sa-hair)
//
// Copyright (C) 2004 Jani Kajala. All rights reserved. Consult your license regarding permissions and restrictions.
//

#ifdef ENGINE_RUNTIME
#define ENABLE_SKINNING
#endif

#define ENABLE_TRANSPARENCY
#define ENABLE_SORT "BACKTOFRONT"
#include "../level/bump.fx"
