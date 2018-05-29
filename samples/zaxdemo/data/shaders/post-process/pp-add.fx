//
// Post-process effect: additive blending of source image
// using ONE/ONE source/destination blending
//
// Copyright (C) 2004 Jani Kajala. All rights reserved. Consult your license regarding permissions and restrictions.
//

// maps
texture BASEMAP; // source image
// shader specific constants
half4	LEVEL = float4(1,1,1,1);


struct VS_IN
{
	float3 pos		 : POSITION;
	float2 uv0		 : TEXCOORD0;
};

struct PS_IN
{
	float4 pos		 : POSITION;
	float2 uv0		 : TEXCOORD0;
};


sampler basetex = sampler_state
{
	Texture   = (BASEMAP);
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
};


PS_IN vshader( const VS_IN IN )
{
	PS_IN o = (PS_IN)0;
	o.pos = float4(IN.pos,1);
	o.uv0 = IN.uv0;
	return o;
}

half4 pshader( PS_IN p ) : COLOR
{
	return tex2D( basetex, p.uv0 ) * LEVEL;
}


technique Default
{
	pass P0x
	{
		VertexShader = compile vs_1_1 vshader();
		PixelShader = compile ps_1_1 pshader();
		CullMode = NONE;

		SrcBlend = ONE;
		DestBlend = ONE;
		AlphaBlendEnable = TRUE;
		
		ZWriteEnable = FALSE;
		ZEnable = FALSE;
	}
}
