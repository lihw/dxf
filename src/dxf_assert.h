// -------------------------------------------------------------- 
// dxf_assert.h
// Assert
//
// An DirectX framework.
//
// All rights reserved by AMD.
//
// Hongwei Li (hongwei.li@amd.com)
// -------------------------------------------------------------- 

#ifndef DXF_ASSERT_H
#define DXF_ASSERT_H

#include "dxf_common.h"

DXF_NAMESPACE_BEGIN

#define DXF_COMPILE_TIME_ASSERT(pred) switch(0) { case 0: case (pred): break; }

// The styles of the assert.
enum AssertStyleEnum
{
    DXF_ASSERT_STYLE_LOGANDEXIT,     ///< write to stdout/logcat and exit.
    DXF_ASSERT_STYLE_POPUP_BOX,      ///< use popup dialog box.
    DXF_ASSERT_STYLE_DEBUG_BREAK,    ///< OutputDebugString + debug break
    DXF_ASSERT_STYLE_DEBUG_INFO,     ///< outputDebugString

    DXF_ASSERT_STYLE_DEFAULT = DXF_ASSERT_STYLE_POPUP_BOX,
};

//
// Assert function.
// \param condition the condition string.
// \param file in which file this assert is triggered.
// \param line at which line this assert is.
// \param style how to present this assert to user.
DXF_EXTERN void DXF_APIENTRY assertBasic(const char* condition, const char* file, int line, 
    const AssertStyleEnum style);

DXF_EXTERN void DXF_APIENTRY assertInfo(const char* fmt, const char* file, int line, 
    const AssertStyleEnum style, ...);

DXF_EXTERN AssertStyleEnum g_assertStyle;

// Set the assert style in the application-wise scope. The default style is DXF_ASSERT_STYLE_DEBUG_BREAK.
DXF_EXTERN void DXF_APIENTRY assertSetStyle(AssertStyleEnum style);

#if defined DXF_DEBUG

#define DXF_ASSERT(condition) \
{ \
    if (!(condition)) \
    { \
        dxf::assertBasic((#condition), __FILE__, __LINE__, dxf::g_assertStyle); \
    } \
}

#define DXF_ASSERT_INFO(condition, fmt, ...) \
{ \
    if (!(condition)) \
    { \
        dxf::assertInfo(fmt, __FILE__, __LINE__, dxf::g_assertStyle, __VA_ARGS__); \
    } \
}

#define DXF_ASSERT_NOT_REACHABLE() \
    dxf::assertBasic("Should not reach here!", __FILE__, __LINE__, dxf::g_assertStyle); 

#define DXF_ASSERT_NOT_IMPLEMENTED() \
    dxf::assertBasic("Not implemented!", __FILE__, __LINE__, dxf::g_assertStyle); 

#else

#define DXF_ASSERT(condition)
#define DXF_ASSERT_INFO(condition, text)
#define DXF_ASSERT_NOT_REACHABLE() 
#define DXF_ASSERT_NOT_IMPLEMENTED() 

#endif // DXF_DEBUG

DXF_NAMESPACE_END


#endif // !DXF_ASSERT_H
