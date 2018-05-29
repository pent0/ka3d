/******************************************************************************
HLSL Normal Map Specular Shader
by Ben Cloward
bsmji@hotmail.com
http://www.monitorstudios.com/bcloward/

This shader is based on several shaders written by Kevin Bjorke of Nvidia and 
included with the Cg plugin for 3DS Max.

This shader uses a normal map for per-pixel lighting - to give the illusion that
the model surface contains more detail than is really there.  It also adds a specular
component for making models look shiny.

It accepts the following inputs:

Ambient Color
Diffuse Color
Diffuse Texture
Specular Mask (alpha channel of diffuse texture)
Normal Map Texture
Specular Color
Shininess
Point Light Position
Point Light Color

It requires hardware support for DirectX 9.

This shader is intended to be used by the DirectX 9 shader of 3DS Max
but it could also be used in other applications that support the FX format.
	
******************************************************************************/

/************* TWEAKABLES **************/

half4 ambient : Ambient
<
    string UIName = "Ambient Color";
> = {0.25f, 0.25f, 0.25f, 1.0f};

half4 surfColor : Diffuse
<
    string UIName = "Diffuse Color";
> = {1.0f, 1.0f, 1.0f, 1.0f};

texture colorTexture : DiffuseMap
<
    string name = "default_color.dds";
	string UIName = "Diffuse Texture";
    string TextureType = "2D";
>;

half4 specularColor : Specular
<
    string UIName = "Specular Color";
> = { 0.2f, 0.2f, 0.2f, 1.0f };

half shininess <
	string UIWidget = "slider";
    float UIMin = 1;
    float UIMax = 128;
    string UIName = "Shininess";
> = 40;

texture normalMap : NormalMap
<
    string name = "default_bump_normal.dds";
	string UIName = "Normal Map";
    string TextureType = "2D";
>;

/************** light info **************/

half4 light1Pos : POSITION
<
	string UIName = "Light Position";
	string Object = "PointLight";
	string Space = "World";
	int refID = 0;
> = {100.0f, 100.0f, 100.0f, 0.0f};

half4 light1Color : LIGHTCOLOR
<
	int LightRef = 0;
> = { 1.0f, 1.0f, 1.0f, 0.0f };

/***********************************************/
/*** automatically-tracked "tweakables" ********/
/***********************************************/

half4x4 wvp : WorldViewProjection < string UIWidget = "None"; >;
half4x4 worldI : WorldInverse < string UIWidget = "None"; >;
half4x4 viewInvTrans : ViewInverse < string UIWidget = "None"; >;
half4x4 world : World < string UIWidget = "None"; >;

/****************************************************/
/********** CG SHADER FUNCTIONS *********************/
/****************************************************/

// input from application
	struct a2v {
	half4 position		: POSITION;
	half2 texCoord		: TEXCOORD0;
	half3 tangent		: TANGENT;
	half3 binormal		: BINORMAL;
	half3 normal		: NORMAL;
};

// output to fragment program
struct v2f {
        half4 position    	: POSITION;
		half2 texCoord    	: TEXCOORD0;
        float3 eyeVec     	: TEXCOORD1;
        half3 lightVec   	: TEXCOORD2;
};

// blinn lighting with lit function
half4 blinn2(half3 N,
		half3 L,
		half3 V,
		uniform half4 diffuseColor,
		uniform half4 specularColor,
		uniform half shininess
		)
	{
	half3 H = normalize(V+L);
	half4 lighting = lit(dot(L,N), dot(H,N), shininess);
	return diffuseColor*lighting.y + specularColor*lighting.z;
	}

/**************************************/
/***** VERTEX SHADER ******************/
/**************************************/

v2f v(a2v In,
		uniform half4x4 worldViewProj, // object to clip space
		uniform half4x4 WorldIMatrix,  //world to object space
        uniform half4 lightPosition,
        uniform half4x4 ViewInvTrans,
        uniform half4x4 world        // object to world space
        )
{
	v2f Out;

  // transform vertex position to homogeneous clip space
	Out.position = mul(In.position, worldViewProj);
  
  //pass texture coordinates
	Out.texCoord = In.texCoord;

  // compute the 3x3 tranform from tangent space to object space
	half3x3 objTangentXf;

	objTangentXf[0] = In.binormal.xyz;
	objTangentXf[1] = -In.tangent.xyz;
	objTangentXf[2] = In.normal.xyz;

   //put the world space light position in object space
   	half4 objSpaceLightPos = mul(lightPosition, WorldIMatrix);
   	half3 objLightVec = objSpaceLightPos.xyz - In.position.xyz;
   // xform light vector from obj space to tangent space
   	Out.lightVec = mul(objTangentXf, objLightVec );
	
  //compute the eye vector in world space and put it in object space
  	half4 objSpaceEyePos = mul(ViewInvTrans[3], WorldIMatrix);
  // xform eye vector from obj space to tangent space
  	half3 objEyeVec = objSpaceEyePos.xyz - In.position.xyz;
  	Out.eyeVec = mul(objTangentXf, objEyeVec);

	return Out;
}

/**************************************/
/***** FRAGMENT PROGRAM ***************/
/**************************************/

float4 f(v2f In,
			uniform sampler2D colorTex,
            uniform sampler2D bumpTex,
            uniform half4 ambient,
            uniform half4 diffuseColor,
            uniform half4 specularColor,
            uniform half shininess,
            uniform half4 light1Color
            ) : COLOR
{
  //fetch the diffuse map
  half4 colorMap = tex2D(colorTex, In.texCoord.xy);
  
  // fetch the bump normal from the normal map
  half3 normal = tex2D(bumpTex, In.texCoord).xyz * 2.0 - 1.0;

  // calculate lighting vectors
  half3 N = normal;
  half3 V = normalize(In.eyeVec);
  half3 L = normalize(In.lightVec);
  
  //lighting
  
  //ambient light
  half4 C = ambient*colorMap;
  
  //diffuse and specular
  C += light1Color * blinn2(N, L, V, colorMap*diffuseColor, specularColor*colorMap.a, shininess);

  return C;
}

/****************************************************/
/********** SAMPLERS ********************************/
/****************************************************/

sampler2D colorTextureSampler = sampler_state
{
	Texture = <colorTexture>;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Anisotropic;
};

sampler2D normalMapSampler = sampler_state
{
	Texture = <normalMap>;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Anisotropic;
};

/****************************************************/
/********** TECHNIQUES ******************************/
/****************************************************/

technique Complete
{ 
    pass envPass 
    {		
	VertexShader = compile vs_1_1 v(wvp,worldI,light1Pos,viewInvTrans,world);
	ZEnable = true;
	ZWriteEnable = true;
	CullMode = CW;
	PixelShader = compile ps_2_0 f(colorTextureSampler,normalMapSampler,ambient,surfColor,specularColor,shininess,light1Color);
    }
}

