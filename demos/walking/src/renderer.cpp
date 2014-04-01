// renderer.cpp
//
// Created at 2014/02/20
//
// Hongwei Li hongwei.li@amd.com
// All rights reserved


#include "renderer.h"

#include "walking.h"

#include <directxmath.h>
#include <directxcolors.h>


Renderer::Renderer()
    : AbstractRenderer()
{
    m_spotlightShader = NULL;
	m_groundShader = NULL;
    m_spotlight = NULL;
	m_ground = NULL;

	m_blend = NULL;
    m_cbEveryFrame = NULL;

	m_angle = DirectX::XMConvertToRadians(180.0f);
    m_position = DirectX::XMFLOAT3(0, 1.75f, 0);
    m_direction = DirectX::XMFLOAT3(0, 0, -1.0f);

	m_mode = THIRD_PERSON;
}

Renderer::~Renderer()
{
}

HRESULT Renderer::initialize(ID3D11Device* device, 
                             ID3D11DeviceContext* context,
                             CDXUTTextHelper *txtHelper)
{
	srand(1001);

    HRESULT hr;

    AbstractRenderer::initialize(device, context, txtHelper);
    
    //
    // shaders
    //
#define SHADER_ROOT L"../demos/walking/media/shaders"
    m_groundShader = new dxf::Shader(m_device);
    V_RETURN(m_groundShader->addVSShader(SHADER_ROOT L"/ground.hlsl", "VS"));
    V_RETURN(m_groundShader->addPSShader(SHADER_ROOT L"/ground.hlsl", "PS"));
    
    m_spotlightShader = new dxf::Shader(m_device);
    V_RETURN(m_spotlightShader->addVSShader(SHADER_ROOT L"/legend.hlsl", "VS"));
    V_RETURN(m_spotlightShader->addPSShader(SHADER_ROOT L"/legend.hlsl", "PS"));
#undef SHADER_ROOT 

    //
    // Models
    //
    //m_ground = new dxf::Model(m_device);
    //V_RETURN(m_ground->loadPlane(100.0f, 100.0f, m_groundShader));
	m_ground = new Ground;
	V_RETURN(m_ground->initialize(m_device, m_context, m_position, m_direction));
    
    const float fov = SPOTLIGHT_FOV;
    float sf, cf;

    DirectX::XMScalarSinCos(&sf, &cf, DirectX::XMConvertToRadians(fov * 0.5f));

	const float r = SPOTLIGHT_RADIUS;
    m_spotlight = new dxf::Model(m_device);
    float points[] = 
    {
        0, 0, 0,
        -sf * r, 0.0f, cf * r,
        sf * r, 0.0f, cf * r,
    };
    V_RETURN(m_spotlight->loadPolygonXZ(points, 3, m_spotlightShader));

	//
    // Constant buffers
    //
    m_cbEveryFrame = new dxf::CBuffer<CbEveryFrameStruct>(m_device);
    V_RETURN(m_cbEveryFrame->create(m_context, "cb-everyframe", "vs", 0));
    
    //
    // Camera
    //
    m_camera.SetProjParams(DirectX::XMConvertToRadians(46.8f), 0.75f, 0.1f, 1000.0f);
    updateCamera();

    //
    // State
    //
    D3D11_DEPTH_STENCIL_DESC dsDesc;
    ZeroMemory(&dsDesc, sizeof(dsDesc));
    dsDesc.DepthEnable = false;
    dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    dsDesc.DepthFunc = D3D11_COMPARISON_LESS;

    m_device->CreateDepthStencilState(&dsDesc, &m_dsState);

	D3D11_BLEND_DESC blendDesc;
	ZeroMemory(&blendDesc, sizeof(blendDesc));

	D3D11_RENDER_TARGET_BLEND_DESC rtbd;
	ZeroMemory(&rtbd, sizeof(rtbd));

	rtbd.BlendEnable			 = true;
	rtbd.SrcBlend				 = D3D11_BLEND_SRC_ALPHA;
	rtbd.DestBlend				 = D3D11_BLEND_INV_SRC_ALPHA;
	rtbd.BlendOp				 = D3D11_BLEND_OP_ADD;
	rtbd.SrcBlendAlpha			 = D3D11_BLEND_SRC_ALPHA;
	rtbd.DestBlendAlpha			 = D3D11_BLEND_INV_SRC_ALPHA;
	rtbd.BlendOpAlpha			 = D3D11_BLEND_OP_ADD;
	rtbd.RenderTargetWriteMask	 = D3D11_COLOR_WRITE_ENABLE_ALL;

	blendDesc.AlphaToCoverageEnable = false;
	blendDesc.RenderTarget[0] = rtbd;

	m_device->CreateBlendState(&blendDesc, &m_blend);
    
    return S_OK;
}

void Renderer::uninitialize()
{
    SAFE_DELETE(m_ground);
    SAFE_DELETE(m_spotlight);
    SAFE_DELETE(m_groundShader);
    SAFE_DELETE(m_spotlightShader);
    SAFE_RELEASE(m_dsState);
    SAFE_DELETE(m_cbEveryFrame);
	SAFE_RELEASE(m_blend);
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

    m_groundShader->bind(m_context);

    m_context->OMSetDepthStencilState(m_dsState, 0);

    DirectX::XMMATRIX mProj = m_camera.GetProjMatrix();
    DirectX::XMMATRIX mView = m_camera.GetViewMatrix();
    //m_cbEveryFrame->data().m_mvp = XMMatrixTranspose(mView * mProj);     // convert row order to column as by default matrix in shader is column order.
    //m_cbEveryFrame->sync(m_context);
    
    //m_ground->render(m_context);
	m_ground->update(m_position, m_direction, &m_camera);
	m_ground->render(m_context);

    if (m_mode == THIRD_PERSON)
    {
		m_context->OMSetBlendState(m_blend, NULL, 0xffffffff);

        m_spotlightShader->bind(m_context);
        DirectX::XMMATRIX translation = DirectX::XMMatrixTranslation(m_position.x, 0.0f, m_position.z);
        DirectX::XMMATRIX rotation = DirectX::XMMatrixRotationY(m_angle);
    
        m_cbEveryFrame->data().m_mvp = XMMatrixTranspose(rotation * translation * mView * mProj); 
        m_cbEveryFrame->sync(m_context);

        m_spotlight->render(m_context);

		m_context->OMSetBlendState(NULL, NULL, 0xffffffff);
    }
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
	if (bKeyDown)
	{
		switch (c)
		{
			case 'W': walk(0.1f); break;
			case 'S': walk(-0.1f); break;
			case 'A': turn(-0.01f); break;
			case 'D': turn(0.01f); break;
			case 'M': 
				m_mode = (m_mode == FIRST_PERSON)? THIRD_PERSON : FIRST_PERSON;
				break;
		}

		updateCamera();
	}
}

void Renderer::walk(float distance)
{
    m_position.x += m_direction.x * distance;
	m_position.z += m_direction.z * distance;
}

void Renderer::turn(float angle)
{
	m_angle += angle;
	
	float c, s;
	DirectX::XMScalarSinCos(&s, &c, m_angle);

    m_direction.z = c;
    m_direction.x = s;
}

    
LRESULT Renderer::msgproc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    return 0;
}
    
void Renderer::updateCamera()
{
	if (m_mode == FIRST_PERSON)
	{
		DirectX::XMFLOAT3 at;
   
		at.x = m_position.x + m_direction.x * 10.0f;
		at.y = 0.0f;
		at.z = m_position.z + m_direction.z * 10.0f;

		DirectX::XMVECTOR vecEye = DirectX::XMLoadFloat3(&m_position);
		DirectX::XMVECTOR vecAt = DirectX::XMLoadFloat3(&at);
    
		m_camera.SetViewParams(vecEye, vecAt);
	}
	else
	{
		DirectX::XMFLOAT3 eye(-100.0f, 100.0f, 100.0f);
		DirectX::XMFLOAT3 at(0.0f, 0.0f, 0.0f);

		DirectX::XMVECTOR vecEye = DirectX::XMLoadFloat3(&eye);
		DirectX::XMVECTOR vecAt = DirectX::XMLoadFloat3(&at);
    
		m_camera.SetViewParams(vecEye, vecAt);
	}
}
