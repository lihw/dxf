// -------------------------------------------------------------- 
// xyz.cpp
// Read the XYZ point cloud file.
//
// A DirectX11 framework.
//
// All rights reserved by AMD.
//
// Hongwei Li (hongwei.li@amd.com)
// -------------------------------------------------------------- 

#include "xyz.h"

#include <vector>


XYZModel *readXYZ(const char *filename)
{
    FILE *fp = NULL;
    fp = fopen(filename, "rb");
    if (fp == NULL)
    {
        fprintf(stderr, "Failed to open %s.", filename);
        return NULL;
    }
    

    std::vector<float> vertices;
    std::vector<float> normals;

    char buffer[1024];
    float x, y, z;
    float nx, ny, nz;
    int line = 1;
    while (fgets(buffer, 1024, fp))
    {
        size_t n = sscanf(buffer, "%f %f %f %f %f %f", &x, &y, &z, &nx, &ny, &nz);
        if (n == 3)
        {
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);
        }
        else if (n == 6)
        {
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);
            
            normals.push_back(nx);
            normals.push_back(ny);
            normals.push_back(nz);
        }
        else
        {
            fprintf(stderr, "The file corrupted at line %d.");
            return NULL;
        }

        line++;
    }

    fclose(fp);
    
    XYZModel *model = new XYZModel();
    model->numnormals  = normals.size() / 3;
    model->numvertices = vertices.size() / 3;

    model->vertices = new float [vertices.size()];
    memcpy(model->vertices, &vertices[0], sizeof(float) * vertices.size());
    if (normals.size() == 0)
    {
        model->normals = new float [normals.size()];
        memcpy(model->normals, &normals[0], sizeof(float) * normals.size());
    }
    else
    {
        model->normals = NULL;
    }

    return model;
}

void xyzDelete(XYZModel *model)
{
    if (model != NULL)
    {
        delete [] model->vertices;
        delete [] model->normals;
        delete model;
    }
}
