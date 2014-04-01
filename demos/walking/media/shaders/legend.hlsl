cbuffer cbChangesEveryFrame
{
    matrix MVP;
};

struct VS_INPUT
{
    float3 Pos    : POSITION;
    float3 Normal : NORMAL;
};

struct PS_INPUT
{
    float4 Pos    : SV_POSITION;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT VS(VS_INPUT input) 
{
    PS_INPUT output = (PS_INPUT)0;
   
    output.Pos = mul(float4(input.Pos, 1.0f), MVP);
    return output;
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(PS_INPUT input) : SV_TARGET
{
    return float4(1.0f, 1.0f, 1.0f, 0.2); 
}
