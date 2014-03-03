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
#include "util/glm.h"
#include "util/xyz.h"
#include "dxf_shader.h"
#include "dxf_assert.h"
#include "dxf_log.h"

//#include <assimp/importer.hpp>
//#include <assimp/scene.h>
//#include <assimp/postprocess.h>


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

    GLMmodel* model = glmReadOBJ((char* )filename);
    DXF_ASSERT(model != NULL);
    if (model == NULL)
    {
        DXF_LOGERROR("Failed to load mesh %s.", filename);
        return S_FALSE;
    }

    // Normalize the model
    glmUnitize(model);
    
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

    if (model->numnormals != 0)
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

    if (model->numtexcoords != 0)
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
    
    m_numVertices = model->numvertices;
    m_numIndices  = model->numtriangles * 3;
    
    m_vertices = new float [vertexSize * m_numVertices];
    m_indices = new UINT [m_numIndices];

    // We suppose the normal index and texture coordinate
    // index are the same as vertex index. 
    for (UINT i = 0; i < m_numVertices; ++i)
    {
        float* v = &m_vertices[i * vertexSize];
        
        *(v++) = model->vertices[i * 3 + 3];
        *(v++) = model->vertices[i * 3 + 4];
        *(v++) = model->vertices[i * 3 + 5];

        if (model->numnormals)
        {
            *(v++) = model->normals[i * 3 + 3];
            *(v++) = model->normals[i * 3 + 4];
            *(v++) = model->normals[i * 3 + 5];
        }

        if (model->numtexcoords)
        {
            *(v++) = model->texcoords[i * 2 + 2];
            *(v++) = model->texcoords[i * 2 + 3];
        }
    }

    for (UINT i = 0; i < model->numtriangles; ++i)
    {
        m_indices[i * 3 + 0] = model->triangles[i].vindices[0] - 1;
        m_indices[i * 3 + 1] = model->triangles[i].vindices[1] - 1;
        m_indices[i * 3 + 2] = model->triangles[i].vindices[2] - 1;
    }
    
    glmDelete(model);

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

HRESULT Model::loadXYZ(const char* filename, Shader* shader)
{
    m_topology = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;

    XYZModel* model = xyzRead((char* )filename);
    DXF_ASSERT(model != NULL);
    if (model == NULL)
    {
        DXF_LOGERROR("Failed to load mesh %s.", filename);
        return S_FALSE;
    }

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

    if (model->numnormals != 0)
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

    m_numVertices = model->numvertices;
    m_numIndices  = 0;
    
    m_vertices = new float [vertexSize * m_numVertices];
    m_indices = NULL;

    // We suppose the normal index and texture coordinate
    // index are the same as vertex index. 
    for (UINT i = 0; i < m_numVertices; ++i)
    {
        float* v = &m_vertices[i * vertexSize];
        
        *(v++) = model->vertices[i * 3 + 3];
        *(v++) = model->vertices[i * 3 + 4];
        *(v++) = model->vertices[i * 3 + 5];

        if (model->numnormals)
        {
            *(v++) = model->normals[i * 3 + 3];
            *(v++) = model->normals[i * 3 + 4];
            *(v++) = model->normals[i * 3 + 5];
        }
    }

    xyzDelete(model);

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

HRESULT Model::loadSphere(UINT numSegments, UINT numRings, Shader* shader)
{
    m_topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

    // -------------------------------------------------------------- 
    // Create the sphere vertex and index buffer
    // -------------------------------------------------------------- 
    UINT numVertices = (numRings + 1) * (numSegments + 1);
    UINT numIndices = 6 * numRings * (numSegments + 1);

    m_vertices = new float [numVertices * 8];
    m_indices = new UINT [numIndices];

    m_numVertices = numVertices;
    m_numIndices = numIndices;

    float *pVertex = m_vertices;
    UINT *pIndex = m_indices;

    float deltaRingAngle = DirectX::XM_PI / numRings;
    float deltaSegAngle = (2.0f * DirectX::XM_PI / numSegments);

    UINT verticeIndex = 0;

    // Generate the group of rings for the sphere
    for (UINT ring = 0; ring <= numRings; ++ring) 
    {
        float r0 = sinf(ring * deltaRingAngle);
        float y0 = cosf(ring * deltaRingAngle);

        // Generate the group of segments for the current ring
        for(UINT seg = 0; seg <= numSegments; seg++) 
        {
            float x0 = r0 * sinf(seg * deltaSegAngle);
            float z0 = r0 * cosf(seg * deltaSegAngle);

            // Add one vertex to the strip which makes up the sphere
            
            // Position.
            *pVertex++ = x0;
            *pVertex++ = y0;
            *pVertex++ = z0;

            // Normal
            DirectX::XMVECTOR n = DirectX::XMVector3Normalize(DirectX::XMVectorSet(x0, y0, z0, 1.0f)); 
            *pVertex++ = DirectX::XMVectorGetX(n);
            *pVertex++ = DirectX::XMVectorGetY(n);
            *pVertex++ = DirectX::XMVectorGetZ(n);

            // Texture coordinate
            *pVertex++ = (float)seg / (float)numSegments;
            *pVertex++ = (float)ring / (float)numRings;

            if (ring != numRings) 
            {
                // each vertex (except the last) has six indices pointing to it
                *pIndex++ = verticeIndex + numSegments;
                *pIndex++ = verticeIndex + numSegments + 1;
                *pIndex++ = verticeIndex;               

                *pIndex++ = verticeIndex;
                *pIndex++ = verticeIndex + numSegments + 1;
                *pIndex++ = verticeIndex + 1;

                verticeIndex++;
            }
        } // end for seg
    } // end for ring

    // Position
    D3D11_INPUT_ELEMENT_DESC vertexElements[3]; 

    vertexElements[0].SemanticName         = "POSITION";
    vertexElements[0].SemanticIndex        = 0;
    vertexElements[0].Format               = DXGI_FORMAT_R32G32B32_FLOAT;
    vertexElements[0].InputSlot            = 0;
    vertexElements[0].AlignedByteOffset    = 0;
    vertexElements[0].InputSlotClass       = D3D11_INPUT_PER_VERTEX_DATA;
    vertexElements[0].InstanceDataStepRate = 0;

    vertexElements[1].SemanticName         = "NORMAL";
    vertexElements[1].SemanticIndex        = 0;
    vertexElements[1].Format               = DXGI_FORMAT_R32G32B32_FLOAT;
    vertexElements[1].InputSlot            = 0;
    vertexElements[1].AlignedByteOffset    = 12;
    vertexElements[1].InputSlotClass       = D3D11_INPUT_PER_VERTEX_DATA;
    vertexElements[1].InstanceDataStepRate = 0;

    vertexElements[2].SemanticName         = "TEXCOORD";
    vertexElements[2].SemanticIndex        = 0;
    vertexElements[2].Format               = DXGI_FORMAT_R32G32_FLOAT;
    vertexElements[2].InputSlot            = 0;
    vertexElements[2].AlignedByteOffset    = 24;
    vertexElements[2].InputSlotClass       = D3D11_INPUT_PER_VERTEX_DATA;
    vertexElements[2].InstanceDataStepRate = 0;

    m_stride = 32;

    if (!createVertexBuffer(m_device))
    {
        return S_FALSE;
    }

    if (FAILED(m_device->CreateInputLayout(vertexElements, 
                    3,
                    shader->vertexShaderBlob()->GetBufferPointer(), 
                    shader->vertexShaderBlob()->GetBufferSize(), 
                    &m_vertexLayout)))
    {
        return false;
    }

    DXUT_SetDebugName(m_vertexLayout, "sphere");

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
        context->Draw(m_numVertices, 0);
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
