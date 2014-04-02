// ground.h
//
// Created at 2014/03/31
//
// Hongwei Li hongwei.li@amd.com
// All rights reserved

#ifndef GROUND_H
#define GROUND_H

#include <dxf/dxf.h>


struct Block
{
    UINT colors[4]; // NESW
    UINT row; // The row index of the block in the tile image
    UINT col; // The row index of the block in the tile image
};

class Ground 
{
public:
    Ground();
    ~Ground();

    HRESULT initialize(ID3D11Device* device, ID3D11DeviceContext* context,
		const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& direction);

    void update(const DirectX::XMFLOAT3& position,
                const DirectX::XMFLOAT3& direction,
				CModelViewerCamera* camera);

    void render(ID3D11DeviceContext* context);

private:
    void loadTiles(ID3D11Device* device,
                   ID3D11DeviceContext* context,
                   const char* tileConfiguration,
                   const char* tileImage);

    bool isInsideTriangle(const float triangle[][2], float x, float z);

    void validateTiling(int w, int h);

	int getTileIndex(int n, int e, int s, int w);

private:
    dxf::Model* m_block;
    UINT m_numActiveBlocks;
    struct CbInitialStruct
    {
        DirectX::XMFLOAT4 tilingUV;
    };
    struct CbEveryFrameStruct   
    {
        UINT tiling[100];
        DirectX::XMMATRIX mvp[100];
    };
    dxf::CBuffer<CbInitialStruct>*     m_cbInitial;
    dxf::CBuffer<CbEveryFrameStruct>*  m_cbEveryFrame;
    dxf::Shader*                       m_shader;
    dxf::Texture*                      m_tileTexture;
    dxf::Sampler*                      m_tileSampler;
    int                                m_bb[4]; //
    int                                m_tiling[100];
    Block                              m_tiles[36];
};

#endif // !GROUND_H
