//
// Shader for line rendering.
// Non-textured, RGBA vertex color with transparency,
//
// Shader is drawn after normal geometry 
//
// Supported conditional compilation defines:
// DISABLE_Z
// ENABLE_GLOW
//
// Copyright (C) 2004 Jani Kajala. All rights reserved. Consult your license regarding permissions and restrictions.
//

// transforms
float4x4 TOTALTM;

// rendering priority: 
//  0=front-to-back (default)
// -1=back-to-front (transparent)
int PRIORITY = -1;


struct VS_IN
{
	float3 pos		 : POSITION;
	float4 dif		 : COLOR0;
};

struct PS_IN
{
	float4 pos		 : POSITION;
	float4 dif		 : COLOR0;
};


PS_IN vshader( const VS_IN IN )
{
	PS_IN o = (PS_IN)0;

	o.pos = mul( float4(IN.pos,1), TOTALTM );
	o.dif = IN.dif;
	return o;
}

float4 pshader( PS_IN IN ) : COLOR
{
	return IN.dif;
}


technique Default
{
	pass P0
	{
		AlphaBlendEnable = TRUE;
		SrcBlend = SRCALPHA;
		DestBlend = INVSRCALPHA;
		#ifdef DISABLE_Z
		ZEnable = FALSE;
		#else
		ZEnable = TRUE;
		#endif
		
		VertexShader = compile vs_1_1 vshader();
		PixelShader = compile ps_1_1 pshader();
	}	
}

#ifdef ENABLE_GLOW
technique GLOW
{
	pass P0
	{
		VertexShader = compile vs_1_1 vshader();
		PixelShader = compile ps_1_1 pshader();
	}
}
#endif
