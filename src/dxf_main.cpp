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

#include "dxf_abstract_renderer.h"
#include "dxf_abstract_control.h"

#include "DXUT/Core/DXUT.h"
#include "DXUT/Optional/SDKmisc.h"
#include "DXUT/Optional/DXUTsettingsDlg.h"
#include "DXUT/Optional/DXUTgui.h"

#include "dxf_assert.h"

static dxf::AbstractRenderer*              g_renderer = NULL;
static dxf::AbstractControl*               g_control = NULL;
static CDXUTDialogResourceManager          g_dialogResourceManager;    // Manager for shared resources of dialogs
static CD3DSettingsDlg                     g_d3dSettingsDlg;           // Device settings dialog
static CDXUTDialog                         g_HUD;                      // Manages the 3D HUD
static CDXUTDialog                         g_sampleUI;                 // Dialog for sample specific controls
static CDXUTTextHelper*                    g_txtHelper;

enum 
{
    IDC_TOGGLEFULLSCREEN = 0xffffde00,
    IDC_TOGGLEREF        = 0xffffde01,
    IDC_CHANGEDEVICE     = 0xffffde02,
};

bool CALLBACK IsD3D11DeviceAcceptable(const CD3D11EnumAdapterInfo* AdapterInfo, 
                                      UINT Output, 
                                      const CD3D11EnumDeviceInfo* DeviceInfo,
                                      DXGI_FORMAT BackBufferFormat, 
                                      bool bWindowed, 
                                      void* pUserContext)
{
    return true;
}

static void CALLBACK onGUIEvent(UINT nEvent, int nControlID, CDXUTControl* pControl, void* pUserContext)
{
    switch (nControlID)
    {
        // Standard DXUT controls
        case IDC_TOGGLEFULLSCREEN:  
            DXUTToggleFullScreen();
            break;
        case IDC_TOGGLEREF:         
            DXUTToggleREF();
            break;
        case IDC_CHANGEDEVICE:      
            g_d3dSettingsDlg.SetActive(!g_d3dSettingsDlg.IsActive());
            break;
        default:
            dxf::getControl()->onEvent(nEvent, nControlID, pControl);
            break;
    }
}

HRESULT CALLBACK OnD3D11CreateDevice(ID3D11Device* pd3dDevice, 
                                     const DXGI_SURFACE_DESC* pBufferSurfaceDesc,
                                     void* pUserContext)
{
    HRESULT hr;

    DXF_ASSERT(g_renderer == NULL);
    DXF_ASSERT(g_control == NULL);

    g_renderer = dxf::createRenderer();
    g_control  = dxf::createControl();
    
    ID3D11DeviceContext* pd3dImmediateContext = DXUTGetD3D11DeviceContext();
    V_RETURN(g_dialogResourceManager.OnD3D11CreateDevice(pd3dDevice, pd3dImmediateContext));
    V_RETURN(g_d3dSettingsDlg.OnD3D11CreateDevice(pd3dDevice));
    g_txtHelper = new CDXUTTextHelper(pd3dDevice, pd3dImmediateContext, &g_dialogResourceManager, 15);
    
    V_RETURN(dxf::getControl()->initialize(pd3dDevice, &g_sampleUI));
    V_RETURN(dxf::getRenderer()->initialize(pd3dDevice, pd3dImmediateContext, g_txtHelper));

    return S_OK;
}

HRESULT CALLBACK OnD3D11ResizedSwapChain(ID3D11Device* pd3dDevice, 
                                         IDXGISwapChain* pSwapChain,
                                         const DXGI_SURFACE_DESC* pBufferSurfaceDesc, 
                                         void* pUserContext)
{
    HRESULT hr;

    V_RETURN(g_dialogResourceManager.OnD3D11ResizedSwapChain(pd3dDevice, pBufferSurfaceDesc));
    V_RETURN(g_d3dSettingsDlg.OnD3D11ResizedSwapChain(pd3dDevice, pBufferSurfaceDesc));
    
    dxf::getControl()->resize(pBufferSurfaceDesc->Width, pBufferSurfaceDesc->Height);
    dxf::getRenderer()->resize(pBufferSurfaceDesc->Width, pBufferSurfaceDesc->Height);

    g_HUD.SetLocation(pBufferSurfaceDesc->Width - 170, 0);
    g_HUD.SetSize(170, 170);
    g_sampleUI.SetLocation(pBufferSurfaceDesc->Width - 245, pBufferSurfaceDesc->Height - 520);
    g_sampleUI.SetSize(245, 520);

    return S_OK;
}

void CALLBACK OnD3D11ReleasingSwapChain(void* pUserContext)
{
    g_dialogResourceManager.OnD3D11ReleasingSwapChain();
}


void CALLBACK OnD3D11DestroyDevice(void* pUserContext)
{
    dxf::getRenderer()->uninitialize();
    dxf::getControl()->uninitialize();

    SAFE_DELETE(g_renderer);
    SAFE_DELETE(g_control);

    g_dialogResourceManager.OnD3D11DestroyDevice();
    g_d3dSettingsDlg.OnD3D11DestroyDevice();
    DXUTGetGlobalResourceCache().OnDestroyDevice();

    SAFE_DELETE(g_txtHelper);
}

void CALLBACK OnFrameMove(double fTime, float fElapsedTime, void* pUserContext)
{
    dxf::getRenderer()->update(fTime, fElapsedTime);
}

void CALLBACK OnD3D11FrameRender(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext, 
    double fTime, float fElapsedTime, void* pUserContext)
{
    // If the settings dialog is being shown, then render it instead of rendering the app's scene
    if (g_d3dSettingsDlg.IsActive())
    {
        g_d3dSettingsDlg.OnRender(fElapsedTime);
        return;
    }
    
    dxf::getRenderer()->render(fTime, fElapsedTime);
    
    g_HUD.OnRender(fElapsedTime);
    g_sampleUI.OnRender(fElapsedTime);
        
    // Always render text info 
    dxf::getRenderer()->renderText(fTime, fElapsedTime);
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

    if (dxf::getRenderer() != NULL)
    {
        dxf::getRenderer()->msgproc(hWnd, uMsg, wParam, lParam);
    }

    return 0;
}


//--------------------------------------------------------------------------------------
// Handle key presses
//--------------------------------------------------------------------------------------
void CALLBACK KeyboardProc(UINT nChar, bool bKeyDown, bool bAltDown, void* pUserContext)
{
    dxf::getRenderer()->keyboard(nChar, bKeyDown, bAltDown);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
#if defined(DEBUG) | defined(_DEBUG)
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

    g_d3dSettingsDlg.Init(&g_dialogResourceManager);
    g_HUD.Init(&g_dialogResourceManager);
    g_sampleUI.Init(&g_dialogResourceManager);
    g_sampleUI.SetCallback(onGUIEvent);

    int iY = 10; 
    g_HUD.AddButton(IDC_TOGGLEFULLSCREEN, L"Toggle full screen", 35, iY, 125, 22 );
    g_HUD.AddButton(IDC_TOGGLEREF, L"Toggle REF (F3)", 35, iY += 24, 125, 22, VK_F3 );
    g_HUD.AddButton(IDC_CHANGEDEVICE, L"Change device (F2)", 35, iY += 24, 125, 22, VK_F2 );
    g_HUD.SetCallback(onGUIEvent);


    dxf::GlobalProperties properties;
    dxf::fillProperties(&properties);


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

    DXUTCreateWindow(properties.applicationName);
    DXUTCreateDevice(D3D_FEATURE_LEVEL_11_0, true, properties.width, properties.height);
    DXUTMainLoop(); // Enter into the DXUT render loop

    return DXUTGetExitCode();
}

DXF_NAMESPACE_BEGIN

AbstractRenderer* DXF_APIENTRY getRenderer()
{
    return g_renderer;
}

AbstractControl* DXF_APIENTRY getControl()
{
    return g_control;
}

DXF_NAMESPACE_END
