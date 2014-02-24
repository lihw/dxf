// -------------------------------------------------------------- 
// dxf_light.h
// The data structure of real-time lights
//
// A DirectX11 framework.
//
// All rights reserved by AMD.
//
// Hongwei Li (hongwei.li@amd.com)
// -------------------------------------------------------------- 

#ifndef DXF_LIGHT_H
#define DXF_LIGHT_H

#include "dxf_common.h"

#include <directxmath.h>

DXF_NAMESPACE_BEGIN

enum LightTypeEnum
{
    LIGHT_NONE        = 0,
    LIGHT_POINT       = 1,
    LIGHT_DIRECTIONAL = 2,
    LIGHT_SPOT        = 3,

    LIGHT_TYPE_FIRST  = LIGHT_NONE,
    LIGHT_TYPE_LAST   = LIGHT_SPOT,
    LIGHT_TYPE_NUMBER = LIGHT_TYPE_LAST - LIGHT_TYPE_FIRST + 1,
};

struct AbstractLight
{
protected:
    AbstractLight();

public:
    DirectX::XMFLOAT4      ambient;
    DirectX::XMFLOAT4      diffuse;
    DirectX::XMFLOAT4      specular;
    DirectX::XMFLOAT3      attenuation;
    float                  shininess;
    DirectX::XMFLOAT3      position;
    float                  padding1;   // Not used.
};

struct PointLight : public AbstractLight
{
    PointLight();
};

struct DirectionalLight : public AbstractLight
{
    DirectionalLight();
};

struct SpotLight : public AbstractLight
{
    SpotLight();

    DirectX::XMFLOAT3   spotDirection;
    float               spotCutoff;
    float               spotExponent;
    float               padding2[3];   // Not used.
};


DXF_NAMESPACE_END

#endif // !DXF_LIGHT_H
