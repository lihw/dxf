cbuffer cbChangesEveryFrame
{
    matrix CameraView;
    matrix Projection;
};

struct VS_INPUT
{
    float3 Pos    : POSITION;
};

struct GS_INPUT
{
    float3 Pos    : POSITION;
};

struct PS_INPUT
{
    float4 Pos    : SV_POSITION;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
GS_INPUT VS(VS_INPUT input) 
{
    GS_INPUT output = (GS_INPUT)0;
   
    output.Pos = input.Pos;
    return output;
}

//--------------------------------------------------------------------------------------
// Geometry Shader
//--------------------------------------------------------------------------------------
[maxvertexcount(8)]
void mainGS( point GS_INPUT input, inout TriangleStream<PS_INPUT> output )
{
    PS_INPUT output = (PS_INPUT)0;
   
    // Expand the point to a small quad
    float4 p = mul(float4(input.Pos, 1.0), CameraView);
    
    // front face
    //  p2 --- p3 
    //  |      |
    //  |      |
    //  p0 --- p1 
    float4 p0 = p + float4(-0.01, -0.01f, 0.001, 0);
    float4 p1 = p + float4( 0.01, -0.01f, 0.001, 0);
    float4 p2 = p + float4(-0.01,  0.01f, 0.001, 0);
    float4 p3 = p + float4( 0.01,  0.01f, 0.001, 0);
    
    psInput.Pos  = mul(p0, Projection);
    output.Append(psInput);
    
    psInput.Pos  = mul(p2, Projection);
    output.Append(psInput);
    
    psInput.Pos  = mul(p1, Projection);
    output.Append(psInput);
    
    psInput.Pos  = mul(p3, Projection);
    output.Append(psInput);

    output.RestartStrip();
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(PS_INPUT input) : SV_TARGET
{
    return float4(0, 0, 0, 0.0); 
}
