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


DXF_NAMESPACE_BEGIN

AbstractRenderer::AbstractRenderer()
{
    m_txtHelper = NULL;
}

AbstractRenderer::~Abstract()
{
}

HRESULT AbstractRenderer::initialize(ID3D11Device* device, CDXUTTextHelper *txtHelper)
{
    return S_OK;
}

void AbstractRenderer::uninitialize()
{
}

void AbstractRenderer::render(ID3D11Device* device, 
                              double fTime, 
                              float fElapsedTime)
{
}

void AbstractRenderer::renderText(ID3D11Device* device, 
                                  double fTime, 
                                  float fElapsedTime)
{
    m_txtHelper->Begin();
    m_txtHelper->SetInsertionPos(2, 0 );
    m_txtHelper->SetForegroundColor(D3DXCOLOR( 1.0f, 1.0f, 0.0f, 1.0f));
    m_txtHelper->DrawTextLine(DXUTGetFrameStats(DXUTIsVsyncEnabled()));
    m_txtHelper->DrawTextLine(DXUTGetDeviceStats());
    m_txtHelper->End();
}

void AbstractRenderer::update(double fTime, fElapsedTime)
{
}

HRESULT AbstractRenderer::resize(UINT width, UINI height)
{
}

void AbstractRenderer::keyboard(UINT c, 
                                bool bKeyDown, 
                                bool bAltDown)
{
}


DXF_NAMESPACE_END
