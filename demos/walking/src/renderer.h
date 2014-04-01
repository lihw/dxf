// renderer.h
//
// Created at 2014/02/20
//
// Hongwei Li hongwei.li@amd.com
// All rights reserved


#ifndef RENDERER_H
#define RENDERER_H

#include <dxf/dxf.h>

#include "ground.h"

class Renderer : public dxf::AbstractRenderer
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
    virtual LRESULT msgproc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
    void walk(float distance);
    void turn(float angle);
    void updateCamera();

private:
    ID3D11DepthStencilState*           m_dsState;
    //dxf::Model*                        m_ground;
    dxf::Model*                        m_spotlight;
    dxf::Shader*                       m_groundShader;
    dxf::Shader*                       m_spotlightShader;

    struct CbEveryFrameStruct   
    {
        DirectX::XMMATRIX m_mvp;
    };
    dxf::CBuffer<CbEveryFrameStruct>*  m_cbEveryFrame;
    CModelViewerCamera                 m_camera;   
	ID3D11BlendState*                  m_blend;

    DirectX::XMFLOAT3      m_position;
	DirectX::XMFLOAT3      m_direction;
    float                  m_angle;

	enum 
	{
		FIRST_PERSON,
		THIRD_PERSON,
	} m_mode;

    Ground* m_ground;
};


#endif // !RENDERER_H
