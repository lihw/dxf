float3 fade3D(float3 t)
{
    return t * t * t * (t * (t * 6 - 15) + 10); // new curve
    //  return t * t * (3 - 2 * t); // old curve
}

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

//Texture2D <float4> Gradient3DTexture : register(t0);
Texture2D <float4> Graident2DTexture : register(t1);
sampler	GradientSampler : register(s1);  

float perm(float x)
{
    return float(Permutation[int(x) % 256]);
}

float grad3D(float x, float2 p)
{
    //float3 g = GraidentSampler.Sample(Graident3D, x / 255.0).rgb;
    //return dot(g, p);
    return 0;
}

float grad2D(float x, float2 p)
{
    float2 g = GraidentSampler.Sample(Graident2D, x / 255.0).rg;
    return dot(g, p);
}

float perlin3D(float3 p)
{
    float3 P = fmod(floor(p), 256.0);

    p -= floor(p);

    float3 f = fade3D(p);

    // HASH COORDINATES FOR 6 OF THE 8 CUBE CORNERS
    float A = perm(P.x) + P.y;
    float AA = perm(A) + P.z;
    float AB = perm(A + 1) + P.z;
    float B = perm(P.x + 1) + P.y;
    float BA = perm(B) + P.z;
    float BB = perm(B + 1) + P.z;


    // AND ADD BLENDED RESULTS FROM 8 CORNERS OF CUBE
    return lerp(
            lerp(lerp(grad3D(perm(AA), p),
                      grad3D(perm(BA), p + float3(-1, 0, 0)), f.x),
                 lerp(grad3D(perm(AB), p + float3(0, -1, 0)),
                      grad3D(perm(BB), p + float3(-1, -1, 0)), f.x), f.y),
            lerp(lerp(grad3D(perm(AA + 1), p + float3(0, 0, -1)),
                      grad3D(perm(BA + 1), p + float3(-1, 0, -1)), f.x),
                 lerp(grad3D(perm(AB + 1), p + float3(0, -1, -1)),
                      grad3D(perm(BB + 1), p + float3(-1, -1, -1)), f.x), f.y),
            f.z);

}

float perlin2D(float2 p)
{
    float2 P = fmod(floor(p), 256.0);

    p -= floor(p);

    float2 f = fade2D(p);

    // HASH COORDINATES FOR 2 OF THE 4 QUAD CORNERS
    float A = perm(P.x) + P.y;
    float B =  perm(P.x + 1) + P.y;

    // AND ADD BLENDED RESULTS FROM 4 CORNERS OF QUAD
    return lerp(lerp(grad2D(perm(A), p),
                     grad2D(perm(B), p + float2(-1, 0)), f.x),
                lerp(grad2D(perm(B + 1), p + float2(0, -1)),
                     grad2D(perm(B + 1), p + float2(-1, -1)), f.x), f.y);
}

/*
float computeHeight(float x, float y, float noiseSize, int octaves)
{
    x *= noiseSize;
    y *= noiseSize;
   
    for (int i = 0; i < octaves; ++i)
    {
        total += perlin2D(x, y);
    }
    total /= (float)octaves;
    total = saturate(total * 0.5f + 0.5f);
}
*/
