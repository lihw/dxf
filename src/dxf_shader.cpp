// -------------------------------------------------------------- 
// dxf_shader.cpp
// Model
//
// A DirectX11 framework.
//
// All rights reserved by AMD.
//
// Hongwei Li (hongwei.li@amd.com)
// -------------------------------------------------------------- 

#include "dxf_shader.h"

#include "dxf_assert.h"
#include "dxut/core/dxut.h"

DXF_NAMESPACE_BEGIN

Shader::Shader(ID3D11Device *device)
{
    DXF_ASSERT(device != NULL);
    m_device = device;

    m_vsShader = NULL;
    m_psShader = NULL;
    m_hsShader = NULL;
    m_dsShader = NULL;
    m_gsShader = NULL;
}

Shader::~Shader()
{
    SAFE_RELEASE(m_vsShader);
    SAFE_RELEASE(m_psShader);
    SAFE_RELEASE(m_hsShader);
    SAFE_RELEASE(m_dsShader);
    SAFE_RELEASE(m_gsShader);
    SAFE_RELEASE(m_vsShaderBlob);
}

void Shader::setMacros(const D3D_SHADER_MACRO *defines)
{
}

void Shader::setFlags(UINT flags)
{
}

HRESULT Shader::addVSShader(LPCWSTR vsShaderFile, LPCSTR mainEntry)
{
    HRESULT   hr = S_OK;
    ID3DBlob* pShaderBlob = NULL;
    ID3DBlob* pErrorBlob = NULL;
    
    DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined DXF_DEBUG 
    dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

    hr = D3DCompileFromFile(vsShaderFile, 
                            NULL, 
                            NULL, 
                            mainEntry, 
                            "vs_5_0", 
                            dwShaderFlags, 
                            0, 
                            &pShaderBlob, 
                            &pErrorBlob);
    if (FAILED(hr))
    {
        OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());
        SAFE_RELEASE(pErrorBlob);
        return hr;
    }
            
    hr = m_device->CreateVertexShader(pShaderBlob->GetBufferPointer(), 
                                      pShaderBlob->GetBufferSize(), 
                                      NULL, 
                                      &m_vsShader);

    m_vsShaderBlob = pShaderBlob;
    DXUT_SetDebugName(m_vsShader, mainEntry);

    return hr;
}

HRESULT Shader::addPSShader(LPCWSTR psShaderFile, LPCSTR mainEntry)
{
    HRESULT   hr = S_OK;
    ID3DBlob* pShaderBlob = NULL;
    ID3DBlob* pErrorBlob = NULL;
    
    DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined DXF_DEBUG 
    dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

    hr = D3DCompileFromFile(psShaderFile, 
                            NULL, 
                            NULL, 
                            mainEntry, 
                            "ps_5_0", 
                            dwShaderFlags, 
                            0, 
                            &pShaderBlob, 
                            &pErrorBlob);
    if (FAILED(hr))
    {
        OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());
        SAFE_RELEASE(pErrorBlob);
        return hr;
    }
            
    hr = m_device->CreatePixelShader(pShaderBlob->GetBufferPointer(), 
                                     pShaderBlob->GetBufferSize(), 
                                     NULL, 
                                     &m_psShader);

    pShaderBlob->Release();
    DXUT_SetDebugName(m_psShader, mainEntry);

    return hr;
}

HRESULT Shader::addHSShader(LPCWSTR hsShaderFile, LPCSTR mainEntry)
{
    HRESULT   hr = S_OK;
    ID3DBlob* pShaderBlob = NULL;
    ID3DBlob* pErrorBlob = NULL;
    
    DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined DXF_DEBUG 
    dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

    hr = D3DCompileFromFile(hsShaderFile, 
                            NULL, 
                            NULL, 
                            mainEntry, 
                            "hs_5_0", 
                            dwShaderFlags, 
                            0, 
                            &pShaderBlob, 
                            &pErrorBlob);
    if (FAILED(hr))
    {
        OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());
        SAFE_RELEASE(pErrorBlob);
        return hr;
    }
            
    hr = m_device->CreateHullShader(pShaderBlob->GetBufferPointer(), 
                                    pShaderBlob->GetBufferSize(), 
                                    NULL, 
                                    &m_hsShader);

    pShaderBlob->Release();
    DXUT_SetDebugName(m_hsShader, mainEntry);

    return hr;
}

HRESULT Shader::addDSShader(LPCWSTR dsShaderFile, LPCSTR mainEntry)
{
    HRESULT   hr = S_OK;
    ID3DBlob* pShaderBlob = NULL;
    ID3DBlob* pErrorBlob = NULL;

    DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined DXF_DEBUG 
    dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

    hr = D3DCompileFromFile(dsShaderFile, 
                            NULL, 
                            NULL, 
                            mainEntry, 
                            "ds_5_0", 
                            dwShaderFlags, 
                            0, 
                            &pShaderBlob, 
                            &pErrorBlob);
    if (FAILED(hr))
    {
        OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());
        SAFE_RELEASE(pErrorBlob);
        return hr;
    }
            
    hr = m_device->CreateDomainShader(pShaderBlob->GetBufferPointer(), 
                                      pShaderBlob->GetBufferSize(), 
                                      NULL, 
                                      &m_dsShader);

    pShaderBlob->Release();
    DXUT_SetDebugName(m_dsShader, mainEntry);

    return hr;
}

HRESULT Shader::addGSShader(LPCWSTR gsShaderFile, LPCSTR mainEntry)
{
    HRESULT   hr = S_OK;
    ID3DBlob* pShaderBlob = NULL;
    ID3DBlob* pErrorBlob = NULL;
    
    DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined DXF_DEBUG 
    dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

    hr = D3DCompileFromFile(gsShaderFile, 
                            NULL, 
                            NULL, 
                            mainEntry, 
                            "gs_5_0", 
                            dwShaderFlags, 
                            0, 
                            &pShaderBlob, 
                            &pErrorBlob);
    if (FAILED(hr))
    {
        OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());
        SAFE_RELEASE(pErrorBlob);
        return hr;
    }
            
    hr = m_device->CreateGeometryShader(pShaderBlob->GetBufferPointer(), 
                                        pShaderBlob->GetBufferSize(), 
                                        NULL, 
                                        &m_gsShader);

    pShaderBlob->Release();
    DXUT_SetDebugName(m_gsShader, mainEntry);

    return hr;
}
    
void Shader::bind(ID3D11DeviceContext *context)
{
    DXF_ASSERT(context != NULL);
    
    context->VSSetShader(m_vsShader, NULL, 0);
    context->PSSetShader(m_psShader, NULL, 0);
    context->HSSetShader(m_hsShader, NULL, 0);
    context->DSSetShader(m_dsShader, NULL, 0);
    context->GSSetShader(m_gsShader, NULL, 0);
}


DXF_NAMESPACE_END
