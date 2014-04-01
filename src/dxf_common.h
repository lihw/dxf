// -------------------------------------------------------------- 
// dxf_common.h
// The common stuff about dxf
//
// An OpenGL framework.
//
// All rights reserved by AMD.
//
// Hongwei Li (hongwei.li@amd.com)
// -------------------------------------------------------------- 

#ifndef DXF_COMMON_H
#define DXF_COMMON_H

// -------------------------------------------------------------- 
// DXF
// -------------------------------------------------------------- 
#define DXF_NAMESPACE_BEGIN namespace dxf {
#define DXF_NAMESPACE_END   };

#if defined _DEBUG || defined DEBUG
# define DXF_DEBUG 1
#endif

#define DXF_UNUSED_ARGUMENT(p) ((void)p)

# include <Windows.h>


// 
// DirectX 11.1 headers
//
#include <d3dcommon.h>
#include <dxgi.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>


#define DXF_DLLEXPORT 
#define DXF_INLINE __forceinline
#define DXF_EXTERN extern
#define DXF_CALLBACK
#define DXF_APIENTRY __stdcall
#define DXF_CCONV __cdecl

enum ShaderBit
{
    VERTEX_SHADER_BIT = 0x01,
    PIXEL_SHADER_BIT = 0x02,
    HULL_SHADER_BIT = 0x04,
    DOMAIN_SHADER_BIT = 0x08,
    GEOMETRY_SHADER_BIT = 0x10,
    COMPUTE_SHADER_BIT = 0x020,
};

#endif // !DXF_COMMON_H

