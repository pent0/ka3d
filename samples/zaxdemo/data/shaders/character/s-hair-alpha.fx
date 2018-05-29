//
// Diffuse/specular hair shader for skinned objects.
// Transparency can be controlled with normal map alpha channel.
// This shader can be used also for regular semi-transparent
// polygons in skinned meshes, but this is especially tweaked
// for hair since hair can be quite accurately sorted by preprocessing
// the hair 'inside-out' polygon order.
//
// Copyright (C) 2004 Jani Kajala. All rights reserved. Consult your license regarding permissions and restrictions.
//

#ifdef ENGINE_RUNTIME
#define ENABLE_SKINNING
#endif

#define ENABLE_TRANSPARENCY
#define ENABLE_SORT "INSIDEOUT"
#include "../level/bump.fx"
