// -------------------------------------------------------------- 
// dxf_framebuffer.cpp
// DirectX framebuffer
//
// A DirectX11 framework.
//
// All rights reserved by AMD.
//
// Hongwei Li (hongwei.li@amd.com)
// -------------------------------------------------------------- 

#include "dxf_framebuffer.h"


DXF_NAMESPACE_BEGIN

FrameBuffer::FrameBuffer(ID3D11Device* device)
{
    m_device = device;
    m_colorTexture = NULL;
    m_depthStencilTexture = NULL;
    m_renderTargetView = NULL;
    m_depthStencilView = NULL;
    m_colorSRV = NULL;
    m_depthStencilSRV = NULL;
}

FrameBuffer::~FrameBuffer()
{
    SAFE_RELEASE(m_colorTexture);
    SAFE_RELEASE(m_depthStencilSRV);
    SAFE_RELEASE(m_renderTargetView);
    SAFE_RELEASE(m_depthStencilView);
    SAFE_RELEASE(m_colorSRV);
    SAFE_RELEASE(m_depthStencilSRV);
}

HRESULT FrameBuffer::create(UINT width, 
               UINT height,
               UINT colorFormat,
               UINT depthFormat,
               UINT stencilFormat)
{
    HRESULT result;

    // Initialize the render target texture description.
    D3D11_TEXTURE2D_DESC textureDesc;
    ZeroMemory(&textureDesc, sizeof(textureDesc));

    // Setup the render target texture description.
    textureDesc.Width = width;
    textureDesc.Height = height;
    textureDesc.MipLevels = 1;
    textureDesc.ArraySize = 1;
    textureDesc.Format = colorFormat;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.Usage = D3D11_USAGE_DEFAULT;
    textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    textureDesc.CPUAccessFlags = 0;
    textureDesc.MiscFlags = 0;

    // Create the render target texture.
    V_RETURN(m_device->CreateTexture2D(&textureDesc, NULL, &m_colorTexture));

    D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;

    renderTargetViewDesc.Format = textureDesc.Format;
    renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    renderTargetViewDesc.Texture2D.MipSlice = 0;

    V_RETURN(device->CreateRenderTargetView(m_colorTexture, &renderTargetViewDesc, &m_renderTargetView));

    // Initialize the depth stencil texture descritpion
    D3D11_TEXTURE2D_DESC descDepth;
    descDepth.Width = width;
    descDepth.Height = height;
    descDepth.MipLevels = 1;
    descDepth.ArraySize = 1;
    descDepth.Format = DXGI_FORMAT_D32_FLOAT;
    descDepth.SampleDesc.Count = 1;
    descDepth.SampleDesc.Quality = 0;
    descDepth.Usage = D3D10_USAGE_DEFAULT;
    descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    descDepth.CPUAccessFlags = 0;
    descDepth.MiscFlags = 0;
    V_RESULT(m_device->CreateTexture2D(&descDepth, NULL, &m_depthStencilTexture));

    // Create the depth stencil view
    D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
    descDSV.Format = descDepth.Format;
    descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    descDSV.Texture2D.MipSlice = 0;
    V_RESULT(m_device->CreateDepthStencilView( g_pDepthStencil, &descDSV, &m_depthStencilView));
    
    // Setup the description of the shader resource view.
    D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;

    srvDesc.Format = textureDesc.Format;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.MipLevels = 1;

    // Create the shader resource view.
    V_RETURN(device->CreateShaderResourceView(m_colorTexture, &srcDesc, &m_colorSRV));
    
    srvDesc.Format = descDepth.Format;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.MipLevels = 1;

    V_RETURN(device->CreateShaderResourceView(m_renderTargetTexture, &srvDesc, &m_depthStencilSRV));
}

void FrameBuffer::use(ID3D11DeviceContext* context)
{
    context->OMSetRenderTarget(1, &m_renderTargetView, m_depthStencilView);
}

DXF_NAMESPACE_END
