// -------------------------------------------------------------- 
// dxf_light.cpp
// The data structure of real-time lights
//
// A DirectX11 framework.
//
// All rights reserved by AMD.
//
// Hongwei Li (hongwei.li@amd.com)
// -------------------------------------------------------------- 

#include "dxf_light.h"

DXF_NAMESPACE_BEGIN

AbstractLight::AbstractLight()
    : position(0, 0, 0)
    , ambient(0, 0, 0, 0)
    , diffuse(0, 0, 0, 0)
    , specular(0, 0, 0, 0)
    , attenuation(0, 0, 0)
{
    shininess   = 1.0f;
}

PointLight::PointLight()
{
}

DirectionalLight::DirectionalLight()
{
}

SpotLight::SpotLight()
    : spotDirection(0, 0, 0)
{
    spotCutoff       = 45.0f;
    spotExponent     = 0.0f;
}

DXF_NAMESPACE_END
