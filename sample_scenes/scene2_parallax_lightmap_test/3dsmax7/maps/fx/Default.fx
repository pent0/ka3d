// 3ds max effect file
// Simple Lighting Model

// This is used by 3dsmax to load the correct parser
string ParamID = "0x0";

//DxMaterial specific 

// light direction (view space)
float3 lightDir : Direction <  
	string UIName = "Light Direction"; 
	string Object = "TargetLight";
	> = {-0.577, -0.577, 0.577};

// light intensity
float4 I_a = { 0.1f, 0.1f, 0.1f, 1.0f };    // ambient
float4 I_d = { 1.0f, 1.0f, 1.0f, 1.0f };    // diffuse
float4 I_s = { 1.0f, 1.0f, 1.0f, 1.0f };    // specular

// material reflectivity
float4 k_a  <
	string UIName = "Ambient";
	> = float4( 0.47f, 0.47f, 0.47f, 1.0f );    // ambient
	
float4 k_d  <
	string UIName = "Diffuse";
	> = float4( 0.47f, 0.47f, 0.47f, 1.0f );    // diffuse
	
float4 k_s  <
	string UIName = "Specular";
	> = float4( 1.0f, 1.0f, 1.0f, 1.0f );    // diffuse    // specular

int n<
	string UIName = "Specular Power";
	string UIType = "IntSpinner";
	float UIMin = 0.0f;
	float UIMax = 50.0f;	
	>  = 15;


// transformations
float4x4 World      : 		WORLD;
float4x4 View       : 		VIEW;
float4x4 Projection : 		PROJECTION;
float4x4 WorldViewProj : 	WORLDVIEWPROJ;
float4x4 WorldView : 		WORLDVIEW;


struct VS_OUTPUT
{
    float4 Pos  : POSITION;
    float4 col : COLOR0;
       
};

VS_OUTPUT VS(
    float3 Pos  : POSITION, 
    float3 col	: COLOR,
    float3 Norm : NORMAL, 
    float2 Tex  : TEXCOORD0)
{
    VS_OUTPUT Out = (VS_OUTPUT)0;

    float3 L = lightDir;


    float3 P = mul(float4(Pos, 1),(float4x4)World);  // position (view space)
    float3 N = normalize(mul(Norm,(float3x3)World)); // normal (view space)
    float3 R = normalize(2 * dot(N, L) * N - L);          // reflection vector (view space)
    float3 V = normalize(P);                             // view direction (view space)

    Out.Pos  = mul(float4(Pos,1),WorldViewProj);    // position (projected)
    
    float4 Diff = I_a * k_a + I_d * k_d * max(0, dot(N, L)); // diffuse + ambient
    float4 Spec = I_s * k_s * pow(max(0, dot(R, V)), n/4);   // specular
    
    Out.col = Diff + Spec;

    return Out;
}



float4 PS(
    float4 Diff : COLOR0,
    float4 Spec : COLOR1,
    float2 Tex  : TEXCOORD0,
    float2 Tex1 : TEXCOORD1 ) : COLOR
{
    float4 color = Diff + Spec;
    return  color ;
}

technique DefaultTechnique
{
    pass P0
    {
        // shaders
        CullMode = None;
        VertexShader = compile vs_1_1 VS();
        PixelShader  = compile ps_1_1 PS();
    }  
}


