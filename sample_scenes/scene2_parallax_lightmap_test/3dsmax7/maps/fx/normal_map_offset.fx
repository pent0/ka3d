/******************************************************************************
HLSL Offset Mapping Shader
by Ben Cloward
bsmji@hotmail.com
http://www.monitorstudios.com/bcloward/

This shader is based on several shaders written by Kevin Bjorke of Nvidia and 
included with the Cg plugin for 3DS Max.

It accepts the following inputs:

Ambient Color
Diffuse Color
Diffuse Texture
Specular Mask (alpha channel of diffuse texture)
Normal Map Texture
DXT5 Compressed Normal Map
Specular Color
Shininess
Point Light Position
Point Light Color
Offset Amount

It requires hardware support for DirectX 9.

This shader has all of the features of the Normal Map Specular shader plus one
additional feature that makes the normal mapping look much more convincing.
It's called Offset Normal Mapping, Parallax Mapping, or Simulated Displacement
Mapping.

The idea behind this added realism is to "punch out" the surface of the model
based on a height map that goes along with the normal map so you get parallax
movement on the surface when your viewpoint changes. It's hard to explain it,
but when you see it, it surprising how much more real the bumps look.

The effect is achieved by multiplying the UV coordinates used for the normal
map and the diffuse map by a height map value (stored in the alpha channel of
the normal map) and by the view vector in the pixel shader. We then use the
resulting coordinates to look up our normal map and diffuse map - instead of
the original texture coordinates.
	
You can control the height of the bumps with the "Offset" value in the shader.
You also need to include a height map in the alpha channel of your normal map.
If you created the normal map with high res geometry, you can get a height map
by rendering a Z depth image of the geometry. If you created your normal map
by converting a bump map, you can just use the original bump map in the alpha
channel. If you just have a normal map and need to generate a height map from
it, you can find a great tool to do that right here:
http://66.70.170.53/Ryan/heightmap/heightmap.html

Normal Map compression is an option that you can use with this shader.
Copy the red channel of your normal map to the alpha channel and then put the 
height map into the red channel instead.  Then delete the blue channels and save
your normal map in DXT5 DDS format.  Put your compressed normal map in the DXT5
Normal Map slot for the shader.  Then choose "Compressed" as the technique
instead of "complete."

This shader is intended to be used by the DirectX 9 shader of 3DS Max
but it could also be used in other applications that support the FX format.
	
******************************************************************************/

/************* TWEAKABLES **************/

half4 ambient : Ambient
<
    string UIName = "Ambient Color";
> = {0.5f, 0.5f, 0.5f, 1.0f};

half4 surfColor : Diffuse
<
    string UIName = "Surface Color";
> = {0.5f, 0.5f, 0.5f, 1.0f};

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

texture CnormalMap : CNormalMap
<
    string name = "default_bump_normal.dds";
	string UIName = "DXT5 Normal Map";
    string TextureType = "2D";
>;

half offset <
    float UIMin = -1;
    float UIMax = 1;
    float UIStep = 0.001;
    string UIName = "Offset";
> = 0.04;

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

/************** lighting textures **************/



/***********************************************/
/*** automatically-tracked "tweakables" ********/
/***********************************************/

half4x4 wvp : WorldViewProj < string UIWidget = "None"; >;
half4x4 worldI : WorldI < string UIWidget = "None"; >;
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
            uniform half4 light1Color,
			uniform float offsetBias
            ) : COLOR
{  
  // calculate lighting vectors
  half3 V = normalize(In.eyeVec);
  half3 L = normalize(In.lightVec);

  //I have to do this take make it look right - don't know why
  half2 newV = V.xy;
  newV.y = -newV.y;
  
  //offset the texture coords based on the view vector
  half height = tex2D(bumpTex, In.texCoord.xy).a;
  half2 offset = newV * (height * 2.0 - 1.0) * offsetBias;
  half2 newTexCoord = In.texCoord.xy + offset;

  //fetch the diffuse map
  half4 colorMap = tex2D(colorTex, newTexCoord);

  
  // fetch the bump normal from the normal map
  half3 normal = tex2D(bumpTex, newTexCoord).xyz * 2.0 - 1.0;

  half3 N = normal;
  
  //lighting
  
  //ambient light
  half4 C = ambient*colorMap;
  
  //diffuse light
  C += light1Color * blinn2(N, L, V, colorMap*diffuseColor, specularColor*colorMap.a, shininess);

  return C;
}

float4 f2(v2f In,
			uniform sampler2D colorTex,
            uniform sampler2D bumpTex,
            uniform half4 ambient,
            uniform half4 diffuseColor,
            uniform half4 specularColor,
            uniform half shininess,
            uniform half4 light1Color,
			uniform float offsetBias
            ) : COLOR
{  
  // calculate lighting vectors
  half3 V = normalize(In.eyeVec);
  half3 L = normalize(In.lightVec);

  //I have to do this take make it look right - don't know why
  half2 newV = V.xy;
  newV.y = -newV.y;
  
  //offset the texture coords based on the view vector
  //height value is stored in the red channel of compressed normal maps
  half height = tex2D(bumpTex, In.texCoord.xy).r;
  half2 offset = newV * (height * 2.0 - 1.0) * offsetBias;
  half2 newTexCoord = In.texCoord.xy + offset;

  //fetch the diffuse map
  half4 colorMap = tex2D(colorTex, newTexCoord);

  
  // fetch the bump normal from the normal map
  //swizzle the compressed dxt5 format
  half3 normal = tex2D(bumpTex, newTexCoord).wyz * 2.0 - 1.0;
  //generate the z component of the vector
  normal.z = sqrt(1 - normal.x * normal.x - normal.y * normal.y);

  half3 N = normal;
  
  //lighting
  
  //ambient light
  half4 C = ambient*colorMap;
  
  //diffuse light
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

sampler2D CnormalMapSampler = sampler_state
{
	Texture = <CnormalMap>;
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
	PixelShader = compile ps_2_0 f(colorTextureSampler,normalMapSampler,ambient,surfColor,specularColor,shininess,light1Color,offset);
    }
}

technique Compressed
{ 
    pass envPass 
    {		
	VertexShader = compile vs_1_1 v(wvp,worldI,light1Pos,viewInvTrans,world);
	ZEnable = true;
	ZWriteEnable = true;
	CullMode = CW;
	PixelShader = compile ps_2_0 f2(colorTextureSampler,CnormalMapSampler,ambient,surfColor,specularColor,shininess,light1Color,offset);
    }
}
