// renderer.cpp
//
// Created at 2014/02/20
//
// Hongwei Li hongwei.li@amd.com
// All rights reserved


#include "renderer.h"

#include <directxmath.h>
#include <directxcolors.h>


Renderer::Renderer()
    : AbstractRenderer()
{
    m_sphereShader = NULL;
    m_pointsShader = NULL;
    m_sphere = NULL;
    m_points = NULL;
    m_cbInit = NULL;
    m_cbEveryFrame = NULL;
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
    // shaders
    //
#define SHADER_ROOT L"../demos/hemisampling/media/shaders"
    m_sphereShader = new dxf::Shader(m_device);
    V_RETURN(m_sphereShader->addVSShader(SHADER_ROOT L"/hemisphere.hlsl", "VS"));
    V_RETURN(m_sphereShader->addPSShader(SHADER_ROOT L"/hemisphere.hlsl", "PS"));
#undef SHADER_ROOT 

    //
    // Models
    //
#define MODEL_ROOT "../demos/hemisampling/media/models"
    m_sphere = new dxf::Model(m_device);
    V_RETURN(m_sphere->loadSphere(64, 32, m_sphereShader));
    //m_points = new dxf::Model(m_device);
    //V_RETURN(m_points->loadXYZ(MODEL_ROOT"/points.xyz", m_pointsShader));
#undef MODEL_ROOT

    //
    // Constant buffers
    //
    m_cbEveryFrame = new dxf::CBuffer<CbEveryFrameStruct>(m_device);
    V_RETURN(m_cbEveryFrame->create(m_context, "cb-everyframe", "vs", 0));
    
    m_cbInit = new dxf::CBuffer<CbInitStruct>(m_device);
    V_RETURN(m_cbInit->create(m_context, "cb-init", "vs", 1));

    m_cbInit->data().light.position = DirectX::XMFLOAT3(-1.0f, -1.0f, -1.0f);
    m_cbInit->data().light.ambient  = DirectX::XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
    m_cbInit->data().light.diffuse  = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    m_cbInit->data().light.specular = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    m_cbInit->sync(m_context);


    //
    // Camera
    //
    DirectX::XMFLOAT3 eye(0.0f, 0.0f, -4.0f);
    DirectX::XMFLOAT3 at(0.0f, 0.0f, 0.0f);
    
    DirectX::XMVECTOR vecEye = DirectX::XMLoadFloat3(&eye);
    DirectX::XMVECTOR vecAt = DirectX::XMLoadFloat3(&at);
    
    m_camera.SetViewParams(vecEye, vecAt);

    //
    // State
    //
    D3D11_DEPTH_STENCIL_DESC dsDesc;
    ZeroMemory(&dsDesc, sizeof(dsDesc));
    dsDesc.DepthEnable = true;
    dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    dsDesc.DepthFunc = D3D11_COMPARISON_LESS;

    m_device->CreateDepthStencilState(&dsDesc, &m_dsState);
    
    return S_OK;
}

void Renderer::uninitialize()
{
    SAFE_DELETE(m_sphere);
    SAFE_DELETE(m_points);
    SAFE_DELETE(m_sphereShader);
    SAFE_DELETE(m_pointsShader);
    SAFE_RELEASE(m_dsState);
    SAFE_DELETE(m_cbEveryFrame);
    SAFE_DELETE(m_cbInit);
}

void Renderer::render(double fTime, 
                      float fElapsedTime)
{
    // Clear the back buffer 
    float ClearColor[4] = {0.0f, 0.125f, 0.3f, 1.0f}; // red,green,blue,alpha
    ID3D11RenderTargetView* pRTV = DXUTGetD3D11RenderTargetView();
    m_context->ClearRenderTargetView(pRTV, ClearColor);

    ID3D11DepthStencilView* pDSV = DXUTGetD3D11DepthStencilView();
    m_context->ClearDepthStencilView(pDSV, D3D11_CLEAR_DEPTH, 1.0, 0);

    m_sphereShader->bind(m_context);

    m_context->OMSetDepthStencilState(m_dsState, 0);

    DirectX::XMMATRIX mProj = m_camera.GetProjMatrix();
    DirectX::XMMATRIX mView = m_camera.GetViewMatrix();
    m_cbEveryFrame->data().m_mvp = XMMatrixTranspose(mView * mProj);     // convert row order to column as by default matrix in shader is column order.
    m_cbEveryFrame->sync(m_context);
    
    m_sphere->render(m_context);
}

void Renderer::renderText(double fTime, 
                          float fElapsedTime)
{
    AbstractRenderer::renderText(fTime, fElapsedTime);
}

void Renderer::update(double fTime, float fElapsedTime)
{
    m_camera.FrameMove(fElapsedTime);
}

HRESULT Renderer::resize(UINT width, UINT height)
{
    float fAspectRatio = (FLOAT)width / (FLOAT)height;
    m_camera.SetProjParams(DirectX::XM_PI / 4.0f, fAspectRatio, 0.1f, 5000.0f);
    m_camera.SetWindow(width, height);
    m_camera.SetButtonMasks(0, MOUSE_WHEEL, MOUSE_LEFT_BUTTON | MOUSE_RIGHT_BUTTON);

    return S_OK;
}

void Renderer::keyboard(UINT c, 
                        bool bKeyDown, 
                        bool bAltDown)
{
}
    
LRESULT Renderer::msgproc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    m_camera.HandleMessages(hWnd, uMsg, wParam, lParam);

    return 0;
}

