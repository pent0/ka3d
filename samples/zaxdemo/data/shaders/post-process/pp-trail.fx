//
// Post-process effect: additive blending of two source images 
// using ONE/ZERO source/destination blending and non-equal
// levels of source colors which can be used to do 'light trails'
// and other similar effects.
//
// Note custom parameters:
// BASEMAP1
// BASEMAP2
// LEVEL1
// LEVEL2
//
// Copyright (C) 2004 Jani Kajala. All rights reserved. Consult your license regarding permissions and restrictions.
//

// maps
texture BASEMAP1;
texture BASEMAP2;
// shader specific constants
half4	LEVEL1 = float4(1,1,1,1) * 0.8;
half4	LEVEL2 = float4(1,1,1,1) * 0.8;


struct VS_IN
{
	float3 pos		 : POSITION;
	float2 uv0		 : TEXCOORD0;
};

struct PS_IN
{
	float4 pos		 : POSITION;
	float2 uv0		 : TEXCOORD0;
	float2 uv1		 : TEXCOORD1;
};


sampler baseTex1 = sampler_state
{
	Texture   = (BASEMAP1);
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
};

sampler baseTex2 = sampler_state
{
	Texture   = (BASEMAP2);
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
};


PS_IN vshader( const VS_IN IN )
{
	PS_IN o = (PS_IN)0;
	o.pos = float4(IN.pos,1);
	o.uv0 = IN.uv0;
	o.uv1 = IN.uv0;
	return o;
}

half4 pshader( PS_IN p ) : COLOR
{
	half4 c1 = tex2D( baseTex1, p.uv0 );
	half4 c2 = tex2D( baseTex2, p.uv1 );
	return c1*LEVEL1 + c2*LEVEL2;
}


technique Default
{
	pass P0x
	{
		VertexShader = compile vs_1_1 vshader();
		PixelShader = compile ps_1_1 pshader();
		CullMode = NONE;

		ZWriteEnable = FALSE;
		ZEnable = FALSE;
	}
}
