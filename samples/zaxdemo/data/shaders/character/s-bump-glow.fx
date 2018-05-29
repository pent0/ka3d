//
// Diffuse/specular bump shader with self-illumination support for skinned objects.
//
// Copyright (C) 2004 Jani Kajala. All rights reserved. Consult your license regarding permissions and restrictions.
//

#ifdef ENGINE_RUNTIME
#define ENABLE_SKINNING
#endif

#define ENABLE_GLOW
#define ENABLE_SELFILLUM
#include "../level/bump.fx"
