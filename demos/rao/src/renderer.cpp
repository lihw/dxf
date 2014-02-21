// renderer.cpp
//
// Created at 2014/02/20
//
// Hongwei Li hongwei.li@amd.com
// All rights reserved


#include "renderer.h"

DXF_NAMESPACE_BEGIN

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
    AbstractRenderer::initialize(device, context, txtHelper);
    
    if (!initializeShaders() || !initializeModels())
    {
        return S_FALSE;
    }

    return S_OK;
}

void Renderer::uninitialize()
{
}

void Renderer::render(double fTime, 
                      float fElapsedTime)
{
    // Clear the back buffer 
    float ClearColor[4] = {0.0f, 0.125f, 0.3f, 1.0f}; // red,green,blue,alpha
    ID3D11RenderTargetView* pRTV = DXUTGetD3D11RenderTargetView();
    m_context->ClearRenderTargetView(pRTV, ClearColor);
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
