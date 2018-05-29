//
// Simple shader with plain, non-shaded, texture.
// Can be used as most basic 'self-illuminating' material.
//
// Supported conditional compilation defines:
// DISABLE_FOG
// DISABLE_TEXTURE
// ENABLE_ADDITIVE / ENABLE_TRANSPARENCY
// ENABLE_LIGHTMAPPING
// ENABLE_SKINNING
// ENABLE_SORT (define as "BACKTOFRONT")
//
// Copyright (C) 2004 Jani Kajala. All rights reserved. Consult your license regarding permissions and restrictions.
//

//--------------------------
//        Settings
//--------------------------

#ifdef DISABLE_TEXTURE
// material properties
half3		DIFFUSEC : Color <string UIName="Color";> = {0.1, 0.1, 0.1};
#else
// maps
texture		BASEMAP : DiffuseMap <string UIName="Texture";>;
#endif

// fog properties
float4		FOG_RANGE	= {0.0, 10000.0, 1.0/(10000+0), 0};
half3		FOG_COLOR	= {0,0,0};

#if defined(ENABLE_ADDITIVE) || defined(ENABLE_TRANSPARENCY)
// rendering priority: 
//  0=front-to-back (default)
// -1=back-to-front (transparent)
// -3=back-to-front (debug lines etc.)
int PRIORITY = -1;
#endif

#ifdef ENABLE_SORT
String SORT=ENABLE_SORT;
#endif

#ifdef ENABLE_LIGHTMAPPING
texture		LIGHTMAP0;
texture		LIGHTMAP1;
texture		LIGHTMAP2;
#endif

float4x4	VIEWTM : VIEW;
#ifdef ENABLE_SKINNING
float4x3 	BONEWORLDTM[29];
float4x4	VIEWPROJTM;
#else
float4x4	TOTALTM : WORLDVIEWPROJECTION;
float4x4	WORLDTM : WORLD;
#endif


//--------------------------
//     Data format
//--------------------------

struct VS_IN
{
	float3 pos : POSITION;
	float2 uv0 : TEXCOORD0;
	#ifdef ENABLE_LIGHTMAPPING
	float2 uv1 : TEXCOORD1;
	#endif
	#ifdef ENABLE_SKINNING
	float2 weights : TEXCOORD2;
	float3 indices : TEXCOORD3;
	#endif
};

struct PS_IN
{
	float4 hpos		: POSITION;
	float4 fog		: COLOR0;
	float2 uv0		: TEXCOORD0;
	#ifdef ENABLE_LIGHTMAPPING
	float2 uv1		: TEXCOORD1;
	#endif
};


//--------------------------
//     Texture samplers
//--------------------------

#ifndef DISABLE_TEXTURE
sampler basetex = sampler_state
{
	Texture   = (BASEMAP);
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
};
#endif

#ifdef ENABLE_LIGHTMAPPING
sampler lighttex0 = sampler_state
{
	Texture   = (LIGHTMAP0);
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
};

sampler lighttex1 = sampler_state
{
	Texture   = (LIGHTMAP0);
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
};

sampler lighttex2 = sampler_state
{
	Texture   = (LIGHTMAP0);
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
};
#endif


//-----------------------------------------
//    Vertex transform functions
//-----------------------------------------

half3 transform( float4x3 tm, half3 v )
{
	return half3( dot(tm[0],v)+tm[3].x, dot(tm[1],v)+tm[3].y, dot(tm[2],v)+tm[3].z );
}

half3 rotate( float3x3 tm, half3 v )
{
	return half3( dot(tm[0],v), dot(tm[1],v), dot(tm[2],v) );
}

/**
 * Transforms vertex.
 *
 * Assumptions #ifdef ENABLE_SKINNING geometry:
 *
 * 1) VS_IN defined before including this file 
 *
 * 2) VS_IN has at least following members:
	float3 pos : POSITION;
	float2 weights : TEXCOORD0;
	float3 indices : TEXCOORD1;
 *
 * 3) Following globals have been set:
	float4x3	BONEWORLDTM[<n>];
	float4x4	VIEWPROJTM : ViewProjection;
 *
 *
 * Assumptions #ifndef ENABLE_SKINNING geometry:
 *
 * 1) VS_IN defined before including this file 
 *
 * 2) VS_IN has at least following members:
	float3 pos : POSITION;
 *
 * 3) Following globals have been set:
	float4x4	TOTALTM : WorldViewProjection;
 */
void transformLitVertex( VS_IN IN,
	out half4 hpos, out half3 worldpos )
{
#ifdef ENABLE_SKINNING
	
	// bone 1 influence
	float i = IN.indices.x;
	float w = IN.weights.x;
	float4x3 bonetm = BONEWORLDTM[i];
	worldpos = transform( bonetm, IN.pos ) * w;

	// bone 2 influence
	i = IN.indices.y;
	w = IN.weights.y;
	bonetm = BONEWORLDTM[i];
	worldpos += transform( bonetm, IN.pos ) * w;

	// bone 3 influence
	i = IN.indices.z;
	w = (1-IN.weights.y-IN.weights.x);
	bonetm = BONEWORLDTM[i];
	worldpos += transform( bonetm, IN.pos ) * w;

	// world position to camera projection space
	hpos = mul( VIEWPROJTM, half4(worldpos,1) );

#else

	// transform vertex to projection space
	worldpos = mul( half4(IN.pos,1), WORLDTM );
	hpos = mul( half4(IN.pos,1), TOTALTM );

#endif
}


//----------------------------------
//        Shader functions
//----------------------------------

PS_IN vshader( const VS_IN IN )
{
	PS_IN OUT;

	OUT.uv0 = IN.uv0;
	#ifdef ENABLE_LIGHTMAPPING
	OUT.uv1 = IN.uv1;
	#endif

	float3 worldpos;
	transformLitVertex( IN, OUT.hpos, worldpos );

	// compute camera Z-distance based fog
	float3 viewpos = mul( float4(worldpos,1), VIEWTM );
	half foglevel = saturate( (viewpos.z - FOG_RANGE.x) * FOG_RANGE.z );
	#ifndef ENGINE_RUNTIME
	foglevel = 0.f;
	#endif
	OUT.fog = float4( FOG_COLOR, foglevel );
	return OUT;
}

float4 pshader( PS_IN IN ) : COLOR
{
	#ifndef DISABLE_TEXTURE
	half4 color = tex2D( basetex, IN.uv0 );
	half3 c = color.xyz;
	half alpha = color.w;
	#else
	half3 c = DIFFUSEC.xyz;
	half alpha = 1;
	#endif

	#ifdef ENABLE_LIGHTMAPPING
	half3 lightmap = .333f * (tex2D(lighttex0, IN.uv1) + tex2D(lighttex1, IN.uv1) + tex2D(lighttex2, IN.uv1));
	c *= lightmap.xyz;
	#endif

	#ifndef DISABLE_FOG
	c = lerp( c, IN.fog.xyz, IN.fog.w );
	#endif
	
	return float4( c, alpha );
}

half4 psBlack( PS_IN IN ) : COLOR
{
	return half4( 0,0,0,0 );
}


//--------------------------
//       Techniques
//--------------------------

technique Default
{
	pass P0
	{
		VertexShader = compile vs_1_1 vshader();
		PixelShader = compile ps_2_0 pshader();

		#ifdef ENABLE_TRANSPARENCY
		AlphaBlendEnable = TRUE;
		SrcBlend = SRCALPHA;
		DestBlend = INVSRCALPHA;
		CullMode = NONE;
		ZWriteEnable = FALSE;
		#endif
		
		#ifdef ENABLE_ADDITIVE
		CullMode = NONE;
		SrcBlend = ONE;
		DestBlend = ONE;
		AlphaBlendEnable = TRUE;
		ZWriteEnable = FALSE;
		#endif
	}
}

technique GLOW
{
	pass P0
	{
		VertexShader = compile vs_1_1 vshader();
		PixelShader = compile ps_1_1 psBlack();
	}
}
