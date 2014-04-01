// -------------------------------------------------------------- 
// dxf_texture.cpp
// Texture
//
// A DirectX11 framework.
//
// All rights reserved by AMD.
//
// Hongwei Li (hongwei.li@amd.com)
// -------------------------------------------------------------- 

#include "dxf_texture.h"

#include "dxf_assert.h"
#include "util\image.h"

#include "DXUT/Core/DXUT.h"
#include "DirectXTex.h"
#include "WICTextureLoader.h"

#pragma comment(lib, "DirectXTex_vs2012_win32.lib")
#pragma comment(lib, "WICTextureLoader_vs2012_win32.lib")

DXF_NAMESPACE_BEGIN

//
// Sampler
//
Sampler::Sampler(ID3D11Device* device)
{
    m_device = device;
    m_sampler = NULL;
}

Sampler::~Sampler()
{
    SAFE_RELEASE(m_sampler);
}

void Sampler::create(D3D11_FILTER filter,
                D3D11_TEXTURE_ADDRESS_MODE wrapU,
                D3D11_TEXTURE_ADDRESS_MODE wrapV,
                D3D11_TEXTURE_ADDRESS_MODE wrapW)
{
    D3D11_SAMPLER_DESC samplerDesc;

    samplerDesc.Filter = filter;
    samplerDesc.AddressU = wrapU;
    samplerDesc.AddressV = wrapV;
    samplerDesc.AddressW = wrapW;
    samplerDesc.MipLODBias = 0.0f;
    samplerDesc.MaxAnisotropy = 1;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
    samplerDesc.BorderColor[0] = 0;
    samplerDesc.BorderColor[1] = 0;
    samplerDesc.BorderColor[2] = 0;
    samplerDesc.BorderColor[3] = 0;
    samplerDesc.MinLOD = 0;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

    HRESULT hr = m_device->CreateSamplerState(&samplerDesc, &m_sampler);
    DXF_ASSERT(hr == S_OK);
}

void Sampler::bind(ID3D11DeviceContext* context, UINT slot, UINT shaders)
{
    DXF_ASSERT(context != NULL);
    DXF_ASSERT(m_sampler != NULL);

    if (shaders & PIXEL_SHADER_BIT)
    {
        context->PSSetSamplers(slot, 1, &m_sampler);
    }
    if (shaders & VERTEX_SHADER_BIT)
    {
        context->VSSetSamplers(slot, 1, &m_sampler);
    }
}


//
// Texture
//

Texture::Texture(ID3D11Device* device)
{
    DXF_ASSERT(device != NULL);
    m_device = device;
    m_texture = NULL;
    m_textureSRV = NULL;
}

Texture::~Texture()
{
    SAFE_RELEASE(m_texture);
    SAFE_RELEASE(m_textureSRV);
}

HRESULT Texture::load2DTexture(ID3D11DeviceContext* context, const char* path)
{
	wchar_t	wpath[256];
    swprintf(wpath, L"%hs", path);
    HRESULT hr;
	
	V_RETURN(DirectX::CreateWICTextureFromFile(m_device, context, wpath, &m_texture, &m_textureSRV));

	const char* suffix = strrchr(path, '.');
    DXUT_SetDebugName(m_textureSRV, suffix + 1);

    return S_OK;
}

void Texture::bind(ID3D11DeviceContext* context, UINT slot, UINT shaders)
{
    DXF_ASSERT(context != NULL);
    DXF_ASSERT(m_textureSRV != NULL);

    if (shaders & PIXEL_SHADER_BIT)
    {
        context->PSSetShaderResources(slot, 1, &m_textureSRV);
    }
    if (shaders & VERTEX_SHADER_BIT)
    {
        context->VSSetShaderResources(slot, 1, &m_textureSRV);
    }
}


DXF_NAMESPACE_END
