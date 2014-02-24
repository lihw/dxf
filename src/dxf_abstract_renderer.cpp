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

LRESULT AbstractRenderer::msgproc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    return 0;
}

DXF_NAMESPACE_END
