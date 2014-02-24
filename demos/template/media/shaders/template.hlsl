cbuffer cbChangesEveryFrame
{
    matrix MVP;
};

cbuffer cbInit
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;
    float3 Attenuation;
    float  Shinness;
    float3 LightDirection;
    float  padding;
};

struct VS_INPUT
{
    float3 Pos    : POSITION;
    float3 Normal : NORMAL;
    float2 Tex    : TEXCOORD;
};

struct PS_INPUT
{
    float4 Pos    : SV_POSITION;
    float4 Color  : TEXCOORD0;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT VS(VS_INPUT input) 
{
    PS_INPUT output = (PS_INPUT)0;
   
    output.Pos = mul(float4(input.Pos, 1.0f), MVP);
    float nDotL = dot(normalize(-LightDirection), normalize(input.Normal));
    //float3 n = normalize(input.Normal) * 0.5f + float3(0.5f, 0.5f, 0.5f);
    //output.Color = float4(n, 1.0f);
    output.Color = nDotL * Diffuse + Ambient;
    return output;
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(PS_INPUT input) : SV_TARGET
{
    return float4(input.Color.rgb, 1.0); 
}
