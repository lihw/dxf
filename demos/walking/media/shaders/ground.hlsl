cbuffer cbChangesEveryFrame : register(b0)
{
    matrix MVP[128];
    int4   Tiling[128];
};
cbuffer cbInitial : register(b1)
{
    float4 TileUV[64];
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
    float4 uv = TileUV[Tiling[instanceID].x];
    output.UV = float4(float2(1-input.Tex.y, 1-input.Tex.x) * uv.xy + uv.zw, 0, 1);
    //output.UV = float4(1 - input.Tex.y, 1 - input.Tex.x, 0, 1);
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

    //return float4(input.UV.xy, 0, 1.0); 
    return float4(color.rrr, 1.0); 
}
