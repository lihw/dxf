// -------------------------------------------------------------- 
// dxf_abstract_control.cpp
// The control of dx demo application.
//
// A DirectX11 framework.
//
// All rights reserved by AMD.
//
// Hongwei Li (hongwei.li@amd.com)
// -------------------------------------------------------------- 
//

#include "dxf_abstract_control.h"

#include "dxf_main.h"


DXF_NAMESPACE_BEGIN

static void CALLBACK onGUIEvent(UINT nEvent, int nControlID, CDXUTControl* pControl, void* pUserContext)
{
    dxf::getControl()->onEvent(nEvent, nControlID, pControl);
}

AbstractControl::AbstractControl()
{
    m_d3dSettingsDlg.Init(&m_dialogResourceManager);
    m_HUD.Init(&m_dialogResourceManager);
    m_HUD.SetCallback(onGUIEvent);
}

AbstractControl::~AbstractControl()
{
}

void AbstractControl::initialize()
{
}

void AbstractControl::uninitialize()
{
}

void AbstractControl::resize(UINT width, UINT height)
{
}

DXF_NAMESPACE_END
