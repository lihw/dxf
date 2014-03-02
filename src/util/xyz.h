// -------------------------------------------------------------- 
// xyz.h
// Read the XYZ point cloud file.
//
// A DirectX11 framework.
//
// All rights reserved by AMD.
//
// Hongwei Li (hongwei.li@amd.com)
// -------------------------------------------------------------- 

#ifndef XYZ_H
#define XYZ_H

#include <windows.h>

struct XYZModel
{
    UINT numvertices;
    UINT numnormals;

    float *vertices;
    float *normals;
};

extern XYZModel *xyzRead(const char *filename);
extern void xyzDelete(XYZModel *model);

#endif // !XYZ_H
