// renderer.h
//
// Created at 2014/02/20
//
// Hongwei Li hongwei.li@amd.com
// All rights reserved


#ifndef RENDERER_H
#define RENDERER_H

#include <dxf/dxf.h>

DXF_NAMESPACE_BEGIN

class Renderer : public AbstractRenderer
{
public:
    Renderer();
    virtual ~Renderer();

    virtual HRESULT initialize(ID3D11Device* device, ID3D11DeviceContext* context, 
        CDXUTTextHelper *txtHelper);
    virtual void uninitialize();
    virtual void render(double fTime, float fElapsedTime);
    virtual void renderText(double fTime, float fElapsedTime);
    virtual void update(double fTime, float fElapsedTime);
    virtual HRESULT resize(UINT width, UINT height);
    virtual void keyboard(UINT c, bool bKeyDown, bool bAltDown);

private:
    bool initializeModels();
    bool initializeShaders();
};

DXF_NAMESPACE_END

#endif // !RENDERER_H
