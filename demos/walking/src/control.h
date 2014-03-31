// control.h
//
// Created at 2014/02/20
//
// Hongwei Li hongwei.li@amd.com
// All rights reserved

#ifndef CONTROL_H
#define CONTROL_H

#include <dxf/dxf.h>

class Control : public dxf::AbstractControl
{
public:
    Control();
    virtual ~Control();

    virtual HRESULT initialize(ID3D11Device* device, CDXUTDialog* ui);
    virtual void uninitialize();
    virtual void onEvent(UINT nEvent, int nControlID, CDXUTControl* pControl);
    virtual void resize(UINT width, UINT height);

private:
};

#endif // !CONTROL_H
