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

#if defined _MSC_VER || defined _WIN32
# define DXF_WIN32
#else
# error "DXFramework supports win32 only"
#endif

#if defined DXF_WIN32

#define DXF_DLLEXPORT 
#define DXF_INLINE __forceinline
#define DXF_EXTERN extern
#define DXF_CALLBACK
#define DXF_APIENTRY __stdcall
#define DXF_CCONV __cdecl

#endif // DXF_WIN32

#endif // !DXF_COMMON_H

