// -------------------------------------------------------------- 
// dxf_texture.h
// Texture
//
// A DirectX11 framework.
//
// All rights reserved by AMD.
//
// Hongwei Li (hongwei.li@amd.com)
// -------------------------------------------------------------- 

#ifndef DXF_TEXTURE_H
#define DXF_TEXTURE_H

#include "dxf_common.h"

DXF_NAMESPACE_BEGIN

class Sampler
{
public:
    Sampler(ID3D11Device* device);
    ~Sampler();

    void create(D3D11_FILTER filter,
                D3D11_TEXTURE_ADDRESS_MODE wrapU,
                D3D11_TEXTURE_ADDRESS_MODE wrapV,
                D3D11_TEXTURE_ADDRESS_MODE wrapW);

    void bind(ID3D11DeviceContext* context, UINT slot, UINT shaders);
    
private:
	ID3D11Device* m_device;
	ID3D11SamplerState* m_sampler;
};

class Texture
{
public:
    Texture(ID3D11Device* device);
    ~Texture();

    HRESULT load2DTexture(ID3D11DeviceContext* context, const char* path);

    void bind(ID3D11DeviceContext* context, UINT slot, UINT shaders);

protected:
    ID3D11Device*             m_device;
    ID3D11Resource*           m_texture;
    ID3D11ShaderResourceView* m_textureSRV;
};


DXF_NAMESPACE_END


#endif // !DXF_TEXTURE_H
