// rao.cpp
//
// Created at 2014/02/20
//
// Hongwei Li hongwei.li@amd.com
// All rights reserved

#include <dxf/dxf.h>

#include "renderer.h"
#include "control.h"

DXF_NAMESPACE_BEGIN

void DXF_APIENTRY fillProperties(GlobalProperties* properties)
{
    properties->width  = 1024;
    properties->height = 768;
    properties->applicationName = L"display-hemisphere-sampling";
}

AbstractRenderer* DXF_APIENTRY createRenderer()
{
    return new Renderer();
}

AbstractControl* DXF_APIENTRY createControl()
{
    return new Control();
}

DXF_NAMESPACE_END
