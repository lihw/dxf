// -------------------------------------------------------------- 
// dxf_cbuffer.h
// Constant buffer
//
// A DirectX11 framework.
//
// All rights reserved by AMD.
//
// Hongwei Li (hongwei.li@amd.com)
// -------------------------------------------------------------- 

#ifndef DXF_CBUFFER_H
#define DXF_CBUFFER_H

#include "dxf_common.h"

DXF_NAMESPACE_BEGIN

template<typename T>
class CBuffer
{
public:
    CBuffer(ID3D11Device* device);
    ~CBuffer();

    HRESULT create(ID3D11DeviceContext* context,
                   LPCSTR name,
                   LPCSTR shader,
                   UINT   slot);
    void sync(ID3D11DeviceContext* context);
    T& data() { return m_data; };

private:
    void uploadVS(ID3D11DeviceContext* context);
    void uploadPS(ID3D11DeviceContext* context);
    void uploadHS(ID3D11DeviceContext* context);
    void uploadDS(ID3D11DeviceContext* context);
    void uploadGS(ID3D11DeviceContext* context);

    typedef void (CBuffer<T>::*UploadFunction)(ID3D11DeviceContext* context);

private:
    ID3D11Device*   m_device;
    ID3D11Buffer*   m_buffer;
    T               m_data;
    UploadFunction  m_function;
    UINT            m_slot;
};

template<typename T>
CBuffer<T>::CBuffer(ID3D11Device* device)
{
    DXF_ASSERT(device != NULL);
    m_device = device;
    m_buffer = NULL;
    // The constant buffer data must be 64-bit aligned.
    DXF_ASSERT(sizeof(T) % 16 == 0);
    m_slot = 0;
}

template<typename T>
CBuffer<T>::~CBuffer()
{
    SAFE_RELEASE(m_buffer);
}

template<typename T>
HRESULT CBuffer<T>::create(ID3D11DeviceContext* context,
                           LPCSTR name,
                           LPCSTR shader,
                           UINT   slot)
{
    m_slot = slot;

    HRESULT hr;
    D3D11_BUFFER_DESC bd;
    ZeroMemory(&bd, sizeof(bd));

    bd.Usage          = D3D11_USAGE_DEFAULT;
    bd.BindFlags      = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = 0;
    bd.ByteWidth      = sizeof(T);

    V_RETURN(m_device->CreateBuffer(&bd, NULL, &m_buffer));
    DXUT_SetDebugName(m_buffer, name);

    if (strcmp(shader, "vs") == 0)
    {
        m_function = &CBuffer<T>::uploadVS;
    }
    else if (strcmp(shader, "ps") == 0)
    {
        m_function = &CBuffer<T>::uploadPS;
    }
    else if (strcmp(shader, "hs") == 0)
    {
        m_function = &CBuffer<T>::uploadHS;
    }
    else if (strcmp(shader, "ds") == 0)
    {
        m_function = &CBuffer<T>::uploadDS;
    }
    else if (strcmp(shader, "gs") == 0)
    {
        m_function = &CBuffer<T>::uploadGS;
    }
    else
    {
        OutputDebugString(L"Invalid shader to bind this constant buffer.\n");
        SAFE_RELEASE(m_buffer);
        return S_FALSE;
    }
    
    return S_OK;
}
    
template<typename T>
void CBuffer<T>::sync(ID3D11DeviceContext* context)
{
    DXF_ASSERT(context != NULL);
    DXF_ASSERT(m_buffer != NULL);
    context->UpdateSubresource(m_buffer, 0, NULL, &m_data, 0, 0);

    (this->*m_function)(context);
}

template<typename T>
void CBuffer<T>::uploadVS(ID3D11DeviceContext* context)
{
    context->VSSetConstantBuffers(m_slot, 1, &m_buffer);
}

template<typename T>
void CBuffer<T>::uploadPS(ID3D11DeviceContext* context)
{
    context->PSSetConstantBuffers(m_slot, 1, &m_buffer);
}

template<typename T>
void CBuffer<T>::uploadGS(ID3D11DeviceContext* context)
{
    context->GSSetConstantBuffers(m_slot, 1, &m_buffer);
}

template<typename T>
void CBuffer<T>::uploadHS(ID3D11DeviceContext* context)
{
    context->HSSetConstantBuffers(m_slot, 1, &m_buffer);
}

template<typename T>
void CBuffer<T>::uploadDS(ID3D11DeviceContext* context)
{
    context->DSSetConstantBuffers(m_slot, 1, &m_buffer);
}

DXF_NAMESPACE_END

#endif // !DXF_CBUFFER_H
