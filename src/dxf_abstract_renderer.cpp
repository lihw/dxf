// -------------------------------------------------------------- 
// dxf_abstract_renderer.cpp
// The renderer of dx demo application.
//
// A DirectX11 framework.
//
// All rights reserved by AMD.
//
// Hongwei Li (hongwei.li@amd.com)
// -------------------------------------------------------------- 

#include "dxf_abstract_renderer.h"

#include "dxf_assert.h"

#include <directxmath.h>
#include <directxcolors.h>


DXF_NAMESPACE_BEGIN

AbstractRenderer::AbstractRenderer()
{
    m_txtHelper = NULL;
    m_device    = NULL;
    m_context   = NULL;
}

AbstractRenderer::~AbstractRenderer()
{
}

HRESULT AbstractRenderer::initialize(ID3D11Device* device, 
                                     ID3D11DeviceContext* context, 
                                     CDXUTTextHelper *txtHelper)
{
    DXF_ASSERT(device != NULL && context != NULL && txtHelper != NULL);
    if (device == NULL || context == NULL || txtHelper == NULL)
    {
        return S_FALSE;
    }

    m_device    = device;
    m_context   = context;
    m_txtHelper = txtHelper;

    return S_OK;
}

void AbstractRenderer::uninitialize()
{
}

void AbstractRenderer::render(double fTime, 
                              float fElapsedTime)
{
}

void AbstractRenderer::renderText(double fTime, 
                                  float fElapsedTime)
{
    m_txtHelper->Begin();
    m_txtHelper->SetInsertionPos(2, 0);
    m_txtHelper->SetForegroundColor(DirectX::XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f));
    m_txtHelper->DrawTextLine(DXUTGetFrameStats(DXUTIsVsyncEnabled()));
    m_txtHelper->DrawTextLine(DXUTGetDeviceStats());
    m_txtHelper->End();
}

void AbstractRenderer::update(double fTime, float fElapsedTime)
{
}

HRESULT AbstractRenderer::resize(UINT width, UINT height)
{
    return S_OK;
}

void AbstractRenderer::keyboard(UINT c, 
                                bool bKeyDown, 
                                bool bAltDown)
{
}

HRESULT AbstractRenderer::createShaderFromFile(ID3D11Device* pd3dDevice, 
                                               LPCWSTR pSrcFile, 
                                               const D3D_SHADER_MACRO* pDefines, 
                                               LPD3DINCLUDE pInclude, 
                                               LPCSTR pFunctionName, 
                                               LPCSTR pProfile, 
                                               UINT Flags1, 
                                               UINT Flags2, 
                                               ID3D11DeviceChild** ppShader, 
                                               ID3DBlob** ppShaderBlob, 
                                               BOOL bDumpShader)
{
    HRESULT   hr = S_OK;
    ID3DBlob* pShaderBlob = NULL;
    ID3DBlob* pErrorBlob = NULL;
    WCHAR     wcFullPath[256];
    
    DXUTFindDXSDKMediaFileCch(wcFullPath, 256, pSrcFile);
    // Compile shader into binary blob
    hr = D3DCompileFromFile(wcFullPath, pDefines, pInclude, pFunctionName, pProfile, 
                            Flags1, Flags2, &pShaderBlob, &pErrorBlob);
    if (FAILED(hr))
    {
        OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());
        SAFE_RELEASE(pErrorBlob);
        return hr;
    }
    
    // Create shader from binary blob
    if (ppShader)
    {
        hr = E_FAIL;
        if (strstr(pProfile, "vs"))
        {
            hr = pd3dDevice->CreateVertexShader(pShaderBlob->GetBufferPointer(), 
                    pShaderBlob->GetBufferSize(), NULL, (ID3D11VertexShader**)ppShader);
        }
        else if (strstr(pProfile, "hs"))
        {
            hr = pd3dDevice->CreateHullShader(pShaderBlob->GetBufferPointer(), 
                    pShaderBlob->GetBufferSize(), NULL, (ID3D11HullShader**)ppShader); 
        }
        else if (strstr(pProfile, "ds"))
        {
            hr = pd3dDevice->CreateDomainShader(pShaderBlob->GetBufferPointer(), 
                    pShaderBlob->GetBufferSize(), NULL, (ID3D11DomainShader**)ppShader);
        }
        else if (strstr(pProfile, "gs"))
        {
            hr = pd3dDevice->CreateGeometryShader(pShaderBlob->GetBufferPointer(), 
                    pShaderBlob->GetBufferSize(), NULL, (ID3D11GeometryShader**)ppShader); 
        }
        else if (strstr(pProfile, "ps"))
        {
            hr = pd3dDevice->CreatePixelShader(pShaderBlob->GetBufferPointer(), 
                    pShaderBlob->GetBufferSize(), NULL, (ID3D11PixelShader**)ppShader); 
        }
        else if (strstr(pProfile, "cs"))
        {
            hr = pd3dDevice->CreateComputeShader(pShaderBlob->GetBufferPointer(), 
                    pShaderBlob->GetBufferSize(), NULL, (ID3D11ComputeShader**)ppShader);
        }
        if (FAILED(hr))
        {
            OutputDebugString(L"Shader creation failed\n");
            SAFE_RELEASE(pErrorBlob);
            SAFE_RELEASE(pShaderBlob);
            return hr;
        }
    }

    // If blob was requested then pass it otherwise release it
    if (ppShaderBlob)
    {
        *ppShaderBlob = pShaderBlob;
    }
    else
    {
        pShaderBlob->Release();
    }

    DXUT_SetDebugName(*ppShader, pFunctionName);

    // Return error code
    return hr;
}

DXF_NAMESPACE_END
