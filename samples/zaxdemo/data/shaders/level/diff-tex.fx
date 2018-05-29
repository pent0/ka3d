//
// Simple diffuse shader.
//
// Map channels:
// - Basemap: Diffuse color
// - Optional transparency in diffuse map alpha (ifdef ENABLE_TRANSPARENCY)
// - Optional lightmap (3) (ifdef ENABLE_LIGHTMAPPING)
//
// Supported conditional compilation defines:
// ENABLE_SKINNING
// ENABLE_TRANSPARENCY
// ENABLE_GLOW
// ENABLE_SORT (either "INSIDEOUT" or "BACKTOFRONT")
// DISABLE_FOG
// DISABLE_TEXTURE
//
// Copyright (C) 2004 Jani Kajala. All rights reserved. Consult your license regarding permissions and restrictions.
//

//--------------------------
//        Settings
//--------------------------

// maps
#ifndef DISABLE_TEXTURE
texture		BASEMAP : DiffuseMap <string UIName="Diffuse map";>;
#endif

#ifdef ENABLE_LIGHTMAPPING
texture		LIGHTMAP0;
texture		LIGHTMAP1;
texture		LIGHTMAP2;
#endif

// material properties
float3		AMBIENTC : Color <string UIName="Ambient color";> = {0.1, 0.1, 0.1};
#ifdef DISABLE_TEXTURE
float3		DIFFUSEC : Color <string UIName="Diffuse color";> = {1.0, 1.0, 1.0};
#endif

// light properties
float3		LIGHTP0 : Position <string UIName="Light position"; string Object="PointLight"; int RefID=0;> = {-10,-10,10};
float3 		LIGHTC0 : LightColor <string UIName = "Light color"; int LightRef=0;> = { 1.0f, 1.0f, 1.0f };

// fog properties
float4		FOG_RANGE	= {0.0, 10000.0, 1.0/(10000+0), 0};
float3		FOG_COLOR	= {0,0,0};

#ifdef ENABLE_TRANSPARENCY
// rendering priority: 
//  0=front-to-back (default)
// -1=back-to-front (transparent)
// -3=back-to-front (debug lines etc.)
int PRIORITY = -1;
#endif

#ifdef ENABLE_SORT
String SORT=ENABLE_SORT;
#endif

// transforms
float3		CAMERAP : WORLDCAMERAPOSITION;
float4x4	VIEWTM : VIEW;
#ifdef ENABLE_SKINNING
float4x3 	BONEWORLDTM[29];
float4x4	VIEWPROJTM;
#else
float4x4	WORLDTM : WORLD;
float4x4	TOTALTM : WORLDVIEWPROJECTION;
#endif


//--------------------------
//     Data format
//--------------------------

struct VS_IN
{
	float3 pos : POSITION;
	float3 normal : NORMAL;
	float2 uv0 : TEXCOORD0;
	float2 uv1 : TEXCOORD1;
#ifdef ENABLE_SKINNING
	float2 weights : TEXCOORD2;
	float3 indices : TEXCOORD3;
#endif
};

struct PS_IN
{
	float4 pos : POSITION;
	float4 fog : COLOR0;
	float2 uv0 : TEXCOORD0;
	float2 uv1 : TEXCOORD1;	
	float3 L : TEXCOORD2;
	float3 N : TEXCOORD3;
};

struct PS_IN_GLOW
{
	float4 pos : POSITION;
	float2 uv0 : TEXCOORD0;
};


//--------------------------
//     Texture samplers
//--------------------------

#ifndef DISABLE_TEXTURE
sampler2D colortex = sampler_state
{
	Texture = <BASEMAP>;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;
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
 * Transforms vertex and its normal.
 *
 * Assumptions #ifdef ENABLE_SKINNING geometry:
 *
 * 1) VS_IN defined before including this file 
 *
 * 2) VS_IN has at least following members:
	float3 pos : POSITION;
	float3 normal : NORMAL;     
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
	float3 normal : NORMAL;     
 *
 * 3) Following globals have been set:
	float4x4	WORLDTM : World;
	float4x4	TOTALTM : WorldViewProjection;
 */
void transformVertex( VS_IN IN,
	out half4 hpos, out half3 worldpos, out half3 worldnormal )
{
#ifdef ENABLE_SKINNING
	
	// bone 1 influence
	float i = IN.indices.x;
	float w = IN.weights.x;
	float4x3 bonetm = BONEWORLDTM[i];
	worldpos = transform( bonetm, IN.pos ) * w;
	worldnormal = rotate( bonetm, IN.normal ) * w;

	// bone 2 influence
	i = IN.indices.y;
	w = IN.weights.y;
	bonetm = BONEWORLDTM[i];
	worldpos += transform( bonetm, IN.pos ) * w;
	worldnormal += rotate( bonetm, IN.normal ) * w;

	// bone 3 influence
	i = IN.indices.z;
	w = (1-IN.weights.y-IN.weights.x);
	bonetm = BONEWORLDTM[i];
	worldpos += transform( bonetm, IN.pos ) * w;

	// world position to camera projection space
	hpos = mul( VIEWPROJTM, half4(worldpos,1) );

	// normalize world normal and tangent
	worldnormal = normalize( worldnormal );

#else

	// transform vertex to projection space
	hpos = mul( half4(IN.pos,1), TOTALTM );

	// transform normal
	worldpos = mul( half4(IN.pos,1), WORLDTM );
	worldnormal = mul( half4(IN.normal,0), WORLDTM );

#endif
}


PS_IN vshader( VS_IN IN )
{
	PS_IN o;

	// copy texcoords
	o.uv0 = IN.uv0;
	o.uv1 = IN.uv1;

	// transform vertex
	float3 worldpos, worldnormal;
	transformVertex( IN, o.pos, worldpos, worldnormal );

	// compute view, light and half angle vectors
	float3 worldlight = normalize( LIGHTP0 - worldpos );

	// store world space L and N for pixel shader
	o.L = worldlight;
	o.N = worldnormal;

	// compute camera Z-distance based fog
	float3 viewpos = mul( float4(worldpos,1), VIEWTM );
	float foglevel = saturate( (viewpos.z - FOG_RANGE.x) * FOG_RANGE.z );
	#ifndef ENGINE_RUNTIME
	foglevel = 0.f;
	#endif
	o.fog = float4( FOG_COLOR, foglevel );
	return o;
}

half4 pshader( PS_IN IN ) : COLOR
{
	#ifdef DISABLE_TEXTURE
	half4 color = half4(DIFFUSEC,1);
	#else
	half4 color = tex2D(colortex, IN.uv0);
	#endif

	#ifdef ENABLE_LIGHTMAPPING
	half4 lightmap = .333f * (tex2D(lighttex0, IN.uv1) + tex2D(lighttex1, IN.uv1) + tex2D(lighttex2, IN.uv1));
	half3 c = lightmap.xyz * color.xyz;
	#else
	half ndotl = dot( normalize(IN.L), IN.N );
	half3 amb = AMBIENTC.xyz * color.xyz;
	half3 dif = ndotl * LIGHTC0.xyz * color.xyz;
	half3 c = amb + dif;
	#endif

	#ifndef DISABLE_FOG
	c = lerp( c, IN.fog.xyz, IN.fog.w );
	#endif

	#ifdef ENABLE_TRANSPARENCY
	half alpha = color.a;
	#else
	half alpha = 1.0;
	#endif
	
	return half4( c, alpha );
}

PS_IN_GLOW vsGlow( VS_IN IN )
{
	PS_IN_GLOW o;

	// copy texcoords
	o.uv0 = IN.uv0;

	// transform vertex
	float3 worldpos, worldnormal;
	transformVertex( IN, o.pos, worldpos, worldnormal );
	return o;
}

half4 psGlow( PS_IN_GLOW IN ) : COLOR
{
#ifdef ENABLE_GLOW
	half4 color = tex2D(colortex, IN.uv0);
	half3 c = color.xyz * color.w;
	return half4( c, 1 );
#else
	return half4( 0,0,0,0 );
#endif
}


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
	}
}

#ifndef ENABLE_TRANSPARENCY
technique GLOW
{
	pass P0
	{
		VertexShader = compile vs_1_1 vsGlow();
		PixelShader = compile ps_1_1 psGlow();
	}
}
#endif
