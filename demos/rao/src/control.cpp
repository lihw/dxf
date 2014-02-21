// control.cpp
//
// Created at 2014/02/20
//
// Hongwei Li hongwei.li@amd.com
// All rights reserved


#include "control.h"

DXF_NAMESPACE_BEGIN

Control::Control()
    : AbstractControl()
{
}

Control::~Control()
{
}

HRESULT Control::initialize(ID3D11Device* device, CDXUTDialog* ui)
{
    return S_OK;
}

void Control::uninitialize()
{
}

void Control::onEvent(UINT nEvent, int nControlID, CDXUTControl* pControl)
{
}

void Control::resize(UINT width, UINT height)
{
}

DXF_NAMESPACE_END
