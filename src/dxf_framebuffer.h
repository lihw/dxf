// -------------------------------------------------------------- 
// dxf_framebuffer.h
// DirectX framebuffer
//
// A DirectX11 framework.
//
// All rights reserved by AMD.
//
// Hongwei Li (hongwei.li@amd.com)
// -------------------------------------------------------------- 


#ifndef DXF_FRAMEBUFFER_H
#define DXF_FRAMEBUFFER_H

#include "dxf_common.h"

DXF_NAMESPACE_BEGIN

class FrameBuffer
{
public:
    FrameBuffer(ID3D11Device* device);
    ~FrameBuffer();

    HRESULT create(UINT width, 
                   UINT height,
                   UINT colorFormat,
                   UINT depthFormat,
                   UINT stencilFormat);

    void use(ID3D11DeviceContext* context);

private:
    ID3D11Device* m_device;
    ID3D11Texture2D* m_colorTexture;
    ID3D11Texture2D* m_depthStencilTexture;
    ID3D11RenderTargetView* m_renderTargetView;
    ID3D11DepthStencilView* m_depthStencilView;
    ID3D11ShaderResourceView* m_colorSRV;
    ID3D11ShaderResourceView* m_depthStencilSRV;
};


DXF_NAMESPACE_END




#endif // !DXF_FRAMEBUFFER_H
