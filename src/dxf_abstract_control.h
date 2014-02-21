// -------------------------------------------------------------- 
// dxf_abstract_control.h
// The control of dx demo application.
//
// A DirectX11 framework.
//
// All rights reserved by AMD.
//
// Hongwei Li (hongwei.li@amd.com)
// -------------------------------------------------------------- 

#ifndef DXF_ABSTRACT_CONTROL_H
#define DXF_ABSTRACT_CONTROL_H

#include "dxf_common.h"

#include "DXUT/Core/DXUT.h"
#include "DXUT/Optional/SDKmisc.h"
#include "DXUT/Optional/DXUTsettingsDlg.h"
#include "DXUT/Optional/DXUTgui.h"


DXF_NAMESPACE_BEGIN

class AbstractControl
{
    AbstractControl(const AbstractControl& other) {}
    void operator=(const AbstractControl& other) {}

protected:
    AbstractControl();

public:
    virtual ~AbstractControl();

    virtual HRESULT initialize(ID3D11Device* device, CDXUTDialog* ui) = 0;
    virtual void uninitialize() = 0;
    virtual void onEvent(UINT nEvent, int nControlID, CDXUTControl* pControl) = 0;
    virtual void resize(UINT width, UINT height) = 0;
};

DXF_NAMESPACE_END



#endif // !DXF_ABSTRACT_CONTROL_H
