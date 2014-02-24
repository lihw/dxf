// -------------------------------------------------------------- 
// dxf_shader.h
// Shader
//
// A DirectX11 framework.
//
// All rights reserved by AMD.
//
// Hongwei Li (hongwei.li@amd.com)
// -------------------------------------------------------------- 

#ifndef DXF_SHADER_H
#define DXF_SHADER_H

#include "dxf_common.h"

DXF_NAMESPACE_BEGIN

class Shader
{
public:
    Shader(ID3D11Device* device);
    ~Shader();

    void setMacros(const D3D_SHADER_MACRO* defines);
    void setFlags(UINT flags); 
    // TODO: set row major and column major.
    HRESULT addVSShader(LPCWSTR vsShaderFile, LPCSTR mainEntry);
    HRESULT addPSShader(LPCWSTR psShaderFile, LPCSTR mainEntry);
    HRESULT addHSShader(LPCWSTR hsShaderFile, LPCSTR mainEntry);
    HRESULT addDSShader(LPCWSTR dsShaderFile, LPCSTR mainEntry);
    HRESULT addGSShader(LPCWSTR gsShaderFile, LPCSTR mainEntry);

    ID3D11VertexShader* vertexShader() { return m_vsShader; }
    ID3D11PixelShader* pixelShader() { return m_psShader; } 
    ID3D11HullShader* hullShader() { return m_hsShader; } 
    ID3D11DomainShader* domainShader() { return m_dsShader; }
    ID3D11GeometryShader* geometryShader() { return m_gsShader; }
    ID3DBlob* vertexShaderBlob() { return m_vsShaderBlob; }

    void bind(ID3D11DeviceContext* context);

private:

private:
    ID3D11Device*           m_device;
    ID3D11VertexShader*     m_vsShader;
    ID3D11PixelShader*      m_psShader;
    ID3D11HullShader*       m_hsShader;
    ID3D11DomainShader*     m_dsShader;
    ID3D11GeometryShader*   m_gsShader;
    ID3DBlob*               m_vsShaderBlob;
};


DXF_NAMESPACE_END


#endif // !DXF_SHADER_H
