//
// Simple shader with plain, non-shaded, texture,
// combined with vertex alpha and color.
// NOTE: Requires transformed vertices
//
// Supported conditional compilation defines:
// ENABLE_TRANSPARENCY
// ENABLE_ADDITIVE
// ENABLE_POINTFILTER
// ENABLE_SORT (either "INSIDEOUT" or "BACKTOFRONT")
//
// Copyright (C) 2004 Jani Kajala. All rights reserved. Consult your license regarding permissions and restrictions.
//

// maps
texture	BASEMAP;

// rendering priority: 
//  0=front-to-back (default)
// -1=back-to-front (transparent)
// -3=back-to-front (sprites)
int PRIORITY = -3;

#ifdef ENABLE_SORT
String SORT=ENABLE_SORT;
#endif


struct PS_IN
{
	float4 pos		 : POSITION;
	float4 dif		 : COLOR0;
	float2 uv0		 : TEXCOORD0;
};


sampler colorTex = sampler_state
{
	Texture   = (BASEMAP);
	#ifdef ENABLE_POINTFILTER
	MinFilter = POINT;
	MagFilter = POINT;
	MipFilter = POINT;
	#else
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
	#endif
};


half4 pshader( PS_IN IN ) : COLOR
{
	float4 rgba = tex2D( colorTex, IN.uv0 );
	rgba *= IN.dif;
	#ifdef ENABLE_TRANSPARENCY
	return float4( rgba.xyz, rgba.w );
	#else
	return float4( rgba.xyz*rgba.w, 1 );
	#endif
}


technique Default
{
	pass P0
	{
		PixelShader = compile ps_1_1 pshader();

		#ifdef ENABLE_ADDITIVE
		CullMode = NONE;
		SrcBlend = ONE;
		DestBlend = ONE;
		AlphaBlendEnable = TRUE;
		ZWriteEnable = FALSE;
		#endif

		#ifdef ENABLE_TRANSPARENCY
		CullMode = NONE;
		SrcBlend = SRCALPHA;
		DestBlend = INVSRCALPHA;
		AlphaBlendEnable = TRUE;
		ZWriteEnable = FALSE;
		#endif
	}
}
