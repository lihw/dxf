// -------------------------------------------------------------- 
// dxf_model.cpp
// Model
//
// A DirectX11 framework.
//
// All rights reserved by AMD.
//
// Hongwei Li (hongwei.li@amd.com)
// -------------------------------------------------------------- 

#include "dxf_model.h"

#include "DXUT/core/dxut.h"
#include "dxf_shader.h"
#include "dxf_assert.h"
#include "dxf_log.h"

#include <assimp/importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


DXF_NAMESPACE_BEGIN
    
Model::Model(ID3D11Device* device)
{
    DXF_ASSERT(device != NULL);
    m_device = device;

    m_vertices = NULL;
    m_indices  = NULL;
    m_vertexLayout = NULL;
    m_vertexBuffer = NULL;
    m_indexBuffer  = NULL;
}

Model::~Model()
{
    SAFE_DELETE(m_vertices);
    SAFE_DELETE(m_indices);
    SAFE_RELEASE(m_vertexLayout);
    SAFE_RELEASE(m_indexBuffer);
    SAFE_RELEASE(m_vertexBuffer);
}

HRESULT Model::loadObj(const char* filename, Shader* shader)
{
    m_topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

    Assimp::Importer importer; 
    const aiScene* scene = importer.ReadFile(filename,  aiProcess_Triangulate | aiProcess_SortByPType); 
    if (!scene)
    {
        DXF_LOGERROR("Failed to load mesh %s.", filename);
        return S_FALSE;
    }
    
    // FIXME: only load the first mesh from the scene
    aiMesh* mesh = scene->mMeshes[0];

    UINT vertexElementIndex = 0;

    D3D11_INPUT_ELEMENT_DESC vertexElements[3]; 
    vertexElements[vertexElementIndex].SemanticName         = "POSITION";
    vertexElements[vertexElementIndex].SemanticIndex        = 0;
    vertexElements[vertexElementIndex].Format               = DXGI_FORMAT_R32G32B32_FLOAT;
    vertexElements[vertexElementIndex].InputSlot            = 0;
    vertexElements[vertexElementIndex].AlignedByteOffset    = 0;
    vertexElements[vertexElementIndex].InputSlotClass       = D3D11_INPUT_PER_VERTEX_DATA;
    vertexElements[vertexElementIndex].InstanceDataStepRate = 0;
    vertexElementIndex += 1;

    UINT numAttributes = 1;
    UINT vertexSize = 3;
    m_stride = 12;

    if (mesh->HasNormals())
    {
        numAttributes++;
        vertexSize += 3;
    
        vertexElements[vertexElementIndex].SemanticName         = "NORMAL";
        vertexElements[vertexElementIndex].SemanticIndex        = 0;
        vertexElements[vertexElementIndex].Format               = DXGI_FORMAT_R32G32B32_FLOAT;
        vertexElements[vertexElementIndex].InputSlot            = 0;
        vertexElements[vertexElementIndex].AlignedByteOffset    = m_stride;
        vertexElements[vertexElementIndex].InputSlotClass       = D3D11_INPUT_PER_VERTEX_DATA;
        vertexElements[vertexElementIndex].InstanceDataStepRate = 0;

        vertexElementIndex += 1;

        m_stride += 12;
    }

    if (mesh->HasTextureCoords(0))
    {
        numAttributes++;
        vertexSize += 2;
        
        vertexElements[vertexElementIndex].SemanticName         = "TEXCOORD";
        vertexElements[vertexElementIndex].SemanticIndex        = 0;
        vertexElements[vertexElementIndex].Format               = DXGI_FORMAT_R32G32_FLOAT;
        vertexElements[vertexElementIndex].InputSlot            = 0;
        vertexElements[vertexElementIndex].AlignedByteOffset    = m_stride;
        vertexElements[vertexElementIndex].InputSlotClass       = D3D11_INPUT_PER_VERTEX_DATA;
        vertexElements[vertexElementIndex].InstanceDataStepRate = 0;

        m_stride += 8;
    }
    
    m_numVertices = mesh->mNumVertices;
    m_numIndices  = mesh->mNumFaces * 3;
    
    m_vertices = new float [vertexSize * m_numVertices];

    // We suppose the normal index and texture coordinate
    // index are the same as vertex index. 
    for (UINT i = 0; i < m_numVertices; ++i)
    {
        float* v = &m_vertices[i * vertexSize];
        
        *(v++) = mesh->mVertices[i][0];
        *(v++) = mesh->mVertices[i][1];
        *(v++) = mesh->mVertices[i][2];

        if (mesh->HasNormals())
        {
            *(v++) = mesh->mNormals[i][0];
            *(v++) = mesh->mNormals[i][1];
            *(v++) = mesh->mNormals[i][2];
        }

        if (mesh->HasTextureCoords(0))
        {
            *(v++) = mesh->mTextureCoords[0][i][0];
            *(v++) = mesh->mTextureCoords[0][i][1];
        }
    }

    m_indices = new UINT [m_numIndices];
    for (UINT i = 0 ; i < mesh->mNumFaces; ++i)
    {
        int j = i * 3;
        m_indices[j + 0] = mesh->mFaces[i].mIndices[0]; 
        m_indices[j + 1] = mesh->mFaces[i].mIndices[1]; 
        m_indices[j + 2] = mesh->mFaces[i].mIndices[2]; 
    }

    if (!createVertexBuffer(m_device))
    {
        return S_FALSE;
    }

    if (FAILED(m_device->CreateInputLayout(vertexElements, 
                    numAttributes,
                    shader->vertexShaderBlob()->GetBufferPointer(), 
                    shader->vertexShaderBlob()->GetBufferSize(), 
                    &m_vertexLayout)))
    {
        return false;
    }

    DXUT_SetDebugName(m_vertexLayout, filename);

    return S_OK;
}

void Model::render(ID3D11DeviceContext* context)
{
    context->IASetPrimitiveTopology(m_topology);
    context->IASetInputLayout(m_vertexLayout);

    UINT offset = 0;
    context->IASetVertexBuffers(0, 1, &m_vertexBuffer, &m_stride, &offset);

    if (m_indexBuffer != NULL)
    {
        context->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
        context->DrawIndexed(m_numIndices, 0, 0);
    }
    else
    {
        context->Draw(0, 0);
    }
}

bool Model::createVertexBuffer(ID3D11Device *device)
{
    D3D11_BUFFER_DESC bd;
    D3D11_SUBRESOURCE_DATA initData;

    //
    // Vertex buffer
    //
    ZeroMemory(&bd, sizeof(bd));
    bd.Usage          = D3D11_USAGE_DEFAULT;
    bd.ByteWidth      = m_stride * m_numVertices;
    bd.BindFlags      = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;
    
    ZeroMemory(&initData, sizeof(initData));
    initData.pSysMem = m_vertices;

    if (FAILED(device->CreateBuffer(&bd, &initData, &m_vertexBuffer)))
    {
        return false;
    }

    //
    // index buffer
    //
    if (m_indices != NULL)
    {
        ZeroMemory(&bd, sizeof(bd));
        bd.Usage          = D3D11_USAGE_DEFAULT;
        bd.ByteWidth      = sizeof(UINT) * m_numIndices;
        bd.BindFlags      = D3D11_BIND_INDEX_BUFFER;
        bd.CPUAccessFlags = 0;
        bd.MiscFlags = 0;
        
        ZeroMemory(&initData, sizeof(initData));
        initData.pSysMem = m_indices;

        if (FAILED(device->CreateBuffer(&bd, &initData, &m_indexBuffer)))
        {
            return false;
        }
    }

    SAFE_DELETE(m_vertices);
    SAFE_DELETE(m_indices);

    return true;
}

DXF_NAMESPACE_END
