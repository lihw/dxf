cbuffer cbChangesEveryFrame : register(b0)
{
    matrix MVP[128];
    matrix WorldView[128];
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
    float4 WorldPos: TEXCOORD1;
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
    output.WorldPos = mul(float4(input.Pos, 1.0f), WorldView[instanceID]);
    //output.UV = float4(1 - input.Tex.y, 1 - input.Tex.x, 0, 1);
    return output;
}

Texture2D <float4> TileTexture : register(t0);
sampler	TileSampler : register(s0);  

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float2 fade2D(float2 t)
{
    return t * t * t * (t * (t * 6 - 15) + 10); // new curve
    //  return t * t * (3 - 2 * t); // old curve
}

static const int Permutation[] =
{
  225,155,210,108,175,199,221,144,203,116, 70,213, 69,158, 33,252,
  5, 82,173,133,222,139,174, 27,  9, 71, 90,246, 75,130, 91,191,
  169,138,  2,151,194,235, 81,  7, 25,113,228,159,205,253,134,142,
  248, 65,224,217, 22,121,229, 63, 89,103, 96,104,156, 17,201,129,
  36,  8,165,110,237,117,231, 56,132,211,152, 20,181,111,239,218,
  170,163, 51,172,157, 47, 80,212,176,250, 87, 49, 99,242,136,189,
  162,115, 44, 43,124, 94,150, 16,141,247, 32, 10,198,223,255, 72,
  53,131, 84, 57,220,197, 58, 50,208, 11,241, 28,  3,192, 62,202,
  18,215,153, 24, 76, 41, 15,179, 39, 46, 55,  6,128,167, 23,188,
  106, 34,187,140,164, 73,112,182,244,195,227, 13, 35, 77,196,185,
  26,200,226,119, 31,123,168,125,249, 68,183,230,177,135,160,180,
  12,  1,243,148,102,166, 38,238,251, 37,240,126, 64, 74,161, 40,
  184,149,171,178,101, 66, 29, 59,146, 61,254,107, 42, 86,154,  4,
  236,232,120, 21,233,209, 45, 98,193,114, 78, 19,206, 14,118,127,
  48, 79,147, 85, 30,207,219, 54, 88,234,190,122, 95, 67,143,109,
  137,214,145, 93, 92,100,245,  0,216,186, 60, 83,105, 97,204, 52
};

Texture1D <float2> Gradient2DTexture : register(t1);
sampler	GradientSampler : register(s1);  

float perm(float x)
{
    return float(Permutation[int(x) % 256]);
}

float grad2D(float x, float2 p)
{
    float2 g = Gradient2DTexture.Sample(GradientSampler, x / 255.0).rg;
    return dot(g, p);
}

float perlin2D(float2 p)
{
    float2 P = fmod(floor(p), 256.0);
	P.x = (P.x < 0)? 256.0f + P.x : P.x;
	P.y = (P.y < 0)? 256.0f + P.y : P.y;

    p -= floor(p);

    float2 f = fade2D(p);

    // HASH COORDINATES FOR 2 OF THE 4 QUAD CORNERS
    float A = perm(P.x) + P.y;
    float B =  perm(P.x + 1) + P.y;

    // AND ADD BLENDED RESULTS FROM 4 CORNERS OF QUAD
    return lerp(lerp(grad2D(perm(A), p),
                     grad2D(perm(B), p + float2(-1, 0)), f.x),
                lerp(grad2D(perm(A + 1), p + float2(0, -1)),
                     grad2D(perm(B + 1), p + float2(-1, -1)), f.x), f.y);
}


float4 PS(PS_INPUT input) : SV_TARGET
{
    //float3 color = TileTexture.Sample(TileSampler, input.UV.xy ).rgb;
    //return float4(input.UV.xy, 0, 1.0); 
    float r = perlin2D(input.WorldPos.zx) * 0.5 + 0.5;
	//float r = perlin2D(input.WorldPos.zx);
    return float4(r, r, r, 1.0); 
	//return float4(input.WorldPos.x / 27.0, 0, 0, 1);
}
