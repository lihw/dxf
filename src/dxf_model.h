// -------------------------------------------------------------- 
// dxf_model.h
// Model
//
// A DirectX11 framework.
//
// All rights reserved by AMD.
//
// Hongwei Li (hongwei.li@amd.com)
// -------------------------------------------------------------- 

#ifndef DXF_MODEL_H
#define DXF_MODEL_H

#include "dxf_common.h"

DXF_NAMESPACE_BEGIN

class Shader;

class Model
{
public:
    Model(ID3D11Device* device);
    ~Model();

    HRESULT loadObj(const char* filename, Shader* shader);
    HRESULT loadXYZ(const char* filename, Shader* shader);
    HRESULT loadSphere(UINT numSegments, UINT numRings, Shader* shader);

    void render(ID3D11DeviceContext* context);

protected:
    bool createVertexBuffer(ID3D11Device *device);

protected:
    ID3D11Device*             m_device;
    ID3D11InputLayout*        m_vertexLayout;
    ID3D11Buffer*             m_vertexBuffer;
    ID3D11Buffer*             m_indexBuffer;
    UINT                      m_stride;
    float*                    m_vertices;
    UINT                      m_numVertices;
    UINT*                     m_indices;
    UINT                      m_numIndices;
    D3D11_PRIMITIVE_TOPOLOGY  m_topology;
};


DXF_NAMESPACE_END


#endif // !DXF_MODEL_H
