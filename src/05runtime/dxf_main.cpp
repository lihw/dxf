// -------------------------------------------------------------- 
// dxf_main.cpp
// The main entry of a single thread directx demo.
//
// A DirectX11 framework.
//
// All rights reserved by AMD.
//
// Hongwei Li (hongwei.li@amd.com)
// -------------------------------------------------------------- 

#include "dxf_main.h"

#include "dxf_renderer.h"
#include "dxf_control.h"

#include "DXUT/Core/DXUT.h"
#include "DXUT/Optional/SDKmisc.h"
#include "DXUT/Optional/DXUTsettingsDlg.h"
#include "DXUT/Optional/DXUTgui.h"


static dxf::Renderer*                      g_renderer = NULL;
static dxf::Control*                       g_control = NULL;
static CDXUTDialogResourceManager          g_dialogResourceManager;    // Manager for shared resources of dialogs
static CD3DSettingsDlg                     g_d3DSettingsDlg;           // Device settings dialog
static CDXUTDialog                         g_HUD;                      // Manages the 3D HUD
static CDXUTDialog                         g_sampleUI;                 // Dialog for sample specific controls
static CDXUTTextHelper*                    g_txtHelper;


bool CALLBACK IsD3D11DeviceAcceptable(const CD3D11EnumAdapterInfo* AdapterInfo, 
                                      UINT Output, 
                                      const CD3D11EnumDeviceInfo* DeviceInfo,
                                      DXGI_FORMAT BackBufferFormat, 
                                      bool bWindowed, 
                                      void* pUserContext)
{
    return true;
}

HRESULT CALLBACK OnD3D11CreateDevice(ID3D11Device* pd3dDevice, 
                                     const DXGI_SURFACE_DESC* pBufferSurfaceDesc,
                                     void* pUserContext)
{
    HRESULT hr;

    V_RETURN(dxf::getMainWindow()->onD3D11CreateDevice(pd3dDevice));

    g_d3dSettingsDlg.Init(&g_dialogResourceManager);
    g_HUD.Init(&g_dialogResourceManager);
    g_sampleUI.Init(&g_DialogResourceManager);
    
    ID3D11DeviceContext* pd3dImmediateContext = DXUTGetD3D11DeviceContext();
    V_RETURN(g_dialogResourceManager.OnD3D11CreateDevice(pDevice, pd3dImmediateContext));
    V_RETURN(g_d3dSettingsDlg.OnD3D11CreateDevice(pDevice));
    g_txtHelper = new CDXUTTextHelper(pDevice, pd3dImmediateContext, &g_dialogResourceManager, 15);
    
    V_RETURN(dxf::getMainControl()->intialize(pd3dDevice, &g_sampleUI));
    V_RETURN(dxf::getMainRenderer()->intialize(pd3dDevice, &g_txtHelper));

    return S_OK;
}

HRESULT CALLBACK OnD3D11ResizedSwapChain(ID3D11Device* pd3dDevice, 
                                         IDXGISwapChain* pSwapChain,
                                         const DXGI_SURFACE_DESC* pBufferSurfaceDesc, 
                                         void* pUserContext)
{
    HRESULT hr;

    V_RETURN(g_dialogResourceManager.OnD3D11ResizedSwapChain(pDevice, pBufferSurfaceDesc));
    V_RETURN(g_d3dSettingsDlg.OnD3D11ResizedSwapChain(pDevice, pBufferSurfaceDesc));
    
    V_RETURN(dxf::getMainControl()->resize(pBufferSurfaceDesc->Width, pBufferSurfaceDesc->Height));
    V_RETURN(dxf::getMainRenderer()->resize(pBufferSurfaceDesc->Width, pBufferSurfaceDesc->Height));

    return S_OK;
}

void CALLBACK OnD3D11ReleasingSwapChain(void* pUserContext)
{
    g_dialogResourceManager.OnD3D10ReleasingSwapChain();
}


void CALLBACK OnD3D11DestroyDevice(void* pUserContext)
{
    dxf::getMainWindow()->uninitialize();
    dxf::getMainControl()->uninitialize();

    g_dialogResourceManager.OnD3D11DestroyDevice();
    g_d3dSettingsDlg.OnD3D11DestroyDevice();
    DXUTGetGlobalResourceCache().OnDestroyDevice();

    SAFE_DELETE(g_txtHelper);
}

void CALLBACK OnFrameMove(double fTime, float fElapsedTime, void* pUserContext)
{
    dxf::getMainRenderer()->update(fTime, fElapsedTime);
}

void CALLBACK OnD3D11FrameRender(ID3D11Device* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext)
{
    // If the settings dialog is being shown, then render it instead of rendering the app's scene
    if (g_d3dSettingsDlg.IsActive())
    {
        g_d3dSettingsDlg.OnRender(fElapsedTime);
        return;
    }
    
    dxf::getRenderer()->render(pd3dDevice, fTime, fElapsedTime);
    
    g_HUD.OnRender( fElapsedTime );
    g_SampleUI.OnRender( fElapsedTime );
        
    // Always render text info 
    dxf::getRenderer()->renderText(pd3dDevice, fTime, fElapsedTime);
}

bool CALLBACK ModifyDeviceSettings(DXUTDeviceSettings* pDeviceSettings, void* pUserContext)
{
    return true;
}

//--------------------------------------------------------------------------------------
// Before handling window messages, DXUT passes incoming windows 
// messages to the application through this callback function. If the application sets 
// *pbNoFurtherProcessing to TRUE, then DXUT will not process this message.
//--------------------------------------------------------------------------------------
LRESULT CALLBACK MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing, void* pUserContext)
{
    // Pass messages to dialog resource manager calls so GUI state is updated correctly
    *pbNoFurtherProcessing = g_dialogResourceManager.MsgProc(hWnd, uMsg, wParam, lParam);
    if (*pbNoFurtherProcessing)
    {
        return 0;
    }

    // Pass messages to settings dialog if its active
    if (g_d3dSettingsDlg.IsActive())
    {
        g_d3dSettingsDlg.MsgProc(hWnd, uMsg, wParam, lParam);
        return 0;
    }

    // Give the dialogs a chance to handle the message first
    *pbNoFurtherProcessing = g_HUD.MsgProc(hWnd, uMsg, wParam, lParam);
    if (*pbNoFurtherProcessing)
    {
        return 0;
    }

    // TODO: mouse event to renderer

    return 0;
}


//--------------------------------------------------------------------------------------
// Handle key presses
//--------------------------------------------------------------------------------------
void CALLBACK KeyboardProc(UINT nChar, bool bKeyDown, bool bAltDown, void* pUserContext)
{
    !dxf::getControl()->keyboard(nChar, bKeyDown, bAltDown) && 
        !dxf::getControl()->keyboard(nChar, bKeyDown, bAltDown);
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPWSTR, int)
{
#if defined(DEBUG) | defined(_DEBUG)
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

    dxf::GlobalProperties properties;
    dxfFillProperties(&properties);
    g_renderer = dxfCreateRenderer();
    g_control  = dxfCreateControl();

    DXUTSetCallbackD3D11DeviceAcceptable(IsD3D11DeviceAcceptable);
    DXUTSetCallbackD3D11DeviceCreated(OnD3D11CreateDevice);
    DXUTSetCallbackD3D11SwapChainResized(OnD3D11ResizedSwapChain);
    DXUTSetCallbackD3D11SwapChainReleasing(OnD3D11ReleasingSwapChain);
    DXUTSetCallbackD3D11DeviceDestroyed(OnD3D11DestroyDevice);
    DXUTSetCallbackD3D11FrameRender(OnD3D11FrameRender);
    
    DXUTSetCallbackMsgProc(MsgProc);
    DXUTSetCallbackKeyboard(KeyboardProc);
    DXUTSetCallbackFrameMove(OnFrameMove);
    DXUTSetCallbackDeviceChanging(ModifyDeviceSettings);
    
    DXUTInit(true, true, NULL); // Parse the command line, show msgboxes on error, no extra command line params
    DXUTSetCursorSettings(true, true); // Show the cursor and clip it when in full screen

    DXUTCreateWindow(properties.name);
    DXUTCreateDevice(true, properties.width, properties.height);
    DXUTMainLoop(); // Enter into the DXUT render loop

    SAFE_DELETE(g_renderer);
    SAFE_DELETE(g_control);

    return DXUTGetExitCode();
}

AbstractRenderer* DXF_APIENTRY getRenderer()
{
    return g_renderer;
}

AbstractRenderer* DXF_APIENTRY getControl()
{
    return g_control;
}
