//
// Post-process effect: Multiply image RGB by image Alpha
//
// Copyright (C) 2004 Jani Kajala. All rights reserved. Consult your license regarding permissions and restrictions.
//

// maps
texture BASEMAP; // source image


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

float4 pshader( PS_IN IN ) : COLOR
{
	float4 basecol = tex2D( basetex, IN.uv0 );
	float3 c = basecol.xyz * basecol.w;
	return float4( c, 1 );
}


technique Default
{
	pass P0
	{
		VertexShader = compile vs_1_1 vshader();
		PixelShader = compile ps_1_1 pshader();

		ZWriteEnable = FALSE;
		ZEnable = FALSE;
	}
}
