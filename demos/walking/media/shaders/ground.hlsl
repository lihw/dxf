cbuffer cbChangesEveryFrame : register(b0)
{
    matrix MVP[100];
};
cbuffer cbInitial : register(b1)
{
    float4 TileUV[100];
};

struct VS_INPUT
{
    float3 Pos    : POSITION;
    float3 Normal : NORMAL;
    float2 Tex    : TEXCOORD;
};

struct PS_INPUT
{
    float4 Pos     : SV_POSITION;
    float4 UV      : TEXCOORD0;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT VS(VS_INPUT input, uint instanceID : SV_InstanceID) 
{
    PS_INPUT output = (PS_INPUT)0;
   
    output.Pos = mul(float4(input.Pos, 1.0f), MVP[instanceID]);
    output.UV = float4(input.Tex * TileUV[instanceID].xy + TileUV[instanceID].zw, 0, 1);
    return output;
}

Texture2D <float4> TileTexture : register(t0);
sampler	TileSampler : register(s0);  

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(PS_INPUT input) : SV_TARGET
{
    float3 color = TileTexture.Sample(TileSampler, input.UV.xy ).rgb;

    return float4(color.rgb, 1.0); 
}
