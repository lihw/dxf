//-------------------------------------------------------------- 
// dxf_abstract_renderer.h
// The window of dx demo application.
//
// A DirectX11 framework.
//
// All rights reserved by AMD.
//
// Hongwei Li (hongwei.li@amd.com)
// -------------------------------------------------------------- 

#ifndef DXF_ABSTRACT_RENDERER_H
#define DXF_ABSTRACT_RENDERER_H

#include "dxf_common.h"

#include "DXUT/Core/DXUT.h"
#include "DXUT/Optional/SDKmisc.h"
#include "DXUT/Optional/DXUTsettingsDlg.h"
#include "DXUT/Optional/DXUTgui.h"


DXF_NAMESPACE_BEGIN

class AbstractRenderer
{
    AbstractRenderer(const AbstractRenderer &other) {}
    void operator=(const AbstractRenderer &other) {}

protected:
    AbstractRenderer();

public:
    virtual ~AbstractRenderer();
    
    virtual HRESULT initialize(ID3D11Device* device, 
                               ID3D11DeviceContext* context, 
                               CDXUTTextHelper *txtHelper);
    virtual void uninitialize();
    virtual void render(double fTime, 
                        float fElapsedTime);
    virtual void renderText(double fTime, 
                            float fElapsedTime);
    virtual void update(double fTime, float fElapsedTime);
    virtual HRESULT resize(UINT width, UINT height);
    virtual void keyboard(UINT c, 
                          bool bKeyDown, 
                          bool bAltDown);
    // TODO: mouse

protected:
    HRESULT createShaderFromFile(ID3D11Device* pd3dDevice, 
                                 LPCWSTR pSrcFile, 
                                 const D3D_SHADER_MACRO* pDefines, 
                                 LPD3DINCLUDE pInclude, 
                                 LPCSTR pFunctionName, 
                                 LPCSTR pProfile, 
                                 UINT Flags1, 
                                 UINT Flags2, 
                                 ID3D11DeviceChild** ppShader, 
                                 ID3DBlob** ppShaderBlob = NULL, 
                                 BOOL bDumpShader = FALSE);

protected:
    CDXUTTextHelper*     m_txtHelper;
    ID3D11Device*        m_device;
    ID3D11DeviceContext* m_context;
};



DXF_NAMESPACE_END



#endif // !DXF_WINDOW_H
