//
// Parallax mapping shader with adjustable 
// glossiness (diffuse map alpha channel), adjustable 
// shininess (specular exponent) and adjustable parallax effect
// (height in normal map alpha channel and separate 
// material specific parallax offset)
//
// Map channels:
// - Basemap: Diffuse color + glossiness in alpha
// - Normal map: Normal + height in alpha
// - Optional glow map (glow strength in alpha) (ifdef ENABLE_GLOW)
// - Optional opacity map (opacity in alpha) (ifdef ENABLE_TRANSPARENCY)
// - Optional lightmap (3) (ifdef ENABLE_LIGHTMAPPING)
//
// Supported preprocessor defines:
// DISABLE_FOG
// ENABLE_GLOW
// ENABLE_LIGHTMAPPING
// ENABLE_REFLECTION
// ENABLE_SELFILLUM
// ENABLE_SKINNING
// ENABLE_TRANSPARENCY
//
// Copyright (C) 2005 Jani Kajala (jani@ka3d.com)
//

//--------------------------
//        Settings
//--------------------------

// ambient color
half4 	AMBIENTC : Ambient <string UIName = "Ambient Color";> = {0.1f, 0.1f, 0.1f, 1.0f};

// diffuse color & map & glossiness in alpha
texture BASEMAP : DiffuseMap <string UIName = "Diffuse Map"; string TextureType = "2D";>;

// specular color & exponent
half4 	SPECULARC : Specular <string UIName = "Specular Color";> = {0.3f, 0.3f, 0.3f, 1.0f};
half 	SHININESS <string UIWidget = "slider"; float UIMin = 1; float UIMax = 100; string UIName = "Shininess";> = 30;

// normal map & height in alpha & adjustable parallax offset
texture NORMALMAP : NormalMap <string UIName = "Normal Map"; string TextureType = "2D";>;
half PARALLAX_OFFSET <float UIMin = -1; float UIMax = 1; float UIStep = 0.001; string UIName = "Parallax Offset";> = 0.03;

// transparency (in map alpha channel)
#ifdef ENABLE_TRANSPARENCY
texture OPACITYMAP <string UIName = "Opacity Map"; string TextureType = "2D";>;
// rendering priority:
//  0=front-to-back (default)
// -1=back-to-front (transparent, rendered after normal geometry)
int PRIORITY = -1;
#endif

// glow/self-illumination strength (in map alpha channel)
#ifdef ENABLE_GLOW
texture GLOWMAP : GlowMap <string UIName = "Glow Map"; string TextureType = "2D";>;
#endif

// reflection cube map
#ifdef ENABLE_REFLECTION
texture REFLMAP : ReflectionMap <string UIName="Reflection Cubemap"; string type="Cube";>;
#endif

// light maps
#ifdef ENABLE_LIGHTMAPPING
texture LIGHTMAP0;
texture LIGHTMAP1;
texture LIGHTMAP2;
#endif

// fog properties
float4 	FOG_RANGE	= {0.0, 10000.0, 1.0/(10000+0), 0};
half3 	FOG_COLOR	= {0,0,0};

// point light [0]
float4 	LIGHTP0 : POSITION <string UIName = "Light Position";	string Object = "PointLight"; string Space = "World"; int refID = 0;>;
half4 	LIGHTC0 : LIGHTCOLOR <int LightRef = 0;> = { 1.0f, 1.0f, 1.0f, 0.0f };

// transformations
float4x4 VIEWTM : View;
float4x4 VIEWTMINV : ViewInverse;
#ifdef ENABLE_SKINNING
float4x3 BONEWORLDTM[29];
float4x4 VIEWPROJTM : ViewProjection;
#else
float4x4 TOTALTM : WorldViewProjection;
float4x4 WORLDTM : World;
#endif

const float3x3 LIGHTING_BASIS = float3x3(
	0.816497f, 0.f, 0.577351f,
	-0.408248f, 0.707109f, 0.577351f,
	-0.408248f, -0.707109f, 0.577351f );


//--------------------------
//     Data format
//--------------------------

struct VS_IN
{
	float3 pos					: POSITION;
	float3 normal				: NORMAL;
	float2 uv0					: TEXCOORD0;

	#ifdef ENGINE_RUNTIME
		#ifdef ENABLE_LIGHTMAPPING
			float2 uv1			: TEXCOORD1;
			#ifdef ENABLE_SKINNING
			float2 weights 		: TEXCOORD2;
			float3 indices 		: TEXCOORD3;
			float3 tangent  	: TEXCOORD4;
			#else
			float3 tangent		: TEXCOORD2;
			#endif
		#else
			#ifdef ENABLE_SKINNING
			float2 weights 		: TEXCOORD1;
			float3 indices 		: TEXCOORD2;
			float3 tangent  	: TEXCOORD3;
			#else
			float3 tangent		: TEXCOORD1;
			#endif
		#endif
	#else
		float3 tangent			: TANGENT;
	#endif
};

struct PS_IN 
{
	half4 hpos    	: POSITION;
	half4 fog 		: COLOR0;
	half2 uv0    	: TEXCOORD0;
	float3 viewv    : TEXCOORD1;
	half3 lightv   	: TEXCOORD2;
	
	#ifdef ENABLE_LIGHTMAPPING
	half2 uv1		: TEXCOORD3;
	#endif
	
	#ifdef ENABLE_REFLECTION
	half3x3 tangenttoworld 	: TEXCOORD4;
	half3 worldview			: TEXCOORD5;
	#endif
};

struct PS_IN_GLOW
{
	half4 hpos 		: POSITION;
	half2 uv0 		: TEXCOORD0;
	half2 uv1 		: TEXCOORD1;
};

struct PS_IN_NORMALONLY
{
	half4 hpos    			: POSITION;
	half2 uv0    			: TEXCOORD0;
	half3x3 tangenttoworld	: TEXCOORD1;
};


//--------------------------
//     Texture samplers
//--------------------------

sampler2D colortex = sampler_state
{
	Texture = <BASEMAP>;
	MinFilter = Linear;
	MagFilter = Linear;
};

sampler2D normaltex = sampler_state
{
	Texture = <NORMALMAP>;
	MinFilter = Linear;
	MagFilter = Linear;
};

#ifdef ENABLE_LIGHTMAPPING
sampler2D lighttex0 = sampler_state
{
	Texture = <LIGHTMAP0>;
	MinFilter = Linear;
	MagFilter = Linear;
};

sampler2D lighttex1 = sampler_state
{
	Texture = <LIGHTMAP1>;
	MinFilter = Linear;
	MagFilter = Linear;
};

sampler2D lighttex2 = sampler_state
{
	Texture = <LIGHTMAP2>;
	MinFilter = Linear;
	MagFilter = Linear;
};
#endif

#ifdef ENABLE_TRANSPARENCY
sampler2D opacitytex = sampler_state
{
	Texture = <OPACITYMAP>;
	MinFilter = Linear;
	MagFilter = Linear;
};
#endif

#ifdef ENABLE_GLOW
sampler2D glowtex = sampler_state
{
	Texture = <GLOWMAP>;
	MinFilter = Linear;
	MagFilter = Linear;
};
#endif

#ifdef ENABLE_REFLECTION
samplerCUBE refltex = sampler_state
{
	Texture = <REFLMAP>;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;
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
 * Transforms vertex position, normal and tangent to 
 * clipping and world space. This function also
 * deals with 3dsmax/engine coordinate system differences.
 *
 * Assumptions #ifdef ENABLE_SKINNING geometry:
 *
 * 1) VS_IN defined before this function
 *
 * 2) VS_IN has at least following members:
 *	float3 pos;
 *	float3 normal;     
 *	float2 weights;
 *	float3 indices;
 *	float3 tangent;
 *
 * 3) Following globals have been set:
 *	float4x3	BONEWORLDTM[<n>];
 *	float4x4	VIEWPROJTM;
 *
 * Assumptions #ifndef ENABLE_SKINNING geometry:
 *
 * 1) VS_IN defined before this function
 *
 * 2) VS_IN has at least following members:
 * 	float3 pos;
 *	float3 normal;     
 *	float3 tangent;
 *
 * 3) Following globals have been set:
 *	float4x4	WORLDTM;
 *	float4x4	TOTALTM;
 */
void transformVertex( VS_IN IN, out half4 hpos,
	out half3 worldpos, out half3 worldnormal, 
	out half3 worldtangent, out half3 worldbinormal )
{
#ifdef ENABLE_SKINNING
	
	// bone 1 influence
	float i = IN.indices.x;
	float w = IN.weights.x;
	float4x3 bonetm = BONEWORLDTM[i];
	worldpos = transform( bonetm, IN.pos ) * w;
	worldnormal = rotate( bonetm, IN.normal ) * w;
	worldtangent = rotate( bonetm, IN.tangent ) * w;

	// bone 2 influence
	i = IN.indices.y;
	w = IN.weights.y;
	bonetm = BONEWORLDTM[i];
	worldpos += transform( bonetm, IN.pos ) * w;
	worldnormal += rotate( bonetm, IN.normal ) * w;
	worldtangent += rotate( bonetm, IN.tangent ) * w;

	// bone 3 influence
	i = IN.indices.z;
	w = (1-IN.weights.y-IN.weights.x);
	bonetm = BONEWORLDTM[i];
	worldpos += transform( bonetm, IN.pos ) * w;

	// world position to camera projection space
	hpos = mul( half4(worldpos,1), VIEWPROJTM );

	// normalize world normal and tangent
	worldnormal = normalize( worldnormal );
	worldtangent = normalize( worldtangent );
	worldbinormal = cross( worldnormal, worldtangent );

#else

	// transform vertex position
	hpos = mul( half4(IN.pos,1), TOTALTM );
	worldpos = mul( half4(IN.pos,1), WORLDTM );

	// transform normal and tangent
	worldnormal = mul( half4(IN.normal,0), WORLDTM );
	#ifdef ENGINE_RUNTIME
	worldtangent = mul( half4(IN.tangent,0), WORLDTM );
	worldbinormal = cross( worldnormal, worldtangent );
	#else
	worldtangent = mul( half4(-IN.tangent,0), WORLDTM );
	worldbinormal = cross( worldtangent, worldnormal );
	#endif

#endif
}


//----------------------------------
//        Shader functions
//----------------------------------

PS_IN vshader( VS_IN IN )
{
	PS_IN OUT;

	// get transformed vertex position
	half3 worldpos, worldnormal, worldtangent, worldbinormal;
	transformVertex( IN, OUT.hpos, worldpos, worldnormal, worldtangent, worldbinormal );

	// copy data	
	OUT.uv0 = IN.uv0;
	#ifdef ENABLE_LIGHTMAPPING
	OUT.uv1 = IN.uv1;
	#endif

	// tangent->world rotation matrix
	half3x3 tangenttoworld;
	tangenttoworld[0] = worldbinormal;
	tangenttoworld[1] = worldtangent;
	tangenttoworld[2] = worldnormal;

	// light & view vectors in world space
	half3 worldlight = LIGHTP0 - worldpos;
	half3 worldview = VIEWTMINV[3] - worldpos;

	// light & view vectors in tangent space
	OUT.lightv = mul( tangenttoworld, worldlight ); // = mul( worldlight, worldtotangent )
	OUT.viewv = mul( tangenttoworld, worldview );

	// compute camera Z-distance based fog
	#ifdef ENGINE_RUNTIME
	half dist = mul( half4(worldpos,1), VIEWTM ).z;
	half foglevel = saturate( (dist - FOG_RANGE.x) * FOG_RANGE.z );
	OUT.fog = half4( FOG_COLOR, foglevel );
	#else
	OUT.fog = half4( FOG_COLOR, 0 );
	#endif

	// setup per pixel reflection
	#ifdef ENABLE_REFLECTION
	OUT.worldview = worldview;
	OUT.tangenttoworld = tangenttoworld;
	#endif
	return OUT;
}

half4 pshader( PS_IN IN ) : COLOR
{
	// vectors in tangent space
	half3 view = normalize(IN.viewv);
	half3 light = normalize(IN.lightv);
	half3 halfv = normalize(view+light);

	// parallax mapping
	#ifdef DISABLE_PARALLAXMAPPING
	half2 uv = IN.uv0;
	#else
	half2 view1 = half2( view.x, -view.y );
	half4 normalmap0 = tex2D(normaltex, IN.uv0);
	half height = normalmap0.w * 2.0 - 1.0;
	half2 offset = view1 * (height * PARALLAX_OFFSET);
	half2 uv = IN.uv0 + offset;
	#endif

	// source maps
	half4 color = tex2D(colortex, uv);
	half3 normal = tex2D(normaltex, uv).xyz * 2.0 - 1.0;

	// lit vertex
	#ifdef ENABLE_LIGHTMAPPING
		half3 lightmap = 
			tex2D(lighttex0,IN.uv1) * dot(LIGHTING_BASIS[0],normal) +
			tex2D(lighttex1,IN.uv1) * dot(LIGHTING_BASIS[1],normal) +
			tex2D(lighttex2,IN.uv1) * dot(LIGHTING_BASIS[2],normal);
		half3 diff = lightmap * color;
		half3 spec = half3(0,0,0);
		//half4 lt = lit( 1, dot(halfv,normal), SHININESS );
		//half3 spec = color.a * SPECULARC * LIGHTC0.xyz * lt.z * lightmap;
	#else
		half4 lt = lit( dot(light,normal), dot(halfv,normal), SHININESS );
		half3 diff = color * LIGHTC0.xyz * lt.y;
		#ifdef ENABLE_REFLECTION
		half3 spec = color.a * texCUBE( refltex, reflect(IN.worldview,mul(normal,IN.tangenttoworld)) );
		#else
		half3 spec = color.a * SPECULARC * LIGHTC0.xyz * lt.z;
		#endif
	#endif

	// combine lighting
	half3 c = AMBIENTC.xyz*color.xyz + diff + spec;

	// self-illumination
	#ifdef ENABLE_SELFILLUM
	half silevel = tex2D(glowtex,IN.uv0).w;
	half3 si = color.xyz * silevel;
	c *= saturate(1.0 - silevel);
	c += si;
	#endif

	// fog
	#ifndef DISABLE_FOG
	c = lerp( c, IN.fog.xyz, IN.fog.w );
	#endif

	// transparency
	#ifdef ENABLE_TRANSPARENCY
	half alpha = tex2D(opacitytex, IN.uv0).w;
	#else
	half alpha = 1.0;
	#endif

	return half4(c,alpha);
}

PS_IN_NORMALONLY vshaderNormalOnly( VS_IN IN )
{
	PS_IN_NORMALONLY OUT;
	
	OUT.uv0 = IN.uv0;

	half3 worldpos, worldnormal, worldtangent, worldbinormal;
	transformVertex( IN, OUT.hpos, worldpos, worldnormal, worldtangent, worldbinormal );
	
	half3x3 tangenttoworld;
	tangenttoworld[0] = worldbinormal;
	tangenttoworld[1] = worldtangent;
	tangenttoworld[2] = worldnormal;
	
	OUT.tangenttoworld = tangenttoworld;
	return OUT;
}

half4 pshaderNormalOnly( PS_IN_NORMALONLY IN ) : COLOR
{
	half3 normal = tex2D(normaltex, IN.uv0).xyz * 2.0 - 1.0;
	half3 wnormal = mul( normal, IN.tangenttoworld );
	wnormal = saturate(wnormal*.5f + .5f);
	return half4(wnormal,1);
}

PS_IN_GLOW vsGlow( VS_IN IN )
{
	PS_IN_GLOW OUT;

	OUT.uv0 = IN.uv0;
	OUT.uv1 = IN.uv0;

	half3 worldpos, worldnormal, worldtangent, worldbinormal;
	transformVertex( IN, OUT.hpos, worldpos, worldnormal, worldtangent, worldbinormal );
	return OUT;
}

half4 psGlow( PS_IN_GLOW IN ) : COLOR
{
#ifdef ENABLE_GLOW
	half4 color = tex2D(colortex, IN.uv0);
	half weight = tex2D(glowtex, IN.uv1).w;
	return color * weight;
#else
	return half4( 0,0,0,0 );
#endif
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

technique NormalOnly
{ 
    pass P0
    {		
		VertexShader = compile vs_1_1 vshaderNormalOnly();
		PixelShader = compile ps_2_0 pshaderNormalOnly();
    }
}
