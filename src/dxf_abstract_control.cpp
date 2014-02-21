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

AbstractControl::AbstractControl()
{
}

AbstractControl::~AbstractControl()
{
}

HRESULT AbstractControl::initialize(ID3D11Device* device, CDXUTDialog* ui)
{
    return S_OK;
}

void AbstractControl::uninitialize()
{
}

void AbstractControl::resize(UINT width, UINT height)
{
}

DXF_NAMESPACE_END
