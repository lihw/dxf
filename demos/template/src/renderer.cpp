// renderer.cpp
//
// Created at 2014/02/20
//
// Hongwei Li hongwei.li@amd.com
// All rights reserved


#include "renderer.h"

#include <directxmath.h>
#include <directxcolors.h>


DXF_NAMESPACE_BEGIN

struct SimpleVertex
{
    DirectX::XMFLOAT3 Pos;
};


Renderer::Renderer()
    : AbstractRenderer()
{
}

Renderer::~Renderer()
{
}

HRESULT Renderer::initialize(ID3D11Device* device, 
                             ID3D11DeviceContext* context,
                             CDXUTTextHelper *txtHelper)
{
    HRESULT hr;

    AbstractRenderer::initialize(device, context, txtHelper);
    
    //
    // Compile shaders
    //
    DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
    // Set the D3D10_SHADER_DEBUG flag to embed debug information in the shaders.
    // Setting this flag improves the shader debugging experience, but still allows 
    // the shaders to be optimized and to run exactly the way they will run in 
    // the release configuration of this program.
    dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

#define SHADER_ROOT L"../demos/template/media/shaders"
    ID3DBlob* pBlobVS = NULL;
    ID3D11DeviceChild* outputShader = NULL;
    V_RETURN(createShaderFromFile(device, SHADER_ROOT L"/template.hlsl", NULL, NULL, "VS",
                "vs_5_0", dwShaderFlags, 0, &outputShader, &pBlobVS));
    m_vertexShader = (ID3D11VertexShader*)outputShader;
    V_RETURN(createShaderFromFile(device, SHADER_ROOT L"/template.hlsl", NULL, NULL, "PS",
                "ps_5_0", dwShaderFlags, 0, &outputShader));
    m_pixelShader = (ID3D11PixelShader*)outputShader;
    
    //
    // Create vertex layout for scene meshes
    //
    const D3D11_INPUT_ELEMENT_DESC vertexElements[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    V_RETURN(m_device->CreateInputLayout(vertexElements, ARRAYSIZE(vertexElements), 
                pBlobVS->GetBufferPointer(), pBlobVS->GetBufferSize(), &m_vertexLayout));
    SAFE_RELEASE(pBlobVS);
    DXUT_SetDebugName(m_vertexLayout, "Primary");

    m_context->IASetInputLayout(m_vertexLayout);
    
    
    //
    // Create models
    //
   
    SimpleVertex vertices[] =
    {
        DirectX::XMFLOAT3(0.0f, 0.5f, 0.5f),
        DirectX::XMFLOAT3(0.5f, -0.5f, 0.5f),
        DirectX::XMFLOAT3(-0.5f, -0.5f, 0.5f),
    };
    D3D11_BUFFER_DESC bd;
    ZeroMemory( &bd, sizeof(bd) );
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(SimpleVertex) * 3;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA InitData;
    ZeroMemory(&InitData, sizeof(InitData));
    InitData.pSysMem = vertices;
    V_RETURN(m_device->CreateBuffer(&bd, &InitData, &m_vertexBuffer));
    if ( FAILED( hr ) )
        return hr;

    // Set vertex buffer
    UINT stride = sizeof(SimpleVertex);
    UINT offset = 0;
    m_context->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

    // Set primitive topology
    m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    D3D11_DEPTH_STENCIL_DESC dsDesc;
    ZeroMemory(&dsDesc, sizeof(dsDesc));
    dsDesc.DepthEnable = false;
    dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    dsDesc.DepthFunc = D3D11_COMPARISON_LESS;

    m_device->CreateDepthStencilState(&dsDesc, &m_dsState);
    
    return S_OK;
}

void Renderer::uninitialize()
{
    SAFE_RELEASE(m_vertexBuffer);
    SAFE_RELEASE(m_vertexLayout);
    SAFE_RELEASE(m_vertexShader);
    SAFE_RELEASE(m_pixelShader);
    SAFE_RELEASE(m_dsState);
}

void Renderer::render(double fTime, 
                      float fElapsedTime)
{
    // Clear the back buffer 
    float ClearColor[4] = {0.0f, 0.125f, 0.3f, 1.0f}; // red,green,blue,alpha
    ID3D11RenderTargetView* pRTV = DXUTGetD3D11RenderTargetView();
    m_context->ClearRenderTargetView(pRTV, ClearColor);

    UINT stride = sizeof(SimpleVertex);
    UINT offset = 0;
    m_context->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
    m_context->IASetInputLayout(m_vertexLayout);
    m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    m_context->VSSetShader(m_vertexShader, NULL, 0);
    m_context->PSSetShader(m_pixelShader, NULL, 0);
    m_context->HSSetSamplers(NULL, 0, 0);
    m_context->DSSetSamplers(NULL, 0, 0);
    m_context->GSSetSamplers(NULL, 0, 0);

    m_context->OMSetDepthStencilState(m_dsState, 0);
    
    m_context->Draw(3, 0);
}

void Renderer::renderText(double fTime, 
                          float fElapsedTime)
{
    AbstractRenderer::renderText(fTime, fElapsedTime);
}

void Renderer::update(double fTime, float fElapsedTime)
{
}

HRESULT Renderer::resize(UINT width, UINT height)
{
    return S_OK;
}

void Renderer::keyboard(UINT c, 
                        bool bKeyDown, 
                        bool bAltDown)
{
}

DXF_NAMESPACE_END
