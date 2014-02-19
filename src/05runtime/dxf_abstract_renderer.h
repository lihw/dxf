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

#include "../dxf_common.h"


DXF_NAMESPACE_BEGIN

class AbstractRenderer
{
    AbstractRenderer(const AbstractRenderer &other) {}
    void operator=(const AbstractRenderer &other) {}

protected:
    AbstractRenderer();

public:
    virtual ~AbstractRenderer();
    
    virtual HRESULT initialize(ID3D11Device* device, CDXUTTextHelper *txtHelper);
    virtual void uninitialize();
    virtual void render(ID3D11Device* device, 
                        double fTime, 
                        float fElapsedTime);
    virtual void renderText(ID3D11Device* device, 
                            double fTime, 
                            float fElapsedTime);
    virtual void update(double fTime, fElapsedTime);
    virtual HRESULT resize(UINT width, UINI height);
    virtual void keyboard(UINT c, 
                          bool bKeyDown, 
                          bool bAltDown);
    // TODO: mouse

protected:
    CDXUTTextHelper* m_txtHelper;
};



DXF_NAMESPACE_END



#endif // !DXF_WINDOW_H
