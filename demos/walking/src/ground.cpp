// ground.cpp
//
// Created at 2014/03/31
//
// Hongwei Li hongwei.li@amd.com
// All rights reserved

#include "ground.h"

#include <algorithm>
#include <float.h>

#include "walking.h"

#define NUM_HCOLORS 3
#define NUM_VCOLORS 2

Ground::Ground()
{
    m_block = NULL;
    m_numActiveBlocks = 0;

	//FILE* fp = fopen("tiles.txt", "wb");
	//for (int n = 0; n < NUM_VCOLORS; ++n)
	//{
	//	for (int e = 0; e < NUM_HCOLORS; ++e)
	//	{
	//		for (int s = 0; s < NUM_VCOLORS; ++s)
	//		{
	//			for (int w = 0; w < NUM_HCOLORS; ++w)
	//			{
	//				fprintf(fp, "%d%d%d%d\n", VCOLORS[n], HCOLORS[e], VCOLORS[s], HCOLORS[w]);
	//			}
	//		}
	//	}
	//}
	//fclose(fp);

	m_shader = NULL;
}

Ground::~Ground()
{
	SAFE_DELETE(m_shader);
    SAFE_DELETE(m_block);
	SAFE_DELETE(m_cbEveryFrame);
	//SAFE_DELETE(m_tileTexture);
}

HRESULT Ground::initialize(ID3D11Device* device, ID3D11DeviceContext* context,
						   const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& direction)
{
	HRESULT hr;

#define SHADER_ROOT L"../demos/walking/media/shaders"
    m_shader = new dxf::Shader(device);
    V_RETURN(m_shader->addVSShader(SHADER_ROOT L"/ground.hlsl", "VS"));
    V_RETURN(m_shader->addPSShader(SHADER_ROOT L"/ground.hlsl", "PS"));
#undef SHADER_ROOT 

    m_block = new dxf::Model(device);
    V_RETURN(m_block->loadPlane(BLOCK_SIZE, BLOCK_SIZE, m_shader));

	m_cbEveryFrame = new dxf::CBuffer<CbEveryFrameStruct>(device);
    V_RETURN(m_cbEveryFrame->create(context, "cb-everyframe", "vs", 0));
    
#define TEXTURE_ROOT "../demos/walking/media/textures"
    loadTiles(device, context, TEXTURE_ROOT"/tileconf.txt", TEXTURE_ROOT"/tile.bmp");
#undef TEXTURE_ROOT

    // Initialize the original tiling.
    // Compute the bounding box of the spotlight area
    float px = direction.x * SPOTLIGHT_RADIUS;
    float pz = direction.z * SPOTLIGHT_RADIUS;

    DirectX::XMFLOAT3 p0, p1;
	
    float s, c;
    DirectX::XMScalarSinCos(&s, &c, DirectX::XMConvertToRadians(SPOTLIGHT_FOV * 0.5f));

    p0.z = pz * c - px * s + position.z;
    p0.x = pz * s + px * c + position.x;
    
    p1.z = pz * c + px * s + position.z;
    p1.x = -pz * s + px * c + position.x;

	float triangle[][2] = 
	{
		{position.x, position.z},
		{p0.x, p0.z},
		{p1.x, p1.z},
	};

    float xmin, xmax, zmin, zmax;

    xmin = std::min(position.x, std::min(p0.x, p1.x));
    xmax = std::max(position.x, std::max(p0.x, p1.x));
    zmin = std::min(position.z, std::min(p0.z, p1.z));
    zmax = std::max(position.z, std::max(p0.z, p1.z));

    int xmini = (int)floor(xmin / BLOCK_SIZE);
    int zmini = (int)floor(zmin / BLOCK_SIZE);
    int xmaxi = (int)ceil(xmax / BLOCK_SIZE);
    int zmaxi = (int)ceil(zmax / BLOCK_SIZE);

    m_bb[0] = xmini;
    m_bb[1] = zmini;
    m_bb[2] = xmaxi;
    m_bb[3] = zmaxi;

	int h = xmaxi - xmini + 1; // The number of rows
	int w = zmaxi - zmini + 1; // The number of columns

    m_numActiveBlocks = h * w;

    // Fill the tiles in a scanline order
    for (int x = 0; x < h; ++x)
    {
        for (int z = 0; z < w; ++z)
        {
            int nColor = -1; 
            if (x > 0)
            {
                int index = m_tiling[(x - 1) * w + z];
                nColor = m_tiles[index].colors[2];
            }
            int wColor = -1;
            if (z > 0)
            {
                int index = m_tiling[x * w + z - 1];
                wColor = m_tiles[index].colors[1];
            }
            
            if (nColor == -1)
            {
                nColor = rand() % NUM_VCOLORS;
            }
            if (wColor == -1)
            {
                wColor = rand() % NUM_HCOLORS;
            }

            m_tiling[x * w + z] = getTileIndex(nColor, rand() % NUM_HCOLORS, rand() % NUM_VCOLORS, wColor);
        }
    }

    validateTiling(w, h);
    
	return S_OK;
}

void Ground::update(const DirectX::XMFLOAT3& position,
        const DirectX::XMFLOAT3& direction, CModelViewerCamera* camera)
{
    // Compute the bounding box of the spotlight area
    float px = direction.x * SPOTLIGHT_RADIUS;
    float pz = direction.z * SPOTLIGHT_RADIUS;

    DirectX::XMFLOAT3 p0, p1;
	
    float s, c;
    DirectX::XMScalarSinCos(&s, &c, DirectX::XMConvertToRadians(SPOTLIGHT_FOV * 0.5f));

    p0.z = pz * c - px * s + position.z;
    p0.x = pz * s + px * c + position.x;
    
    p1.z = pz * c + px * s + position.z;
    p1.x = -pz * s + px * c + position.x;

	float triangle[][2] = 
	{
		{position.x, position.z},
		{p0.x, p0.z},
		{p1.x, p1.z},
	};

    float xmin, xmax, zmin, zmax;

    xmin = std::min(position.x, std::min(p0.x, p1.x));
    xmax = std::max(position.x, std::max(p0.x, p1.x));
    zmin = std::min(position.z, std::min(p0.z, p1.z));
    zmax = std::max(position.z, std::max(p0.z, p1.z));

    int xmini = (int)floor(xmin / BLOCK_SIZE);
    int zmini = (int)floor(zmin / BLOCK_SIZE);
    int xmaxi = (int)ceil(xmax/ BLOCK_SIZE);
    int zmaxi = (int)ceil(zmax / BLOCK_SIZE);

    int h = xmaxi - xmini + 1;
    int w = zmaxi - zmini + 1;

    int tiling[100];
    memset(tiling, 0xff, 100 * sizeof(int));

    //
    // Intersect with the previous bounding box
    //
    int interBB[4];
    interBB[0] = std::max(m_bb[0], xmini);
    interBB[1] = std::max(m_bb[1], zmini);
    interBB[2] = std::min(m_bb[2], xmaxi);
    interBB[3] = std::min(m_bb[3], zmaxi);
    
    // Fill the tiling plane with intersected tiles
    for (int x = interBB[0]; x <= interBB[2]; ++x)
    {
        for (int z = interBB[1]; z <= interBB[3]; ++z)
        {
            int oldj = z - m_bb[1];
            int oldi = x - m_bb[0];
            int newj = z - zmini; 
            int newi = x - xmini; 

            int oldTileIndex = oldi * (m_bb[3] - m_bb[1] + 1) + oldj;
            int tileIndex = newi * w + newj;

            tiling[tileIndex] = m_tiling[oldTileIndex];
        }
    }

	//DXF_ASSERT(memcmp(tiling, m_tiling, sizeof(int) * w * h) == 0);//


    // Filing the rest
    int hedge[200];
    int vedge[200];

    memset(hedge, 0xff, 200 * sizeof(int));
    memset(vedge, 0xff, 200 * sizeof(int));
    
    // Color the edges
    for (int x = 0; x < h; ++x)
    {
        for (int z = 0; z < w; ++z)
        {
            int tileIndex = tiling[x * w + z];
            if (tileIndex != -1)
            {
                hedge[x * (w + 1) + z]     = m_tiles[tileIndex].colors[3];
                hedge[x * (w + 1) + z + 1] = m_tiles[tileIndex].colors[1];
                
                vedge[z * (h + 1) + x]     = m_tiles[tileIndex].colors[0];
                vedge[z * (h + 1) + x + 1] = m_tiles[tileIndex].colors[2];
            }
        }
    }

    for (int x = 0; x < h; ++x)
    {
        for (int z = 0; z <= w; ++z)
        {
            if (hedge[x * (w + 1) + z] == -1)
            {
                hedge[x * (w + 1) + z] = (x + z + w + h) % NUM_HCOLORS;
            }
        }
    }

	for (int z = 0; z < w; ++z)
	{
		for (int x = 0; x <= h; ++x)
		{
			if (vedge[z * (h + 1) + x] == -1)
            {
                vedge[z * (h + 1) + x] = (x + z + w + h) % NUM_VCOLORS;
            }
		}
	}
    
    // Fill the tiles.
    DirectX::XMMATRIX mProj = camera->GetProjMatrix();
    DirectX::XMMATRIX mView = camera->GetViewMatrix();
    DirectX::XMMATRIX viewProj = mView * mProj;

    m_numActiveBlocks = 0;
    for (int x = 0; x < h; ++x)
    {
        for (int z = 0; z < w; ++z)
        {
            int ww = hedge[x * (w + 1) + z];
            int e = hedge[x * (w + 1) + z + 1];
            int n = vedge[z * (h + 1) + x];
            int s = vedge[z * (h + 1) + x + 1];
            
            int tileIndex = getTileIndex(n, e, s, ww);
			DXF_ASSERT(tileIndex >= 0);
            m_tiling[x * w + z] = tileIndex;

            float xx = (float)(x + xmini) * BLOCK_SIZE;
            float zz = (float)(z + zmini) * BLOCK_SIZE;

            DirectX::XMMATRIX translation = DirectX::XMMatrixTranslation(xx, 0, zz);
            int i = m_numActiveBlocks++;
            m_cbEveryFrame->data().mvp[i] = XMMatrixTranspose(translation * viewProj); 
        }
    }

    validateTiling(w, h);

    m_bb[0] = xmini;
    m_bb[1] = zmini;
    m_bb[2] = xmaxi;
    m_bb[3] = zmaxi;
}

void Ground::render(ID3D11DeviceContext* context)
{
	m_cbEveryFrame->sync(context);
        
	m_block->render(context, m_numActiveBlocks);
}
    
bool Ground::isInsideTriangle(const float triangle[][2], float x, float z)
{
    float y1 = triangle[0][0];
    float y2 = triangle[1][0];
    float y3 = triangle[2][0];
    
    float x1 = triangle[0][1];
    float x2 = triangle[1][1];
    float x3 = triangle[2][1];

    float y = x;
    x = z;

    return (x1 - x2) * (y - y1) - (y1 - y2) * (x - x1) > 0 &&
           (x2 - x3) * (y - y2) - (y2 - y3) * (x - x2) > 0 &&
           (x3 - x1) * (y - y3) - (y3 - y1) * (x - x3) > 0;    
}
    
void Ground::loadTiles(ID3D11Device* device,
                       ID3D11DeviceContext* context,
                       const char* tileConfiguration,
                       const char* tileImage)
{
    // Load the configuration.
	int numTiles = 0;
	for (int n = 0; n < NUM_VCOLORS; ++n)
	{
		for (int e = 0; e < NUM_HCOLORS; ++e)
        {
            for (int s = 0; s < NUM_VCOLORS; ++s)
            {
                for (int w = 0; w < NUM_HCOLORS; ++w)
                {
                    Block b;
                    b.colors[0] = n;
                    b.colors[1] = e;
                    b.colors[2] = s;
                    b.colors[3] = w;

                    m_tiles[numTiles++] = b;
                }
            }
        }
    }

    // Load the image.
    //m_tileTexture = new dxf::Texture(device);
    //if (!m_tileTexture->load2DTexture(context, tileImage)) 
    //{
    //    return ;
    //}
}
    
void Ground::validateTiling(int w, int h)
{
    for (int x = 0; x < h; ++x)
    {
        for (int z = 0; z < w; ++z)
        {
            int nColor = -1; 
            if (x > 0)
            {
                int index = m_tiling[(x - 1) * w + z];
                nColor = m_tiles[index].colors[2];
            }
            int wColor = -1;
            if (z > 0)
            {
                int index = m_tiling[x * w + z - 1];
                wColor = m_tiles[index].colors[1];
            }
            
            if (nColor != -1) 
            {
                DXF_ASSERT(nColor == m_tiles[m_tiling[x * w + z]].colors[0]);
            }
            if (wColor != -1)
            {
                DXF_ASSERT(wColor == m_tiles[m_tiling[x * w + z]].colors[3]);
            }
        }
    }
}
        
int Ground::getTileIndex(int n, int e, int s, int w)
{
	int nIndex = n;
	int eIndex = e;
	int sIndex = s;
	int wIndex = w;

	return nIndex * (NUM_HCOLORS * NUM_HCOLORS * NUM_VCOLORS) + 
		   eIndex * (NUM_HCOLORS * NUM_VCOLORS) + 
		   sIndex * (NUM_HCOLORS) + 
		   wIndex;
}
